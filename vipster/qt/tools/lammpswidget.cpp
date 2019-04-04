#include <QMessageBox>

#include <thread>

#include "lammpswidget.h"
#include "ui_lammpswidget.h"

#include "step.h"

using namespace Vipster;

class ExitError:public std::runtime_error{
public:
    ExitError(std::string error):std::runtime_error(error){}
};

class AbortError:public std::runtime_error{
public:
    AbortError(std::string error):std::runtime_error(error){}
};

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
    // broadcast command to lammps-workers
    auto len = cmd.size();
    MPI_Bcast(&len, 1, MPI_INT, MPI_ROOT, intercomm);
    MPI_Bcast((void*)cmd.c_str(), cmd.size(), MPI_CHAR, MPI_ROOT, intercomm);
    // collect results
    std::string errMsg{};
    LMPResult results[size];
    for(int i=0; i<size; ++i){
	MPI_Status status;
        LMPResult tmp;
        MPI_Recv(&tmp, 1, MPI_BYTE, MPI_ANY_SOURCE, 0, intercomm, &status);
        results[status.MPI_SOURCE] = tmp;
        if(tmp != LMPResult::SUCCESS){
            if(errMsg.empty()){
                errMsg = std::string{"Error executing \""} + cmd + "\":\n\n";
            }
            uint64_t len;
            MPI_Recv(&len, 1, MPI_UINT64_T, status.MPI_SOURCE, 1, intercomm, &status);
            char *buf = new char[len+1];
            buf[len] = '\0';
            MPI_Recv(buf, len, MPI_CHAR, status.MPI_SOURCE, 1, intercomm, &status);
            errMsg += "Error in process " + std::to_string(i) + ": " + buf;
            delete[] buf;
            if(tmp == LMPResult::ABORT){
		throw(AbortError{errMsg});
            }
        }
    }
    // display error and throw so we exit the eval-loop
    if(!errMsg.empty()){
	throw(ExitError(errMsg));
    }
}

void LammpsWidget::sendAbort()
{
    for(int i=0; i<size; ++i){
	uint8_t dummy{};
	MPI_Send(&dummy, 1, MPI_UINT8_T, i, 2, intercomm);
    }
    MPI_Comm_disconnect(&intercomm);
}

void LammpsWidget::sendOp(LMPMessage op, const std::string& cmd)
{
    switch(op){
    case LMPMessage::CMD:
        // transmit command
	MPI_Bcast(&op, 1, MPI_UINT8_T, MPI_ROOT, intercomm);
        sendCmd(cmd);
        break;
    case LMPMessage::ABORT:
	sendAbort();
	return;
    case LMPMessage::EXIT:
	MPI_Bcast(&op, 1, MPI_UINT8_T, MPI_ROOT, intercomm);
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
    auto total_steps = ui->nStep->value();
    auto granularity = ui->granularity->value();
    auto iterations = total_steps/granularity;
    auto last_iter = total_steps%granularity;
    std::string run, run_last;
    run = "run " + std::to_string(granularity);
    run_last = "run " + std::to_string(last_iter);
    try{
	auto script = ui->scriptInput->toPlainText().split('\n');
	for(const auto& cmd: script){
	    sendOp(LMPMessage::CMD, cmd.toStdString());
	}
        for(size_t i=0; i<iterations; ++i){
            sendOp(LMPMessage::CMD, run);
        }
        if(last_iter){
            sendOp(LMPMessage::CMD, run_last);
        }
        sendOp(LMPMessage::EXIT);
    }catch(const AbortError& e){
	sendOp(LMPMessage::ABORT);
	QMessageBox msg{this};
	msg.setText(e.what());
	msg.exec();
    }catch(const ExitError& e){
	sendOp(LMPMessage::EXIT);
	QMessageBox msg{this};
	msg.setText(e.what());
	msg.exec();
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
        info, // allow for oversubscription
        0,  // am singleton, am root
        MPI_COMM_SELF, // spawning communicator
        &intercomm, // resulting intercommunicator
        MPI_ERRCODES_IGNORE); //ignore errors, return value should be enough
    if(!spawned){
	// check if workers report success
	LMPResult res;
	MPI_Status status;
        MPI_Recv(&res, 1, MPI_BYTE, MPI_ANY_SOURCE, 0, intercomm, &status);
	if(res != LMPResult::SUCCESS){
	    spawned = 1;
	}
    }
    if(!spawned){
        // this call should be threaded!
        work();
        ui->runButton->setEnabled(true);
        running = false;
        size = -1;
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
