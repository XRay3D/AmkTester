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

struct Point {
    Point() { }
    Point(const QJsonObject& object)
        : Parcel(object["value"].toInt())
        , Description(object["name"].toString()) {
    }
    uint16_t Parcel{};
    QString Description;
    friend QDataStream& operator<<(QDataStream& stream, const Point& point) {
        stream << point.Parcel;
        stream << point.Description;
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, Point& point) {
        stream >> point.Parcel;
        stream >> point.Description;
        return stream;
    }
};

class PointEdit : public QDialog {
    Q_OBJECT

public:
    explicit PointEdit(Point& point, QLineEdit* lineEdit, QWidget* parent = 0);
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
    Point& point;
    void Yes();
    bool flag;
    uint lastParcVal;
};

#endif // POINTEDIT_H
