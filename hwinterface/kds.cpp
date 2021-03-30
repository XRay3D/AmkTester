#include "kds.h"

#include <QDebug>

using namespace Elemer;

Kds::Kds()
    : Device()
{
}

uint16_t Kds::getData(DataType type, bool* ok)
{
    uint16_t data {};
    bool success {};
    success = isConnected() && (write<Cmd::GetData>(type), read(data));
    if (ok)
        *ok = success;
    qDebug() << __FUNCTION__ << success;
    return data;
}

bool Kds::setRelay(uint16_t data)
{
    bool success = isConnected() && write<Cmd::SetRelay>(data) == RetCcode::OK;
    qDebug() << __FUNCTION__ << success;
    return success;
}

bool Kds::writeRelay(uint16_t data)
{
    bool success = isConnected() && write<Cmd::WriteRelay>(data) == RetCcode::OK;
    qDebug() << __FUNCTION__ << success;
    return success;
}

bool Kds::writeSerNum(uint16_t sn)
{
    bool success = isConnected() && write<Cmd::WriteSerNum>(sn) == RetCcode::OK;
    qDebug() << __FUNCTION__ << success;
    return success;
}

bool Kds::writeChCount(uint16_t count)
{
    bool success = isConnected() && write<Cmd::WriteChCount>(count) == RetCcode::OK;
    qDebug() << __FUNCTION__ << success;
    return success;
}

uint8_t Kds::getProtocolType(bool* ok)
{
    uint8_t data {};
    bool success = isConnected() && read<Cmd::GetProtocolType>(data);
    if (ok)
        *ok = success;
    qDebug() << __FUNCTION__ << success;
    return data;
}

bool Kds::writeDevAddress(uint8_t address)
{
    bool success = isConnected() && write<Cmd::WriteDevAddress>(address) == RetCcode::OK;
    if (success)
        setAddress(address);
    qDebug() << __FUNCTION__ << success;
    return success;
}

bool Kds::writeDevBaud(Baud baud)
{
    bool success = isConnected() && write<Cmd::WriteDevBaud>(baud) == RetCcode::OK;
    if (success)
        port()->setBaudRate(bauds[baud]);
    qDebug() << __FUNCTION__ << success;
    return success;
}

QByteArray Kds::getVer(bool* ok)
{
    QByteArray data {};
    bool success = isConnected() && read<Cmd::GetVer>(data) == RetCcode::OK;
    if (ok)
        *ok = success;
    qDebug() << __FUNCTION__ << success;
    return data;
}

bool Kds::resetCpu()
{
    bool success = isConnected() && write<Cmd::ResetCpu>() == RetCcode::OK;
    qDebug() << __FUNCTION__ << success;
    return success;
}
