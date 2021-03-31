#include "myprotokol.h"
//#include <QDebug>

MyProtokol::MyProtokol()
{
}

bool MyProtokol::CheckData(const QByteArray& data)
{
    if (data.size() > 4) {
        if (data.at(0) == -86 && data.at(1) == 85) {
            if (int(data[2]) == data.size()) {
                if (quint8(data[data.size() - 1]) == CalcCrc(data)) {
                    return true;
                }
            }
        }
    }
    return false;
}

QByteArray MyProtokol::Parcel(quint8 cmd, quint8* pData, quint8 Len)
{
    QByteArray data;
    data.clear();
    data.push_back(85); //старт 0x55
    data.push_back(-86); //старт 0xAA
    data.push_back(Len + 5); //размер посылки
    data.push_back(cmd);
    for (int i = 0; i < Len; ++i) {
        data.push_back(*pData++); //данные
    }
    data.push_back(1); //байт для crc
    data[data.size() - 1] = CalcCrc(data); //crc
    //    qDebug() << "Parcel" << data.toHex().toUpper();
    return data;
}

QByteArray MyProtokol::Parcel(quint8 cmd)
{
    return Parcel(cmd, nullptr, 0);
}

quint8 MyProtokol::CalcCrc(const QByteArray& data)
{
    quint8 crc8 = 0;
    quint8 len = data.size() - 1;
    for (quint8 i = 0; i < len; ++i) {
        crc8 ^= data[i];
        for (quint8 j = 0; j < 8; ++j) {
            if (crc8 & 0x80)
                crc8 = (crc8 << 1) ^ POLYNOMIAL;
            else
                crc8 <<= 1;
        }
    }
    return crc8;
}

QByteArray MyProtokol::Parcel(quint8 cmd, const QByteArray& data)
{
    return Parcel(cmd, (quint8*)data.data(), (quint8)data.size());
}
