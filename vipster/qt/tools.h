#ifndef TOOLS_H
#define TOOLS_H

#include "basewidget.h"

#include "tools/cellmodwidget.h"
#include "tools/definewidget.h"
#include "tools/millerwidget.h"
#include "tools/pickwidget.h"
#include "tools/pinwidget.h"
#include "tools/scriptwidget.h"
#ifdef VIPSTER_LAMMPS
#include "tools/lammpswidget.h"
#endif

std::vector<std::pair<BaseWidget*, QString>> makeToolWidgets(QWidget* parent)
{
    return {
        {new PickWidget(parent), "Selected Atoms"},
        {new DefineWidget(parent), "Filter Atoms"},
        {new MillerWidget(parent), "Lattice Planes"},
        {new PinWidget(parent), "Pin Steps"},
        {new CellModWidget(parent), "Modify Cell"},
        {new ScriptWidget(parent), "Script"},
#ifdef VIPSTER_LAMMPS
        {new LammpsWidget(parent), "LAMMPS"}
#endif
    };
}

#endif // TOOLS_H
