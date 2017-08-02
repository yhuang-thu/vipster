#ifndef STEP_H
#define STEP_H

#include <config.h>
#include <vec.h>
#include <atom.h>
#include <bond.h>
#include <set>
#include <memory>

namespace Vipster{
class Molecule;
class Step
{
public:
    Step();
    Step(const std::shared_ptr<PseMap> &pse);
    friend Molecule;
    friend std::ostream& operator<< (std::ostream& s, const Step& st);
    void    newAtom(const Atom &at);
    void    newAtom(Atom&& at={"C",{{0,0,0}},0,{{false,false,false}},false});
    void    newAtom(Atom at, AtomFmt fmt);
    void    newAtoms(const std::vector<Atom> &v);
    void    delAtom(size_t idx);
    void    setAtom(size_t idx,const Atom& at);
    void    setAtom(size_t idx,Atom&& at);
    void    setAtom(size_t idx,Atom at,AtomFmt fmt);
    const Atom& getAtom(size_t idx) const;
    Atom    getAtom(size_t idx, AtomFmt fmt) const;
    const std::vector<Atom>& getAtoms() const;
    std::vector<Atom> getAtoms(AtomFmt fmt) const;
    size_t  getNat(void) const noexcept;
    AtomFmt getFmt() const noexcept;
    void    setFmt(AtomFmt fmt, bool scale=false);
    void    setCellDim(float cdm, bool scale=false);
    void    setCellDim(float cdm, bool scale, AtomFmt fmt);
    float   getCellDim() const noexcept;
    float   getCellDim(AtomFmt fmt) const noexcept;
    void    setCellVec(const Mat &vec, bool scale=false);
    const Mat& getCellVec(void) const noexcept;
    Vec   getCenter(bool com=false) const noexcept;
    std::set<std::string> getTypes(void)const noexcept;
    size_t  getNtyp(void) const noexcept;
    void  setComment(const std::string &s);
    const std::string& getComment(void) const noexcept;
    const std::vector<Bond>& getBonds() const;
    const std::vector<Bond>& getBonds(float cutfac) const;
    const std::vector<Bond>& getBondsCell() const;
    const std::vector<Bond>& getBondsCell(float cutfac) const;
    size_t  getNbond(void) const noexcept;
    std::shared_ptr<PseMap> pse;
private:
    Atom formatAtom(Atom at, AtomFmt source, AtomFmt target) const;
    std::vector<Atom> formatAtoms(std::vector<Atom> atvec, AtomFmt source, AtomFmt target) const;
    void setBonds(float cutfac) const;
    void setBondsCell(float cutfac) const;
    void checkBond(std::size_t i, std::size_t j, float cutfac, Vec dist, std::array<int, 3> offset) const;
    enum class BondLevel { None, Molecule, Cell };
    //DATA following:
    AtomFmt format=AtomFmt::Bohr;
    std::string comment;
    std::vector<Atom> atoms;
    float celldim {1.};
    Mat cellvec {{ {{1.,0.,0.}}, {{0.,1.,0.}}, {{0.,0.,1.}} }};
    Mat invvec {{ {{1.,0.,0.}}, {{0.,1.,0.}}, {{0.,0.,1.}} }};
    mutable bool bonds_outdated{true};
    mutable BondLevel bonds_level{BondLevel::None};
    mutable float bondcut_factor{1.1};
    mutable std::vector<Bond> bonds;
};
std::ostream& operator<< (std::ostream& s, const Step& st);

}
#endif // STEP_H
