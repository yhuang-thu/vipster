#ifndef SELDATA_H
#define SELDATA_H

#include "guidata.h"
#include "molecule.h"

namespace Vipster {
namespace GUI {
    struct SelProp{ // 16 bytes
        Vec pos;    // 3*4 = 12 bytes
        float rad;  // 4 bytes
    };

    class SelData: public Data{
        // CPU-Data:
        std::vector<SelProp> sel_buffer{};
        std::array<float, 9>  cell_mat{};
        Step::selection* curSel{nullptr};
        // GPU-State/Data:
        GLuint vao{0}, vbo{0};
        // Shader:
        static struct{
            GLuint program;
            GLuint vertex, position, vert_scale;
            GLint offset, pos_scale, scale_fac, color;
            bool initialized{false};
        } shader;
    public:
        SelData(const GlobalData& glob, Step::selection* sel=nullptr);
        ~SelData() override;
        void drawMol(const Vec &off) override;
        void drawCell(const Vec &off, const PBCVec &mult) override;
        void updateGL() override;
        void initGL() override;
        void update(Step::selection* sel);
    };
}
}

#endif // SELDATA_H
