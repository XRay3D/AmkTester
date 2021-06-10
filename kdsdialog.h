#pragma once

#include <QDialog>

namespace Ui {
class KdsDialog;
}

class KdsDataModel;
class Kds;

class KdsDialog : public QDialog {
    Q_OBJECT

public:
    explicit KdsDialog(Kds* kds, QWidget* parent = nullptr);
    ~KdsDialog();

private slots:
    void onSerNumSpinBoxEditingFinished();
    void onChCountSpinBoxEditingFinished();
    void onAddressSpinBoxEditingFinished();
    void onBaudComboBoxCurrentIndexChanged(int index);
    void onDataLineEditEditingFinished();

private:
    Ui::KdsDialog* ui;
    KdsDataModel* model;
    Kds* const kds;
};
