#include "wraplammps.h"

#include <mpi.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#ifndef LAMMPS_EXCEPTIONS
#define LAMMPS_EXCEPTIONS
#endif
#include "lammps/src/library.h"
#include "lammps/src/lammps.h"
#include "lammps/src/input.h"
#include "lammps/src/error.h"
#include "lammps/src/exceptions.h"
#undef LAMMPS_EXCEPTIONS

using namespace LAMMPS_NS;
using namespace std::chrono_literals;

static bool running{true};

void runCommand(LAMMPS* lmp, MPI_Comm intercomm, int me, int size)
{
    static constexpr uint8_t SUCC{LMPResult::SUCCESS};
    static constexpr uint8_t FAIL{LMPResult::FAIL};
    static constexpr uint8_t ABORT{LMPResult::ABORT};
    //obtain command
    int cmdSize;
    MPI_Bcast(&cmdSize, 1, MPI_INT, 0, intercomm);
    char *buf = new char[cmdSize+1];
    MPI_Bcast(buf, cmdSize, MPI_CHAR, 0, intercomm);
    buf[cmdSize] = '\0';
   try {
        lmp->input->one(buf);
    } catch (const LAMMPSAbortException& e){
        if(size>1){
            MPI_Send(&ABORT, 1, MPI_UINT8_T, 0, 0, intercomm);
        }else{
            MPI_Send(&FAIL, 1, MPI_UINT8_T, 0, 0, intercomm);
        }
        uint64_t len = e.message.size();
        MPI_Send(&len, 1, MPI_UINT64_T, 0, 1, intercomm);
        MPI_Send(e.message.c_str(), len, MPI_CHAR, 0, 1, intercomm);
	return;
    } catch (const LAMMPSException& e) {
        MPI_Send(&FAIL, 1, MPI_UINT8_T, 0, 0, intercomm);
        auto len = e.message.size();
        MPI_Send(&len, 1, MPI_UINT64_T, 0, 1, intercomm);
        MPI_Send(e.message.c_str(), len, MPI_CHAR, 0, 1, intercomm);
	return;
    }
    MPI_Send(&SUCC, 1, MPI_UINT8_T, 0, 0, intercomm);
}

void runLammps(LAMMPS *lmp, MPI_Comm intercomm, int me, int size)
{
    LMPMessage msg;
    while(true){
        MPI_Bcast(&msg, 1, MPI_UINT8_T, 0, intercomm);
        switch(msg){
        case LMPMessage::CMD:
            //execute command, return SUCCESS or fail upon exception
            runCommand(lmp, intercomm, me, size);
            break;
        case LMPMessage::EXIT:
            return;
        case LMPMessage::ABORT:
            return;
        }
    }
}

void waitAbort(MPI_Comm intercomm, int me)
{
    while(running){
	std::this_thread::sleep_for(1s);
	int flag{0};
	MPI_Status status;
	MPI_Iprobe(0, 2, intercomm, &flag, &status);
	if(flag){
	    uint8_t dummy;
	    MPI_Recv(&dummy, 1, MPI_UINT8_T, 0, 2, intercomm, &status);
	    MPI_Comm_disconnect(&intercomm);
	    MPI_Finalize();
	    exit(0);
	}
    }
}

void launchLammps(){
    static constexpr uint8_t SUCC{LMPResult::SUCCESS};
    static constexpr uint8_t FAIL{LMPResult::FAIL};
    static constexpr uint8_t ABORT{LMPResult::ABORT};
    int level;
    int tmp{0};
    MPI_Init_thread(&tmp, nullptr, MPI_THREAD_MULTIPLE, &level);
    MPI_Comm intercomm;
    MPI_Comm_get_parent(&intercomm);
    int me, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if((size>1) && (level != MPI_THREAD_MULTIPLE)){
	// if we can't get real multithreading, prohibit using multiple process
	MPI_Send(&FAIL, 1, MPI_UINT8_T, 0, 0, intercomm);
	MPI_Finalize();
	return;
    }
    MPI_Send(&SUCC, 1, MPI_UINT8_T, 0, 0, intercomm);
    // launch thread that shall wait for abort-signal
    auto abort = std::thread(waitAbort, intercomm, me);
    // disable regular lammps-output
    std::array<char*,5> lmparg{
        nullptr,
        "-screen", "none",
        "-log","none"
    };
    auto *lmp = new LAMMPS{lmparg.size(), lmparg.data(), MPI_COMM_WORLD};
    runLammps(lmp, intercomm, me, size);
    delete lmp;
    running = false;
    abort.join();
    MPI_Finalize();
}
