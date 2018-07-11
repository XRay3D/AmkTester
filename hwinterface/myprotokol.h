#ifndef MYPROTOKOL_H
#define MYPROTOKOL_H

#include <QByteArray>
#include <QElapsedTimer>
#include <stdint.h>

#pragma pack(push, 1)

typedef struct Parcel_t {
    quint16 start;
    quint8 len;
    quint8 cmd;
    quint8 data[1];
} Parcel_t;

#pragma pack(pop)

enum {
    TX = 0xAA55,
    RX = 0x55AA,
    MIN_LEN = 5
};

class MyProtokol {
public:
    explicit MyProtokol();
    template <typename T>
    QByteArray parcel(quint8 cmd, const T& value)
    {
        QByteArray data(MIN_LEN + sizeof(T), 0);
        Parcel_t* d = reinterpret_cast<Parcel_t*>(data.data());
        *d = { TX, static_cast<quint8>(data.size()), cmd, 0 };
        memcpy(d->data, &value, sizeof(T));
        d->data[sizeof(T)] = calcCrc(data); //crc
        return data;
    }

    template <typename T>
    T value(const QByteArray& data) const { return *reinterpret_cast<const T*>(data.constData() + 4); }

    template <typename T>
    T pValue(const QByteArray& data) const { return reinterpret_cast<const T*>(data.constData() + 4); }

    QByteArray parcel(quint8 cmd);

    bool checkParcel(const QByteArray& data);

    quint8 calcCrc(const QByteArray& data);

private:
    enum { POLYNOMIAL = 0x1D }; // x^8 + x^4 + x^3 + x^2 + 1
    static const uint8_t array[0x100];
};

#endif // MYPROTOKOL_H
