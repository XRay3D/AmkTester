#ifndef MYPROTOKOL_H
#define MYPROTOKOL_H

#include <QByteArray>

class MyProtokol {
public:
    MyProtokol();

    QByteArray Parcel(quint8 cmd, quint8* pData, quint8 Len);
    QByteArray Parcel(quint8 cmd, const QByteArray& data);
    QByteArray Parcel(quint8 cmd);
    bool CheckData(const QByteArray& data);
    quint8 CalcCrc(const QByteArray& data);

private:
    enum { POLYNOMIAL = 0x1D }; // x^8 + x^4 + x^3 + x^2 + 1
};

#endif // MYPROTOKOL_H
