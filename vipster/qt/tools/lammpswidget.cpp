#include <QMessageBox>

#include <thread>

#include "lammpswidget.h"
#include "ui_lammpswidget.h"

#include "step.h"

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

void LammpsWidget::sendCmd(const std::string& cmd)
{
    MPI_Status status;
    // broadcast command to lammps-workers
    auto len = cmd.size();
    MPI_Bcast(&len, 1, MPI_INT, MPI_ROOT, intercomm);
    MPI_Bcast((void*)cmd.c_str(), cmd.size(), MPI_CHAR, MPI_ROOT, intercomm);
    // collect results
    std::string errMsg{};
    LMPResult results[size];
    for(int i=0; i<size; ++i){
        LMPResult tmp;
        MPI_Recv(&tmp, 1, MPI_BYTE, MPI_ANY_SOURCE, 0, intercomm, &status);
        results[status.MPI_SOURCE] = tmp;
        if(tmp){
            if(errMsg.empty()){
                errMsg = std::string{"Error executing \""} + cmd + "\":\n\n";
            }
            int len;
            MPI_Recv(&len, 1, MPI_INT, i, 1, intercomm, &status);
            char *buf = new char[len+1];
            buf[len] = '\0';
            MPI_Recv(buf, len, MPI_CHAR, i, 1, intercomm, &status);
            errMsg += "Error in process " + std::to_string(i) + ": " + buf;
            delete[] buf;
            if(tmp == LMPResult::ABORT){
                // If lammps needs to be aborted, inform user that program state has become invalid
                // and block widget for remainder of runtime
                errMsg += "This error cannot be recovered.\n"
                          "Restart Vipster to reenable the LAMMPS-Widget.";
                QMessageBox msg{this};
                msg.setText(QString::fromStdString(errMsg));
                msg.exec();
                throw(Error{"ABORT"});
            }
        }
    }
    // display error and throw so we exit the eval-loop
    if(!errMsg.empty()){
        QMessageBox msg{this};
        msg.setText(QString::fromStdString(errMsg));
        msg.exec();
        throw(Error{"FAIL"});
    }
}

void LammpsWidget::sendOp(LMPMessage op, const std::string& cmd)
{
    MPI_Bcast(&op, 1, MPI_UINT8_T, MPI_ROOT, intercomm);
    switch(op){
    case LMPMessage::CMD:
        // transmit command
        sendCmd(cmd);
        break;
    case LMPMessage::EXIT:
        ui->runButton->setEnabled(true);
        running = false;
        size = -1;
        return;
    case LMPMessage::ABORT:
        return;
    }
}

void LammpsWidget::work()
{
    int me;
    MPI_Comm_rank(intercomm, &me);
    if(me){
        throw Error("Lammps-widget: Am not root of intercommunicator");
    }
    size_t total_steps, granularity, iterations, last_iter;
    //TODO: these two shall be fed from gui
    total_steps = 10000;
    granularity = 10;
    iterations = total_steps/granularity;
    last_iter = total_steps%granularity;
    std::string run, run_last;
    run = "run " + std::to_string(granularity);
    run_last = "run " + std::to_string(last_iter);
    try{
        for(auto& cmd: commands){
            sendOp(LMPMessage::CMD, cmd);
        }
        for(size_t i=0; i<iterations; ++i){
            sendOp(LMPMessage::CMD, run);
        }
        if(last_iter){
            sendOp(LMPMessage::CMD, run_last);
        }
        sendOp(LMPMessage::EXIT, "");
    }catch(const Error& e){
        if(!strcmp(e.what(), "ABORT")){
            sendOp(LMPMessage::ABORT, "");
        }else{
            sendOp(LMPMessage::EXIT, "");
        }
    }
}

void LammpsWidget::on_runButton_clicked()
{
    //TODO: not working yet
    // prevent double spawning if button is clicked multiple times before setDisabled is executed
    if(running) return;
    ui->runButton->setDisabled(true);
    size = ui->nProc->value();
    running = true;
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
        size, // spawning this many processes
        info, // don't use info
        0,  // am singleton, am root
        MPI_COMM_SELF, // spawning communicator
        &intercomm, // resulting intercommunicator
        MPI_ERRCODES_IGNORE); //ignore errors, return value should be enough
    if(!spawned){
        // this call should be threaded!
        work();
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
