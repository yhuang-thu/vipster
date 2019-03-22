#ifndef LAMMPSWIDGET_H
#define LAMMPSWIDGET_H

#include <QWidget>
#include <mpi.h>
#include "basewidget.h"
#include "wraplammps.h"
#include "global.h"

namespace Ui {
class LammpsWidget;
}

class LammpsWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit LammpsWidget(QWidget *parent = nullptr);
    ~LammpsWidget();

private slots:
    void on_runButton_clicked();

private:
    void work();
    void sendCmd(const std::string& cmd);
    void sendOp(LMPMessage op, const std::string& cmd);
    bool running{false};
    int size{-1};
    MPI_Comm intercomm;
    Ui::LammpsWidget *ui;
    std::vector<LMPMessage> operations{
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::CMD,
        LMPMessage::EXIT,
    };
    std::vector<std::string> commands{
        "units real",
        "dimension 3",
        "newton on",
        "boundary p p p",
        "atom_style atomic",
        "",
        "#test comment",
        "read_data bargl",
    };
};

#endif // LAMMPSWIDGET_H
