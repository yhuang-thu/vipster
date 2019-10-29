#ifndef IOPLUGIN_H
#define IOPLUGIN_H

#include "../molecule.h"
#include "parameters.h"
#include "configs.h"
#include "settings.h"
#include "data.h"

#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <iomanip>

namespace Vipster::IO{

struct Plugin{
    // TODO: remove Args?
    enum Args:uint8_t{None=0x0, Read=0x1, Write=0x2, Param=0x4, Config=0x8};
    std::string name;
    std::string extension;
    std::string command;
    uint8_t     arguments;
    IO::Data    (*parser)(const std::string& name, std::istream &file);
    bool        (*writer)(const Molecule& m, std::ostream &file,
                          const BaseParam *const p,
                          const BaseConfig *const c,
                          size_t idx) = nullptr;
    std::unique_ptr<BaseParam> (*makeParam)(const std::string& name) = nullptr;
    std::unique_ptr<BaseConfig> (*makeConfig)(const std::string& name) = nullptr;
};

class Error: public std::runtime_error
{
    public:
        Error(const std::string& reason, bool fatal=true)
            : std::runtime_error{reason}, fatal{fatal}
        {}
        bool fatal;
};

}

#endif // IOPLUGIN_H
