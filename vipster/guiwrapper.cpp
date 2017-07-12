#include "guiwrapper.h"
#include <fstream>
#include <iostream>
#include <atom_model.h>
#include <bond_model.h>

#ifdef __EMSCRIPTEN__
std::string readShader(std::string filePath)
{
    std::string content;
    std::ifstream fileStream{filePath};

    if(!fileStream) throw std::invalid_argument{"Shader not found: "+filePath};

    content.assign(std::istreambuf_iterator<char>{fileStream},
                   std::istreambuf_iterator<char>{});
    return content;
}
#else
#include <QFile>
std::string readShader(QString filePath)
{
    QFile f(filePath);
    f.open(QIODevice::ReadOnly);
    return f.readAll().toStdString();
}
#endif

void GuiWrapper::loadShader(GLuint &program, std::string header, std::string vertShaderStr, std::string fragShaderStr)
{
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLint gl_ok = GL_FALSE;

    vertShaderStr = header + vertShaderStr;
    const char *vertShaderSrc = vertShaderStr.c_str();
    glShaderSource(vertShader, 1, &vertShaderSrc, nullptr);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &gl_ok);
    if(!gl_ok){
        std::cout << "Vertex-Shader does not compile" << std::endl;
        GLint infoLen = 0;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<char> infoLog;
        infoLog.resize((infoLen > 1)?infoLen:1);
        glGetShaderInfoLog(vertShader, infoLen, nullptr, &infoLog[0]);
        std::cout << &infoLog[0] << std::endl;
        throw std::invalid_argument{"Vertex-Shader does not compile"};
    }

    fragShaderStr = header + fragShaderStr;
    const char *fragShaderSrc = fragShaderStr.c_str();
    glShaderSource(fragShader, 1, &fragShaderSrc, nullptr);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &gl_ok);
    if(!gl_ok){
        std::cout << "Shader does not compile" << std::endl;
        GLint infoLen = 0;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<char> infoLog(infoLen > 1?infoLen:1);
        glGetShaderInfoLog(fragShader, infoLen, nullptr, &infoLog[0]);
        std::cout << &infoLog[0] << std::endl;
        throw std::invalid_argument{"Shader does not compile"};
    }

    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &gl_ok);
    if(!gl_ok){
        std::cout << "Program does not link" << std::endl;
        GLint infoLen = 0;
        std::vector<char> infoLog(infoLen > 1?infoLen:1);
        glGetProgramInfoLog(program, infoLen, nullptr, &infoLog[0]);
        std::cout << &infoLog[0] << std::endl;
        throw std::invalid_argument{"Program does not link"};
    }

    glDetachShader(program, vertShader);
    glDeleteShader(vertShader);
    glDetachShader(program, fragShader);
    glDeleteShader(fragShader);
}

void guiMatScale(guiMat &m, float f)
{
    for(int i=0;i<4;i++){
        m[i*4+0]*=f;
        m[i*4+1]*=f;
        m[i*4+2]*=f;
    }
}

void guiMatTranslate(guiMat &m, float x, float y, float z)
{
    //assuming 0 0 0 1 in last row of m
    m[3]+=x;
    m[7]+=y;
    m[11]+=z;
}

void guiMatRot(guiMat &m, float a, float x, float y, float z)
{
    if(a==0){
        return;
    }
    float tmp = a * M_PI / 180.;
    float s = std::sin(tmp);
    float c = std::cos(tmp);
    if(x==0){
        if(y==0){
            if(z!=0){
                // z-axis
                if(z<0) s = -s;
                m[0] = c * (tmp = m[0]) - s * m[4];
                m[4] = s * tmp + c * m[4];
                m[1] = c * (tmp = m[1]) - s * m[5];
                m[5] = s * tmp + c * m[5];
                m[2] = c * (tmp = m[2]) - s * m[6];
                m[6] = s * tmp + c * m[6];
                m[3] = c * (tmp = m[3]) - s * m[7];
                m[7] = s * tmp + c * m[7];
            }
        }else if(z==0){
            // y-axis
            if(y<0) s = -s;
            m[0] = c * (tmp = m[0]) + s * m[8];
            m[8] = -s * tmp + c * m[8];
            m[1] = c * (tmp = m[1]) + s * m[9];
            m[9] = -s * tmp + c * m[9];
            m[2] = c * (tmp = m[2]) + s * m[10];
            m[10] = -s * tmp + c * m[10];
            m[3] = c * (tmp = m[3]) + s * m[11];
            m[11] = -s * tmp + c * m[11];
        }
    }else if(y==0 && z==0){
        // x-axis
        if(x<0) s = -s;
        m[4] = c * (tmp = m[4]) - s * m[8];
        m[8] = s * tmp + c * m[8];
        m[5] = c * (tmp = m[5]) - s * m[9];
        m[9] = s * tmp + c * m[9];
        m[6] = c * (tmp = m[6]) - s * m[10];
        m[10] = s * tmp + c * m[10];
        m[7] = c * (tmp = m[7]) - s * m[11];
        m[11] = s * tmp + c * m[11];
    }else{
        // general rotation
        Vec axis{{x,y,z}};
        axis /= Vec_length(axis);
        Vec axismc = axis*(1-c);
        guiMat rotate{{c+axismc[0]*axis[0], axismc[1]*axis[0]-s*axis[2], axismc[2]*axis[0]+s*axis[1], 0,
                      axismc[0]*axis[1]+s*axis[2], c+axismc[1]*axis[1], axismc[2]*axis[1]-s*axis[0], 0,
                      axismc[0]*axis[2]-s*axis[1], axismc[1]*axis[2]+s*axis[0], c+axismc[2]*axis[2], 0,
                      0,0,0,1}};
        m = rotate * m;
    }
}

guiMat guiMatMkOrtho(float l, float r, float b, float t, float n, float f)
{
    return guiMat{{2/(r-l), 0, 0, (r+l)/(l-r),
                   0, 2/(t-b), 0, (t+b)/(b-t),
                   0, 0, (2/(n-f)), ((f+n)/(n-f)),
                   0, 0, 0, 1}};
}

guiMat guiMatMkLookAt(Vec eye, Vec target, Vec up)
{
    Vec dir = target - eye;
    dir /= Vec_length(dir);
    Vec r = Vec_cross(dir, up);
    r /= Vec_length(r);
    Vec u = Vec_cross(r, dir);
    return guiMat{{r[0], r[1], r[2], -Vec_dot(r, eye),
                  u[0], u[1], u[2], -Vec_dot(u, eye),
                  -dir[0], -dir[1], -dir[2], Vec_dot(dir, eye),
                  0, 0, 0, 1}};
}

guiMat operator *=(guiMat &a, const guiMat &b)
{
    a = guiMat{{a[0]*b[0]+a[1]*b[4]+a[2]*b[ 8]+a[3]*b[12],
               a[0]*b[1]+a[1]*b[5]+a[2]*b[ 9]+a[3]*b[13],
               a[0]*b[2]+a[1]*b[6]+a[2]*b[10]+a[3]*b[14],
               a[0]*b[3]+a[1]*b[7]+a[2]*b[11]+a[3]*b[15],
               a[4]*b[0]+a[5]*b[4]+a[6]*b[ 8]+a[7]*b[12],
               a[4]*b[1]+a[5]*b[5]+a[6]*b[ 9]+a[7]*b[13],
               a[4]*b[2]+a[5]*b[6]+a[6]*b[10]+a[7]*b[14],
               a[4]*b[3]+a[5]*b[7]+a[6]*b[11]+a[7]*b[15],
               a[8]*b[0]+a[9]*b[4]+a[10]*b[ 8]+a[11]*b[12],
               a[8]*b[1]+a[9]*b[5]+a[10]*b[ 9]+a[11]*b[13],
               a[8]*b[2]+a[9]*b[6]+a[10]*b[10]+a[11]*b[14],
               a[8]*b[3]+a[9]*b[7]+a[10]*b[11]+a[11]*b[15],
               a[12]*b[0]+a[13]*b[4]+a[14]*b[ 8]+a[15]*b[12],
               a[12]*b[1]+a[13]*b[5]+a[14]*b[ 9]+a[15]*b[13],
               a[12]*b[2]+a[13]*b[6]+a[14]*b[10]+a[15]*b[14],
               a[12]*b[3]+a[13]*b[7]+a[14]*b[11]+a[15]*b[15]}};
    return a;
}

guiMat operator *(guiMat a, const guiMat &b)
{
    return a*=b;
}

void GuiWrapper::initUBO(void)
{
    GLuint atomLoc = glGetUniformBlockIndex(atom_program, "viewMat");
    glUniformBlockBinding(atom_program, atomLoc, 0);
//    GLuint bondLoc = glGetUniformBlockIndex(bond_program, "viewMat");
//    glUniformBlockBinding(bond_program, bondLoc, 0);
    GLuint cellLoc = glGetUniformBlockIndex(cell_program, "viewMat");
    glUniformBlockBinding(cell_program, cellLoc, 0);

    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2*16*sizeof(float), NULL, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, atomLoc, ubo, 0, 2*16*sizeof(float));
}

void GuiWrapper::initAtomVAO(void)
{
    glGenVertexArrays(1, &atom_vao);
    glBindVertexArray(atom_vao);
    glGenBuffers(1, &sphere_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo);
    glBufferData(GL_ARRAY_BUFFER, atom_model_npoly*3*sizeof(float), (void*)&atom_model, GL_STATIC_DRAW);
    GLuint vertexLoc = glGetAttribLocation(atom_program, "vertex_modelspace");
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(vertexLoc);
    glGenBuffers(1, &atom_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, atom_vbo);
    GLuint positionLoc = glGetAttribLocation(atom_program, "position_modelspace");
    glVertexAttribPointer(positionLoc,3,GL_FLOAT,GL_FALSE,8*sizeof(float),0);
    glVertexAttribDivisor(positionLoc,1);
    glEnableVertexAttribArray(positionLoc);
    GLuint scaleLoc = glGetAttribLocation(atom_program, "scale_modelspace");
    glVertexAttribPointer(scaleLoc,1,GL_FLOAT,GL_FALSE,8*sizeof(float),(const GLvoid*)(3*sizeof(float)));
    glVertexAttribDivisor(scaleLoc,1);
    glEnableVertexAttribArray(scaleLoc);
    GLuint colorLoc = glGetAttribLocation(atom_program, "color_input");
    glVertexAttribPointer(colorLoc,4,GL_FLOAT,GL_FALSE,8*sizeof(float),(const GLvoid*)(4*sizeof(float)));
    glVertexAttribDivisor(colorLoc,1);
    glEnableVertexAttribArray(colorLoc);
}

void GuiWrapper::initBondVAO(void)
{
    glGenVertexArrays(1, &bond_vao);
    glBindVertexArray(bond_vao);
    glGenBuffers(1, &torus_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, torus_vbo);
    glBufferData(GL_ARRAY_BUFFER, bond_model_npoly*3*sizeof(float), (void*)&bond_model, GL_STATIC_DRAW);
    GLuint vertexLoc = glGetAttribLocation(bond_program, "vertex_modelspace");
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(vertexLoc);
    glGenBuffers(1, &bond_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, bond_vbo);
    GLuint mMatLoc = glGetAttribLocation(bond_program, "mMatrix");
    glVertexAttribPointer(mMatLoc  ,3,GL_FLOAT,GL_FALSE,24*sizeof(float),0);
    glVertexAttribPointer(mMatLoc+1,3,GL_FLOAT,GL_FALSE,24*sizeof(float),(const GLvoid*)(3*sizeof(float)));
    glVertexAttribPointer(mMatLoc+2,3,GL_FLOAT,GL_FALSE,24*sizeof(float),(const GLvoid*)(6*sizeof(float)));
    glVertexAttribDivisor(mMatLoc,1);
    glVertexAttribDivisor(mMatLoc+1,1);
    glVertexAttribDivisor(mMatLoc+2,1);
    glEnableVertexAttribArray(mMatLoc);
    glEnableVertexAttribArray(mMatLoc+1);
    glEnableVertexAttribArray(mMatLoc+2);
    GLuint positionLoc = glGetAttribLocation(bond_program, "position_modelspace");
    glVertexAttribPointer(positionLoc,3,GL_FLOAT,GL_FALSE,24*sizeof(float),(const GLvoid*)(9*sizeof(float)));
    glVertexAttribDivisor(positionLoc,1);
    glEnableVertexAttribArray(positionLoc);
    GLuint critLoc = glGetAttribLocation(bond_program, "pbc_crit");
    glVertexAttribPointer(critLoc,1,GL_FLOAT,GL_FALSE,24*sizeof(float),(const GLvoid*)(12*sizeof(float)));
    glVertexAttribDivisor(critLoc,1);
    glEnableVertexAttribArray(critLoc);
    GLuint color1Loc = glGetAttribLocation(bond_program, "s1Color");
    glVertexAttribPointer(color1Loc,4,GL_FLOAT,GL_FALSE,24*sizeof(float),(const GLvoid*)(16*sizeof(float)));
    glVertexAttribDivisor(color1Loc,1);
    glEnableVertexAttribArray(color1Loc);
    GLuint color2Loc = glGetAttribLocation(bond_program, "s2Color");
    glVertexAttribPointer(color2Loc,4,GL_FLOAT,GL_FALSE,24*sizeof(float),(const GLvoid*)(20*sizeof(float)));
    glVertexAttribDivisor(color2Loc,1);
    glEnableVertexAttribArray(color2Loc);
}

void GuiWrapper::initCellVAO(void)
{
    glGenVertexArrays(1, &cell_vao);
    glBindVertexArray(cell_vao);
    glGenBuffers(1, &cell_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cell_ibo);
    GLushort indices[24] = {0,1,0,2,0,3,1,4,1,5,2,4,2,6,3,5,3,6,4,7,5,7,6,7};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), (void*)indices, GL_STATIC_DRAW);
    glGenBuffers(1, &cell_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cell_vbo);
    GLuint vertexLoc = glGetAttribLocation(cell_program, "vertex_modelspace");
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);
}

void GuiWrapper::deleteGLObjects(void)
{
    glDeleteBuffers(1, &ubo);
    glDeleteProgram(atom_program);
    glDeleteBuffers(1, &atom_vbo);
    glDeleteBuffers(1, &sphere_vbo);
    glDeleteVertexArrays(1, &atom_vao);
    glDeleteProgram(bond_program);
    glDeleteBuffers(1, &bond_vbo);
    glDeleteBuffers(1, &torus_vbo);
    glDeleteVertexArrays(1, &bond_vao);
    glDeleteProgram(cell_program);
    glDeleteBuffers(1, &cell_vbo);
    glDeleteBuffers(1, &cell_ibo);
    glDeleteVertexArrays(1, &cell_vao);
}
