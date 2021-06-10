#pragma once

#include <QDialog>

struct ResistanceMatrix;

namespace Ui {
class ResultDialog;
}

class ResultDialog : public QDialog {
    Q_OBJECT

public:
    explicit ResultDialog(const ResistanceMatrix& ref, const ResistanceMatrix& result, QWidget* parent = nullptr);
    ~ResultDialog();

protected:
    void changeEvent(QEvent* e);

private:
    Ui::ResultDialog* ui;
};
