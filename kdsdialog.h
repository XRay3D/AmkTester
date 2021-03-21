#ifndef KDSDIALOG_H
#define KDSDIALOG_H

#include <QDialog>

namespace Ui {
class KdsDialog;
}

class KdsDataModel;
class Kds_;

class KdsDialog : public QDialog {
    Q_OBJECT

public:
    explicit KdsDialog(Kds_* kds, QWidget* parent = nullptr);
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
    Kds_* const kds;
};

#endif // KDSDIALOG_H
