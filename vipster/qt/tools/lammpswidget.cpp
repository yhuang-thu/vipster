#include <QMessageBox>

#include <thread>
#include <mpi.h>

#include "lammpswidget.h"
#include "ui_lammpswidget.h"

#include "step.h"

#include "wraplammps.h"

using namespace Vipster;

LammpsWidget::LammpsWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::LammpsWidget)
{
    ui->setupUi(this);
    auto nproc = std::thread::hardware_concurrency();
    ui->nProc->setMaximum(nproc);
    ui->nProc->setValue((nproc>1) ? (nproc-1) :1);
    int tmp{0};
    MPI_Init(&tmp, nullptr);
}

LammpsWidget::~LammpsWidget()
{
    MPI_Finalize();
    delete ui;
}

void work(MPI_Comm comm)
{
    int me;
    MPI_Comm_rank(comm, &me);
    if(me){
        throw Error("Lammps-widget: Am not root of intercommunicator");
    }
    std::vector<LMPMessage> messages{
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::ABORT,
    };
    auto abort{LMPMessage::ABORT};
    for(auto& msg: messages){
        MPI_Bcast(&msg, 1, MPI_UINT8_T, MPI_ROOT, comm);
        switch(msg){
        case LMPMessage::CMD:
            // TODO: transmitt message
            break;
        case LMPMessage::ABORT:
            return;
        default:
            // should not arrive here, abort execution
            MPI_Bcast(&abort, 1, MPI_UINT8_T, MPI_ROOT, comm);
            break;
        }
    }
}

void LammpsWidget::on_runButton_clicked()
{
    MPI_Comm intercomm;
    // prepare argv to launch in lammps-mode
    char **argv = new char*[2];
    argv[0] = "__LMP__";
    argv[1] = nullptr;
    // prepare info to allow oversubscribing, don't fail when hyperthreading is used
    MPI_Info info;
    MPI_Info_create(&info);
    MPI_Info_set(info, "map_by", "core:OVERSUBSCRIBE");
    MPI_Comm_set_errhandler(MPI_COMM_SELF, MPI_ERRORS_RETURN);
    // spawn workers
    int spawned = MPI_Comm_spawn(
        QApplication::arguments()[0].toUtf8(), // relaunch this application
        argv, // given __LMP__ as argument, which will trigger the lammps-wrapping
        ui->nProc->value(), // spawning this many processes
        info, // don't use info
        0,  // am singleton, am root
        MPI_COMM_SELF, // spawning communicator
        &intercomm, // resulting intercommunicator
        MPI_ERRCODES_IGNORE); //ignore errors, return value should be enough
    if(!spawned){
        // this call should be threaded!
        work(intercomm);
    }else{
        QMessageBox msg{this};
        msg.setText("Failed to spawn LAMMPS processes.\n"
                    "Detailed information may be found in stdout and/or stderr.\n"
                    "Vipster will exit now, as validity of MPI state can not be guaranteed.\n"
                    "If this problem persists, please file a bug.");
        msg.exec();
        QApplication::exit(1);
    }
}
