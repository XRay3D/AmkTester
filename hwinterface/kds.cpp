#include "kds.h"

#include <QDebug>

using namespace Elemer;

Kds_::Kds_()
    : AsciiDevice()
{
}

uint16_t Kds_::getData(DataType type, bool* ok)
{
    if (m_connected) {
        emit write(createParcel(m_address, AmkCmd::GetData, type));
        if (wait())
            return m_data[1].toUInt(ok);
    }
    if (ok)
        *ok = false;
    return {};
}

bool Kds_::setRelay(uint16_t data) { return m_connected && writeData(AmkCmd::SetRelay, data) == RretCcode::OK; }

bool Kds_::writeRelay(uint16_t data) { return m_connected && writeData(AmkCmd::WriteRelay, data) == RretCcode::OK; }

bool Kds_::writeSerNum(uint16_t sn) { return m_connected && writeData(AmkCmd::WriteSerNum, sn) == RretCcode::OK; }

bool Kds_::writeChCount(uint16_t count) { return m_connected && writeData(AmkCmd::WriteChCount, count) == RretCcode::OK; }

uint8_t Kds_::getProtocolType(bool* ok)
{
    if (m_connected) {
        emit write(createParcel(m_address, AmkCmd::GetProtocolType));
        if (wait())
            return m_data[0].toInt();
    }
    return false;
}

bool Kds_::writeDevAddress(uint8_t address)
{
    bool success = m_connected && writeData(AmkCmd::WriteDevAddress, address) == RretCcode::OK;
    if (success)
        setAddress(address);
    return success;
}

bool Kds_::writeDevBaud(Baud baud)
{
    bool success = m_connected && writeData(AmkCmd::WriteDevBaud, baud) == RretCcode::OK;
    if (success)
        port()->setBaudRate(bauds[baud]);
    return success;
}

bool Kds_::fileOpen() { return m_connected && writeData(AmkCmd::FileOpen) == RretCcode::OK; }

bool Kds_::fileSeek(uint16_t offset, Seek seek) { return m_connected && writeData(AmkCmd::FileSeek, offset, seek) == RretCcode::OK; }

bool Kds_::fileClose() { return m_connected && writeData(AmkCmd::FileClose) == RretCcode::OK; }

QByteArray Kds_::getVer(bool* ok)
{
    if (m_connected && readData(AmkCmd::GetVer))
        return m_data[1];
    return {};
}

bool Kds_::resetCpu() { return m_connected && writeData(AmkCmd::ResetCpu) == RretCcode::OK; }
