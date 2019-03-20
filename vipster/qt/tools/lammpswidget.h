#ifndef LAMMPSWIDGET_H
#define LAMMPSWIDGET_H

#include <QWidget>
#include "basewidget.h"

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
    Ui::LammpsWidget *ui;
};

#endif // LAMMPSWIDGET_H
