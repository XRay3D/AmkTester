#include "elemer.h"

Elemer::Elemer()
{
}

QByteArray Elemer::ControlSum(const QByteArray& data)
{
    quint16 KS = 0xFFFF;
    for (int n = 1; n < data.size(); ++n) {
        KS ^= data.at(n);
        for (int i = 0; i < 8; ++i) {
            if (KS & 0x01)
                KS = (KS >> 1) ^ 40961;
            else
                KS >>= 1;
        }
    }
    return QByteArray().setNum(KS);
}

int Elemer::Check(const QByteArray& Parcel, QList<QByteArray>& Array)
{
    QByteArray data(Parcel);
    while (data.size() && data[0] != '!') {
        data.remove(0, 1);
    }
    if (data[0] == '!' && data.count(';')) {
        Array = data.split(';');
        data.clear();
        for (int i = 0; i < Array.size() - 1; ++i) {
            data += Array.at(i) + ";";
        }
        if (Array.last().count(ControlSum(data))) {
            return Array.size();
        }
    }
    return 0;
}
