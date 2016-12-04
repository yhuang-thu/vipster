#ifndef IOWRAPPER
#define IOWRAPPER

#include <molecule.h>
#include <ioplugin.h>
#include <map>
#include <ioplugins/xyz.h>
#include <ioplugins/pwinput.h>

namespace Vipster{
    enum class  IOFmt{XYZ,PWI};
    const       std::map<IOFmt, IOPlugin const *const> IOPlugins{
                    {IOFmt::XYZ, &IO::XYZ},
                    {IOFmt::PWI, &IO::PWInput}
                };
    IOData      readFile(std::string fn, IOFmt fmt);
//    bool        writeFile(const IOData &d, std::string fn, IOFmt fmt);
}

#endif // IOWRAPPER

