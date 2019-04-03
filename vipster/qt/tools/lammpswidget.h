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
    void sendAbort();
    void sendOp(LMPMessage op, const std::string& cmd);
    bool running{false};
    int size{-1};
    MPI_Comm intercomm;
    Ui::LammpsWidget *ui;
};

#endif // LAMMPSWIDGET_H
