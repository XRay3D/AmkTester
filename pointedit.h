#pragma once

#include <QDialog>

class QFrame;
class QGridLayout;
class QJsonObject;
class QLabel;
class QLineEdit;
class QSpinBox;

namespace Ui {
class PointEdit;
}

struct Point {
    Point() { }
    Point(const QJsonObject& object);
    uint16_t Parcel {};
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

    QFrame* line;
    QGridLayout* gridLayout;
    QLabel* lblDescription;
    QLabel* lblParcel;
    QLineEdit* leDescription;
    QPushButton* pbClean;
    QPushButton* pbNo;
    QPushButton* pbYes;
    QSpinBox* sbxParcel;

    void setupUi(QWidget* PointEdit); // setupUi

private slots:
    void on_spinBox_Parcel_valueChanged(int arg1);

private:
    QLineEdit* lineEdit;
    Point& point;
    void Yes();
    bool flag;
    uint lastParcVal;
};
