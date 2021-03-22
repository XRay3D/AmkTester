#include "kds.h"

#include <QDebug>

using namespace Elemer;

Kds_::Kds_()
    : AsciiDevice()
{
}

uint16_t Kds_::getData(DataType type, bool* ok)
{
    uint16_t data {};
    bool success {};
    switch (type) {
    case In:
        success = isConnected() && readStr<Cmd::GetData, DataType::In>(data);
        break;
    case SerNum:
        success = isConnected() && readStr<Cmd::GetData, DataType::SerNum>(data);
        break;
    case ChCount:
        success = isConnected() && readStr<Cmd::GetData, DataType::ChCount>(data);
        break;
    case Data:
        success = isConnected() && readStr<Cmd::GetData, DataType::Data>(data);
        break;
    default:;
    }

    if (ok)
        *ok = success;
    return data;
}

bool Kds_::setRelay(uint16_t data)
{
    bool success = isConnected() && writeStr<Cmd::SetRelay>(data) == RetCcode::OK;
    return success;
}

bool Kds_::writeRelay(uint16_t data)
{
    bool success = isConnected() && writeStr<Cmd::WriteRelay>(data) == RetCcode::OK;
    return success;
}

bool Kds_::writeSerNum(uint16_t sn)
{
    bool success = isConnected() && writeStr<Cmd::WriteSerNum>(sn) == RetCcode::OK;
    return success;
}

bool Kds_::writeChCount(uint16_t count)
{
    bool success = isConnected() && writeStr<Cmd::WriteChCount>(count) == RetCcode::OK;
    return success;
}

uint8_t Kds_::getProtocolType(bool* ok)
{
    uint8_t data {};
    bool success = isConnected() && readStr<Cmd::GetProtocolType>(data);
    if (ok)
        *ok = success;
    return data;
}

bool Kds_::writeDevAddress(uint8_t address)
{
    bool success = isConnected() && writeStr<Cmd::WriteDevAddress>(address) == RetCcode::OK;
    if (success)
        setAddress(address);
    return success;
}

bool Kds_::writeDevBaud(Baud baud)
{
    bool success = isConnected() && writeStr<Cmd::WriteDevBaud>(baud) == RetCcode::OK;
    if (success)
        port()->setBaudRate(bauds[baud]);
    return success;
}

QByteArray Kds_::getVer(bool* ok)
{
    QByteArray data {};
    bool success = isConnected() && readStr<Cmd::GetVer>(data) == RetCcode::OK;
    if (ok)
        *ok = success;
    return data;
}

bool Kds_::resetCpu()
{
    bool success = isConnected() && writeStr<Cmd::ResetCpu>() == RetCcode::OK;
    return success;
}
