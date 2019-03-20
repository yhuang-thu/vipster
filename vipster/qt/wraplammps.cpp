#include "wraplammps.h"

#include <mpi.h>
#include <unistd.h>
#include <iostream>

//#include "lammps/src/library.h"
#include "lammps/src/lammps.h"

using namespace LAMMPS_NS;

void runLammps(LAMMPS *lmp)
{
    MPI_Comm intercomm;
    MPI_Comm_get_parent(&intercomm);
    int me;
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    LMPMessage msg;
    while(true){
        MPI_Bcast(&msg, 1, MPI_UINT8_T, 0, intercomm);
        switch(msg){
        case LMPMessage::CMD:
            //execute command, return SUCCESS or fail upon exception
            if(!me) std::cout << "executing something" << std::endl;
            break;
        case LMPMessage::ABORT:
            if(!me) std::cout << "aborting lammps" << std::endl;
            return;
        default:
            //fail here
            break;
        }
    }
}

void launchLammps(){
    int tmp{0};
    MPI_Init(&tmp, nullptr);
    std::array<char*,5> lmparg{
        nullptr,
        "-screen", "none",
        "-log","none"
    };
    auto *lmp = new LAMMPS{lmparg.size(), lmparg.data(), MPI_COMM_WORLD};
    runLammps(lmp);
    delete lmp;
    MPI_Finalize();
}
