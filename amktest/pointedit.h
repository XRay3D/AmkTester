#ifndef POINTEDIT_H
#define POINTEDIT_H

#include <QDialog>
#include <QGridLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

namespace Ui {
class PointEdit;
}

class PointEdit : public QDialog {
    Q_OBJECT

public:
    struct Point {
        Point() {}
        Point(const QJsonObject& object)
            : Parcel(object["value"].toString())
            , Description(object["name"].toString())
        {
        }
        QString Parcel;
        QString Description;
    };

    explicit PointEdit(Point* point, QLineEdit* lineEdit, QWidget* parent = 0);
    ~PointEdit();

    QGridLayout* gridLayout;
    QLabel* lblParcel;
    QLabel* lblDescription;
    QPushButton* pbClean;
    QPushButton* pbYes;
    QPushButton* pbNo;
    QLineEdit* leDescription;
    QFrame* line;
    QSpinBox* sbxParcel;

    void setupUi(QWidget* PointEdit); // setupUi

    void retranslateUi(QWidget* PointEdit); // retranslateUi

private slots:
    void on_spinBox_Parcel_valueChanged(int arg1);

private:
    QLineEdit* lineEdit;
    Point* point;
    void Yes();
    bool flag;
    uint lastParcVal;
};

#endif // POINTEDIT_H
