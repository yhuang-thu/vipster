#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>

#include <iostream>

#include <glwrapper.h>
#include <molecule.h>
#include <iowrapper.h>
#include <atom_model.h>

namespace em = emscripten;
using namespace Vipster;

static GLWrapper gl;

template<typename T>
em::class_<std::array<T, 3>> register_array(const char* name) {
    typedef std::array<T, 3> ArrType;
    return em::class_<std::array<T, 3>>(name)
        .template constructor<>()
        .function("size", &ArrType::size)
        .function("get", &em::internal::VectorAccess<ArrType>::get)
        .function("set", &em::internal::VectorAccess<ArrType>::set)
    ;
}

EMSCRIPTEN_BINDINGS(vipster) {
    em::enum_<IOFmt>("IOFmt")
            .value("XYZ", IOFmt::XYZ)
            .value("PWI", IOFmt::PWI)
            ;
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void emReadFile(char* fn, IOFmt fmt){
    auto d = readFile(fn, fmt);
    gl.molecules.push_back(d.mol);
    gl.curStep = &gl.molecules.back().getStep(0);
}

}

EM_BOOL mouse_event(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    enum class MouseMode { Camera, Select, Modify};
    enum class OpMode { None, Rotation, Translation };
    static OpMode currentOp = OpMode::None;
    static long localX, localY;
    switch (eventType) {
    case EMSCRIPTEN_EVENT_MOUSEDOWN:
        if(currentOp == OpMode::None){
            switch(mouseEvent->button){
            case 0:
                currentOp = OpMode::Rotation;
                break;
            case 1:
                currentOp = OpMode::Translation;
                break;
            case 2:
                gl.rMat = {{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
                gl.rMatChanged = true;
            default:
                break;
            }
        }
        break;
    case EMSCRIPTEN_EVENT_MOUSEUP:
        if(currentOp!=OpMode::None && !mouseEvent->buttons){
            currentOp = OpMode::None;
        }
        break;
    case EMSCRIPTEN_EVENT_MOUSEMOVE:
        switch(currentOp){
        case OpMode::Rotation:
            glMatRot(gl.rMat, mouseEvent->canvasX-localX, 0, 1, 0);
            glMatRot(gl.rMat, mouseEvent->canvasY-localY, 1, 0, 0);
            gl.rMatChanged = true;
            break;
        case OpMode::Translation:
            glMatTranslate(gl.vMat, (mouseEvent->canvasX-localX)/10.,
                           -(mouseEvent->canvasY-localY)/10.,0);
            gl.vMatChanged = true;
            break;
        default:
            break;
        }
        break;
    }
    localX = mouseEvent->canvasX;
    localY = mouseEvent->canvasY;
    return 1;
}

EM_BOOL wheel_event(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
{
    glMatScale(gl.vMat, wheelEvent->deltaY<0?1.1:0.9);
    gl.vMatChanged = true;
    return 1;
}

EM_BOOL key_event(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
    switch(keyEvent->keyCode){
        case 37:
            glMatRot(gl.rMat, -10, 0, 1, 0);
            gl.rMatChanged = true;
        break;
        case 39:
            glMatRot(gl.rMat, 10, 0, 1, 0);
            gl.rMatChanged = true;
        break;
        case 38:
            glMatRot(gl.rMat, -10, 1, 0, 0);
            gl.rMatChanged = true;
        break;
        case 40:
            glMatRot(gl.rMat, 10, 1, 0, 0);
            gl.rMatChanged = true;
        break;
    }
    return 1;
}

void one_iter(){
    int width, height, fullscreen;
    static Step* curStep{nullptr};
    if(curStep != gl.curStep){
        curStep = gl.curStep;
        gl.atom_buffer.clear();
        for(const Atom& at:curStep->getAtoms()){
            PseEntry &pse = (*curStep->pse)[at.name];
            gl.atom_buffer.push_back({{at.coord[0],at.coord[1],at.coord[2],pse.covr,
                                    pse.col[0],pse.col[1],pse.col[2],pse.col[3]}});
        }
        glBindBuffer(GL_ARRAY_BUFFER, gl.atom_vbo);
        glBufferData(GL_ARRAY_BUFFER, gl.atom_buffer.size()*8*sizeof(float),
                     (void*)gl.atom_buffer.data(), GL_STATIC_DRAW);
    }
    emscripten_get_canvas_size(&width, &height, &fullscreen);
    if( width != gl.width || height != gl.height){
        glViewport(0,0,width,height);
        gl.width = width;
        gl.height = height;
        float aspect = (float)width/height;
        gl.pMat = glMatOrtho(-10*aspect,10*aspect,-10,10,0,1000);
        gl.pMatChanged = true;
    }
    if(gl.rMatChanged){
        glUniformMatrix4fv(glGetUniformLocation(gl.atom_program, "rMatrix"),
                           1, true, gl.rMat.data());
        glUniformMatrix4fv(glGetUniformLocation(gl.atom_program, "vpMatrix"),
                           1, true, (gl.pMat*gl.vMat*gl.rMat).data());
        gl.pMatChanged = false;
        gl.vMatChanged = false;
        gl.rMatChanged = false;
    }else if(gl.pMatChanged || gl.vMatChanged){
        glUniformMatrix4fv(glGetUniformLocation(gl.atom_program, "vpMatrix"),
                           1, true, (gl.pMat*gl.vMat*gl.rMat).data());
        gl.pMatChanged = false;
        gl.vMatChanged = false;
    }
    glBindVertexArray(gl.atom_vao);
    glDrawArraysInstanced(GL_TRIANGLES,0,atom_model_npoly,gl.atom_buffer.size());
}

int main()
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.enableExtensionsByDefault = 1;
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context( 0, &attrs );
    if (!context)
    {
        printf("WebGL 2 is not supported!\n");
        return 0;
    }

    emscripten_webgl_make_context_current(context);
    glClearColor(1,1,1,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    GLWrapper gl{};

    gl.atom_program = loadShader("# version 300 es\nprecision highp float;\n",
                                 "/atom.vert",
                                 "/atom.frag");
    glUseProgram(gl.atom_program);

    initAtomVAO(gl);

    gl.vMat = glMatLookAt({{0,0,10}},{{0,0,0}},{{0,1,0}});
    gl.rMat = {{1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1}};
    Vec offset = {{0,0,0}};
    GLint atfacLoc = glGetUniformLocation(gl.atom_program, "atom_fac");
    glUniform1f(atfacLoc, (GLfloat)0.5);
    GLint offsetLoc = glGetUniformLocation(gl.atom_program, "offset");
    glUniform3fv(offsetLoc, 1, offset.data());
    gl.pMatChanged = true;
    gl.vMatChanged = true;
    gl.rMatChanged = true;

    gl.molecules.emplace_back("test");
    gl.curStep = &gl.molecules[0].getStep(0);
    gl.curStep->newAtom();
    gl.curStep->newAtom({"O",{{1,0,0}}});
    gl.curStep->newAtom({"F",{{0,1,0}}});


    emscripten_set_mousedown_callback("#canvas", nullptr, 1, mouse_event);
    emscripten_set_mouseup_callback(0, nullptr, 1, mouse_event);
    emscripten_set_mousemove_callback(0, nullptr, 1, mouse_event);
    emscripten_set_wheel_callback("#canvas", nullptr, 1, wheel_event);
    emscripten_set_keypress_callback(0, nullptr, 1, key_event);
    emscripten_set_main_loop(one_iter, 0, 1);
    return 1;
}
