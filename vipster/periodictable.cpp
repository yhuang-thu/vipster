#include "periodictable.h"
#include <cctype>
#include <optional>

using namespace Vipster;

bool Vipster::operator==(const Element &lhs, const Element &rhs)
{
    return std::tie(lhs.PWPP, lhs.CPPP, lhs.CPNL, lhs.Z, lhs.m,
                    lhs.bondcut, lhs.covr, lhs.vdwr, lhs.col)
            ==
           std::tie(rhs.PWPP, rhs.CPPP, rhs.CPNL, rhs.Z, rhs.m,
                    rhs.bondcut, rhs.covr, rhs.vdwr, rhs.col);
}

const Vipster::PeriodicTable Vipster::pte = {
    {{"",      { "", "", "", 0,   0.0,       -1,   1.46, 3.21, {   0,   0,   0, 255 } }},
     {"H",     { "", "", "", 1,   1.0079,    0.72, 0.72, 2.27, { 190, 190, 190, 255 } }},
     {"He",    { "", "", "", 2,   4.0026,    0.6,  0.6,  2.65, { 215, 255, 255, 255 } }},
     {"Li",    { "", "", "", 3,   6.941,     2.53, 2.53, 3.44, { 204, 126, 255, 255 } }},
     {"Be",    { "", "", "", 4,   9.0122,    1.7,  1.7,  2.89, { 192, 255,   0, 255 } }},
     {"B",     { "", "", "", 5,   10.811,    1.55, 1.55, 3.63, { 255, 181, 181, 255 } }},
     {"C",     { "", "", "", 6,   12.0107,   1.46, 1.46, 3.21, { 102, 102, 102, 255 } }},
     {"N",     { "", "", "", 7,   14.007,    1.34, 1.34, 2.93, {  11,  11, 255, 255 } }},
     {"O",     { "", "", "", 8,   15.999,    1.38, 1.38, 2.87, { 255,  11,  11, 255 } }},
     {"F",     { "", "", "", 9,   18.998,    1.34, 1.34, 2.78, { 126, 177, 255, 255 } }},
     {"Ne",    { "", "", "", 10,  20.18,     1.3,  1.3,  2.91, { 177, 226, 243, 255 } }},
     {"Na",    { "", "", "", 11,  22.99,     2.91, 2.91, 4.29, { 170,  90, 241, 255 } }},
     {"Mg",    { "", "", "", 12,  24.305,    2.46, 2.46, 3.27, { 137, 255,   0, 255 } }},
     {"Al",    { "", "", "", 13,  26.982,    2.23, 2.23, 3.48, { 190, 164, 164, 255 } }},
     {"Si",    { "", "", "", 14,  28.086,    2.1,  2.1,  3.97, { 126, 153, 153, 255 } }},
     {"P",     { "", "", "", 15,  30.974,    2.0,  2.0,  3.4,  { 255, 126,   0, 255 } }},
     {"S",     { "", "", "", 16,  32.065,    1.93, 1.93, 3.4,  { 177, 177,   0, 255 } }},
     {"Cl",    { "", "", "", 17,  35.453,    1.87, 1.87, 3.31, {  30, 239,  30, 255 } }},
     {"Ar",    { "", "", "", 18,  39.948,    1.83, 1.83, 3.55, { 126, 208, 226, 255 } }},
     {"K",     { "", "", "", 19,  39.098,    3.7,  3.7,  5.2,  { 141,  62, 210, 255 } }},
     {"Ca",    { "", "", "", 20,  40.078,    3.29, 3.29, 4.37, {  61, 255,   0, 255 } }},
     {"Sc",    { "", "", "", 21,  44.9559,   2.72, 2.72, 3.99, { 228, 228, 228, 255 } }},
     {"Ti",    { "", "", "", 22,  47.867,    2.57, 2.57, 3.21, { 190, 192, 197, 255 } }},
     {"V",     { "", "", "", 23,  50.9415,   2.36, 2.36, 3.21, { 164, 164, 170, 255 } }},
     {"Cr",    { "", "", "", 24,  51.9961,   2.4,  2.4,  3.21, { 137, 153, 197, 255 } }},
     {"Mn",    { "", "", "", 25,  54.938,    2.63, 2.63, 3.21, { 155, 121, 197, 255 } }},
     {"Fe",    { "", "", "", 26,  55.845,    2.36, 2.36, 3.21, { 223, 102,  51, 255 } }},
     {"Co",    { "", "", "", 27,  58.9332,   2.38, 2.38, 3.21, { 239, 141, 159, 255 } }},
     {"Ni",    { "", "", "", 28,  58.6934,   2.29, 2.29, 3.08, {  79, 208,  79, 255 } }},
     {"Cu",    { "", "", "", 29,  63.546,    2.61, 2.61, 2.65, { 197, 126,  51, 255 } }},
     {"Zn",    { "", "", "", 30,  65.39,     2.48, 2.48, 2.63, { 124, 126, 175, 255 } }},
     {"Ga",    { "", "", "", 31,  69.723,    2.38, 2.38, 3.53, { 192, 141, 141, 255 } }},
     {"Ge",    { "", "", "", 32,  72.64,     2.31, 2.31, 3.99, { 102, 141, 141, 255 } }},
     {"As",    { "", "", "", 33,  74.922,    2.25, 2.25, 3.5,  { 188, 126, 226, 255 } }},
     {"Se",    { "", "", "", 34,  78.96,     2.19, 2.19, 3.59, { 255, 159,   0, 255 } }},
     {"Br",    { "", "", "", 35,  79.904,    2.15, 2.15, 3.5,  { 164,  39,  39, 255 } }},
     {"Kr",    { "", "", "", 36,  83.798,    2.08, 2.08, 3.82, {  90, 183, 208, 255 } }},
     {"Rb",    { "", "", "", 37,  85.4678,   3.99, 3.99, 5.73, { 112,  44, 175, 255 } }},
     {"Sr",    { "", "", "", 38,  87.62,     3.63, 3.63, 4.71, {   0, 255,   0, 255 } }},
     {"Y",     { "", "", "", 39,  88.9059,   3.06, 3.06, 3.21, { 146, 255, 255, 255 } }},
     {"Zr",    { "", "", "", 40,  91.224,    2.8,  2.8,  3.21, { 146, 223, 223, 255 } }},
     {"Nb",    { "", "", "", 41,  92.9064,   2.59, 2.59, 3.21, { 113, 192, 201, 255 } }},
     {"Mo",    { "", "", "", 42,  95.94,     2.74, 2.74, 3.21, {  84, 181, 181, 255 } }},
     {"Tc",    { "", "", "", 43,  98.0,      2.95, 2.95, 3.21, {  57, 157, 157, 255 } }},
     {"Ru",    { "", "", "", 44,  101.07,    2.38, 2.38, 3.21, {  35, 141, 141, 255 } }},
     {"Rh",    { "", "", "", 45,  102.9055,  2.55, 2.55, 3.21, {   9, 124, 139, 255 } }},
     {"Pd",    { "", "", "", 46,  106.42,    2.48, 2.48, 3.08, {   0, 104, 132, 255 } }},
     {"Ag",    { "", "", "", 47,  107.8682,  2.89, 2.89, 3.25, { 223, 223, 255, 255 } }},
     {"Cd",    { "", "", "", 48,  112.411,   2.8,  2.8,  2.99, { 255, 215, 141, 255 } }},
     {"In",    { "", "", "", 49,  114.818,   2.72, 2.72, 3.65, { 164, 117, 113, 255 } }},
     {"Sn",    { "", "", "", 50,  118.71,    2.66, 2.66, 4.1,  { 102, 126, 126, 255 } }},
     {"Sb",    { "", "", "", 51,  121.76,    2.61, 2.61, 3.89, { 157,  99, 181, 255 } }},
     {"Te",    { "", "", "", 52,  127.6,     2.55, 2.55, 3.89, { 210, 121,   0, 255 } }},
     {"I",     { "", "", "", 53,  126.9045,  2.51, 2.51, 3.74, { 146,   0, 146, 255 } }},
     {"Xe",    { "", "", "", 54,  131.293,   2.46, 2.46, 4.08, {  66, 157, 175, 255 } }},
     {"Cs",    { "", "", "", 55,  132.9055,  4.25, 4.25, 6.48, {  86,  21, 141, 255 } }},
     {"Ba",    { "", "", "", 56,  137.327,   3.74, 3.74, 5.06, {   0, 201,   0, 255 } }},
     {"La",    { "", "", "", 57,  138.9055,  3.19, 3.19, 3.21, { 112, 210, 255, 255 } }},
     {"Ce",    { "", "", "", 58,  140.116,   1.46, 1.46, 3.21, { 255, 255, 197, 255 } }},
     {"Pr",    { "", "", "", 59,  140.9077,  1.46, 1.46, 3.21, { 215, 255, 197, 255 } }},
     {"Nd",    { "", "", "", 60,  144.24,    1.46, 1.46, 3.21, { 197, 255, 197, 255 } }},
     {"Pm",    { "", "", "", 61,  145.0,     1.46, 1.46, 3.21, { 163, 255, 197, 255 } }},
     {"Sm",    { "", "", "", 62,  150.36,    1.46, 1.46, 3.21, { 141, 255, 197, 255 } }},
     {"Eu",    { "", "", "", 63,  151.964,   1.46, 1.46, 3.21, {  95, 255, 197, 255 } }},
     {"Gd",    { "", "", "", 64,  157.25,    1.46, 1.46, 3.21, {  68, 255, 197, 255 } }},
     {"Tb",    { "", "", "", 65,  158.9253,  1.46, 1.46, 3.21, {  48, 255, 197, 255 } }},
     {"Dy",    { "", "", "", 66,  162.5,     1.46, 1.46, 3.21, {  30, 255, 197, 255 } }},
     {"Ho",    { "", "", "", 67,  164.9303,  1.46, 1.46, 3.21, {   0, 255, 155, 255 } }},
     {"Er",    { "", "", "", 68,  167.259,   1.46, 1.46, 3.21, {   0, 228, 117, 255 } }},
     {"Tm",    { "", "", "", 69,  168.9342,  1.46, 1.46, 3.21, {   0, 210,  81, 255 } }},
     {"Yb",    { "", "", "", 70,  173.04,    1.46, 1.46, 3.21, {   0, 190,  55, 255 } }},
     {"Lu",    { "", "", "", 71,  174.967,   3.02, 3.02, 3.21, {   0, 170,  35, 255 } }},
     {"Hf",    { "", "", "", 72,  178.49,    2.83, 2.83, 3.21, {  75, 192, 255, 255 } }},
     {"Ta",    { "", "", "", 73,  180.9479,  2.61, 2.61, 3.21, {  75, 164, 255, 255 } }},
     {"W",     { "", "", "", 74,  183.84,    2.76, 2.76, 3.21, {  33, 146, 214, 255 } }},
     {"Re",    { "", "", "", 75,  186.207,   3.0,  3.0,  3.21, {  37, 124, 170, 255 } }},
     {"Os",    { "", "", "", 76,  190.23,    2.42, 2.42, 3.21, {  37, 102, 150, 255 } }},
     {"Ir",    { "", "", "", 77,  192.217,   2.59, 2.59, 3.21, {  21,  84, 135, 255 } }},
     {"Pt",    { "", "", "", 78,  195.078,   2.42, 2.42, 3.31, { 243, 237, 208, 255 } }},
     {"Au",    { "", "", "", 79,  196.9665,  2.72, 2.72, 3.14, { 204, 208,  30, 255 } }},
     {"Hg",    { "", "", "", 80,  200.592,   2.82, 2.82, 2.93, { 181, 181, 192, 255 } }},
     {"Tl",    { "", "", "", 81,  204.3833,  2.8,  2.8,  3.7,  { 164,  84,  75, 255 } }},
     {"Pb",    { "", "", "", 82,  207.2,     2.78, 2.78, 3.82, {  86,  88,  95, 255 } }},
     {"Bi",    { "", "", "", 83,  208.9804,  2.76, 2.76, 3.91, { 157,  79, 181, 255 } }},
     {"Po",    { "", "", "", 84,  209.,      1.46, 1.46, 3.72, { 170,  90,   0, 255 } }},
     {"At",    { "", "", "", 85,  210.,      1.46, 1.46, 3.82, { 117,  79,  68, 255 } }},
     {"Rn",    { "", "", "", 86,  222.,      2.74, 2.74, 4.16, {  66, 130, 150, 255 } }},
     {"Fr",    { "", "", "", 87,  223.,      1.46, 1.46, 6.58, {  66,   0, 102, 255 } }},
     {"Ra",    { "", "", "", 88,  226.,      1.46, 1.46, 5.35, {   0, 124,   0, 255 } }},
     {"Ac",    { "", "", "", 89,  227.,      1.46, 1.46, 3.21, { 112, 170, 248, 255 } }},
     {"Th",    { "", "", "", 90,  232.03806, 1.46, 1.46, 3.21, {   0, 186, 255, 255 } }},
     {"Pa",    { "", "", "", 91,  231.03588, 1.46, 1.46, 3.21, {   0, 159, 255, 255 } }},
     {"U",     { "", "", "", 92,  238.02891, 1.46, 1.46, 3.51, {   0, 141, 255, 255 } }},
     {"Np",    { "", "", "", 93,  237.,      1.46, 1.46, 3.21, {   0, 126, 255, 255 } }},
     {"Pu",    { "", "", "", 94,  244.,      1.46, 1.46, 3.21, {   0, 106, 255, 255 } }},
     {"Am",    { "", "", "", 95,  243.,      1.46, 1.46, 3.21, {  84,  90, 241, 255 } }},
     {"Cm",    { "", "", "", 96,  247.,      1.46, 1.46, 3.21, { 119,  90, 226, 255 } }},
     {"Bk",    { "", "", "", 97,  247.,      1.46, 1.46, 3.21, { 137,  79, 226, 255 } }},
     {"Cf",    { "", "", "", 98,  251.,      1.46, 1.46, 3.21, { 159,  53, 210, 255 } }},
     {"Es",    { "", "", "", 99,  252.,      1.46, 1.46, 3.21, { 177,  30, 210, 255 } }},
     {"Fm",    { "", "", "", 100, 257.,      1.46, 1.46, 3.21, { 177,  30, 186, 255 } }},
     {"Md",    { "", "", "", 101, 258.,      1.46, 1.46, 3.21, { 177,  11, 164, 255 } }},
     {"No",    { "", "", "", 102, 259.,      1.46, 1.46, 3.21, { 188,  11, 135, 255 } }},
     {"Lr",    { "", "", "", 103, 266.,      1.46, 1.46, 3.21, { 197,   0, 102, 255 } }},
     {"Rf",    { "", "", "", 104, 267.,      1.46, 1.46, 3.21, { 204,   0,  88, 255 } }},
     {"Db",    { "", "", "", 105, 268.,      1.46, 1.46, 3.21, { 208,   0,  79, 255 } }},
     {"Sg",    { "", "", "", 106, 269.,      1.46, 1.46, 3.21, { 215,   0,  68, 255 } }},
     {"Bh",    { "", "", "", 107, 270.,      1.46, 1.46, 3.21, { 223,   0,  55, 255 } }},
     {"Hs",    { "", "", "", 108, 269.,      1.46, 1.46, 3.21, { 228,   0,  44, 255 } }},
     {"Mt",    { "", "", "", 109, 278.,      1.46, 1.46, 3.21, { 234,   0,  37, 255 } }},
     {"Ds",    { "", "", "", 110, 281.,      1.46, 1.46, 3.21, { 237,   0,  35, 255 } }},
     {"Rg",    { "", "", "", 111, 281.,      1.46, 1.46, 3.21, { 239,   0,  33, 255 } }},
     {"Cn",    { "", "", "", 112, 285.,      1.46, 1.46, 3.21, { 241,   0,  30, 255 } }},
     {"Nh",    { "", "", "", 113, 286.,      1.46, 1.46, 3.21, { 243,   0,  28, 255 } }},
     {"Fl",    { "", "", "", 114, 289.,      1.46, 1.46, 3.21, { 246,   0,  25, 255 } }},
     {"Mc",    { "", "", "", 115, 289.,      1.46, 1.46, 3.21, { 248,   0,  21, 255 } }},
     {"Lv",    { "", "", "", 116, 293.,      1.46, 1.46, 3.21, { 252,   0,  19, 255 } }},
     {"Ts",    { "", "", "", 117, 294.,      1.46, 1.46, 3.21, { 252,   0,  17, 255 } }},
     {"Og",    { "", "", "", 118, 294.,      1.46, 1.46, 3.21, { 252,   0,  15, 255 } }},
     {"BCP",   { "", "", "", 0,   0.0,       -1.,  0.72, 2.27, {   0, 255,   0, 255 } }},
     {"RCP",   { "", "", "", 0,   0.0,       -1.,  0.72, 2.27, { 255,   0,   0, 255 } }},
     {"CCP",   { "", "", "", 0,   0.0,       -1.,  0.72, 2.27, { 255, 255,   0, 255 } }},
     {"NNACP", { "", "", "", 0,   0.0,       -1.,  0.72, 2.27, {   0,   0, 255, 255 } }}}
};

PeriodicTable::PeriodicTable(std::initializer_list<PeriodicTable::value_type> il,
                             const PeriodicTable *r)
    : StaticMap{il}, root{r}
{}

PeriodicTable::iterator PeriodicTable::find_or_fallback(const std::string &k)
{
    // send lookup either to specific root or hard-coded fallback-table
    const PeriodicTable& root = this->root? *this->root : Vipster::pte;
    auto entry = find(k);
    if(entry != end()){
        return entry;
    }else{
        // if key is ONLY a number, interpret as atomic number
        char *p;
        std::size_t Z = std::strtoul(k.c_str(), &p, 10);
        if(*p){
            // found a derived/custom name, try to guess base-name
            bool islower = std::islower(k[0]);
            // gradually ignore appended letters until we reach a matching atom type
            for(size_t i=k.length(); i>0; --i)
            {
                auto search = [&](const std::string& key)->std::optional<const Element>{
                    const_iterator tmp = find(key);
                    // local lookup
                    if(tmp != end()){
                        return {tmp->second};
                    }
                    // lookup in fallback table
                    tmp = root.find(key);
                    if(tmp != root.end()){
                        return {tmp->second};
                    }
                    return {};
                };
                if(islower){
                    // first check values starting with a uppercase letter to match defaults
                    std::string tmp = k.substr(0,i);
                    tmp[0] = std::toupper(k[0]);
                    auto test = search(tmp);
                    if(test){
                        return emplace(k, test.value()).first;
                    }
                }
                std::string tmp = k.substr(0,i);
                auto test = search(tmp);
                if(test){
                    return emplace(k, test.value()).first;
                }
            }
        }else{
            // interpret atomic number
            for(const auto& pair: root){
                if(pair.second.Z == Z){
                    return emplace(k, pair.second).first;
                }
            }
        }
    }
    return emplace(k, root.at("")).first;
}

Element& PeriodicTable::operator [](const std::string& k)
{
    return find_or_fallback(k)->second;
}

void Vipster::to_json(nlohmann::json& j, const Element& p)
{
    j["PWPP"] = p.PWPP;
    j["CPPP"] = p.CPPP;
    j["CPNL"] = p.CPNL;
    j["Z"] = p.Z;
    j["m"] = p.m;
    j["bondcut"] = p.bondcut;
    j["covr"] = p.covr;
    j["vdwr"] = p.vdwr;
    j["col"] = p.col;
}

void Vipster::from_json(const nlohmann::json& j, Element& p)
{
    p.PWPP = j.value("PWPP", "");
    p.CPPP = j.value("CPPP", "");
    p.CPNL = j.value("CPNL", "");
    p.Z = j.value("Z", 0u);
    p.m = j.value("m", 0.0);
    p.bondcut = j.value("bondcut", -1.);
    p.covr = j.value("covr", 1.46);
    p.vdwr = j.value("vdwr", 3.21);
    p.col = j.value("col", ColVec{0,0,0,255});
}

void Vipster::to_json(nlohmann::json& j, const PeriodicTable& m)
{
    for(const auto& e: m){
        j[e.first] = e.second;
    }
}

void Vipster::from_json(const nlohmann::json& j, PeriodicTable& m)
{
    for(auto it=j.begin(); it!=j.end(); ++it){
        m.insert_or_assign(it.key(), it.value());
    }
}
