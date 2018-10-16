#include "config.h"

using namespace Vipster;

IO::PWConfig::PWConfig(std::string name, PWConfig::AtomFmt atoms, CellFmt cell)
    : BaseConfig{name}, atoms{atoms}, cell{cell}
{}

std::unique_ptr<IO::BaseConfig> IO::PWConfig::copy()
{
    return std::make_unique<IO::PWConfig>(*this);
}

void Vipster::IO::to_json(nlohmann::json &j, const IO::PWConfig& c)
{
    j["atomfmt"] = c.atoms;
    j["cellfmt"] = c.cell;
}

void Vipster::IO::from_json(const nlohmann::json &j, IO::PWConfig& c)
{
    c.atoms = j.at("atomfmt");
    c.cell = j.at("cellfmt");
}

void IO::PWConfig::parseJson(const nlohmann::json::iterator& it)
{
    name = it.key();
    from_json(it.value(), *this);
}

nlohmann::json IO::PWConfig::toJson()
{
    nlohmann::json j;
    to_json(j, *this);
    return j;
}
