#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include "../basewidget.h"
#include "vipster/fileio.h"

namespace Ui {
class PresetWidget;
}

class PresetWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit PresetWidget(QWidget *parent = nullptr);
    ~PresetWidget() override;
    std::vector<std::pair<std::string, Vipster::IO::Preset>> presets;
    void registerPreset(const std::string& name,
                        const Vipster::IO::Preset& data);
    void clearPresets();
    Vipster::IO::Preset* curPreset{nullptr};

private slots:
    void on_presetSel_currentIndexChanged(int index);

    void on_helpButton_clicked();

private:
    Ui::PresetWidget *ui;
};

#endif // CONFIGWIDGET_H
