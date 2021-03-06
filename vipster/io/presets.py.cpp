#include "vipster/pyvipster.h"
#include "vipster/io/plugin.h"
#include "vipster/io/presets.h"

namespace Vipster::IO{
std::ostream& operator<<(std::ostream& os, const Preset &p){
    nlohmann::json j;
    to_json(j, p);
    return os << j;
}
}

PYBIND11_MAKE_OPAQUE(Vipster::IO::Presets);
PYBIND11_MAKE_OPAQUE(Vipster::IO::Presets::mapped_type);

namespace Vipster::Py{

void Presets(py::module &io){
    py::class_<IO::Preset>(io, "Preset")
        .def("__repr__", [](const IO::Preset &p){
            std::ostringstream s;
            nlohmann::json j;
            to_json(j, p);
            s << p.getFmt()->command << "-Preset" << j;
            return s.str();
        });

    py::bind_map<IO::Presets::mapped_type>(io, "__StrPresMap__");
    py::bind_map<IO::Presets>(io, "IOPresets")
        .def("__repr__", [](IO::Presets& p){
            std::ostringstream s;
            s << "IOPresets{";
            bool f = false;
            for (auto const &kv:p){
                if (f)
                    s << ", ";
                s << kv.first << ": __StrPresMap__";
                f = true;
            }
            s << '}';
            return s.str();
        })
    ;
}

}
