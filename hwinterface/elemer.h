#ifndef ELEMER_H
#define ELEMER_H

#include <QDebug>
#include <QtGui>
#include <stdint.h>

class Elemer {
public:
    explicit Elemer();
    static QByteArray ControlSum(const QByteArray& data);
    static int Check(const QByteArray& Parcel, QList<QByteArray>& Array);
};

#endif // ELEMER_H
