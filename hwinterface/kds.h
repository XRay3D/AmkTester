#pragma once

#include <QtSerialPort>
#include <ascii_device.h>

enum /*class*/ RretCcode : uint8_t {
    OK,
    UNKNOWN_CMD,
    UNNOWN_DATA_TYPE,
    EEPROM_ERROR,
    EEPROM_ADDRES_TOO_BIG
};
// clang-format off
enum class AmkCmd : uint8_t {
    /* KDS */
    GetType         = 0,
    GetData         = 1,
    WriteDevAddr    = 3,
    WriteBaud       = 4,
    SetRelay         = 5,
    WriteRelay       = 6,
    WriteSerNum     = 7,
    WriteChCount    = 8,
    /* DEV */
    GetProtocolType = 32,
    WriteDevAddress = 33,
    WriteDevBaud    = 34,
    /* FILE */
    FileOpen        = 40,
    FileSeek        = 41,
    FileRead        = 42,
    FileWrite       = 43,
    FileClose       = 44,
    /* COM */
    GetVer          = 0XFE,
    ResetCpu        = 0XFF,
};
// clang-format on

class Kds_ final : public Elemer::AsciiDevice {
    Q_OBJECT
    static constexpr int bauds[] { 300, 600, 1200, 2400, 4800, 9600, 19200 };
    enum { WriteDelay = 10000 };

public:
    Kds_();
    Elemer::DeviceType type() const override { return Elemer::KDS; }

    enum DataType : uint8_t {
        IN,
        SerNum,
        ChCount,
        Data,
    };

    enum Baud : uint8_t {
        Baud300,
        Baud600,
        Baud1200,
        Baud2400,
        Baud4800,
        Baud9600,
        Baud19200,
    };
    enum Seek {
        Set,
        Cur,
        End,
    };

    /* KDS */
    uint16_t getData(DataType type, bool* ok = {});
    bool setRelay(uint16_t data);
    bool writeRelay(uint16_t data);
    bool writeSerNum(uint16_t sn);
    bool writeChCount(uint16_t count = {});
    /* DEV */
    uint8_t getProtocolType(bool* ok = {});
    bool writeDevAddress(uint8_t address);
    bool writeDevBaud(Baud baud);
    /* FILE */
    bool fileOpen();
    bool fileSeek(uint16_t offset, Seek seek = Seek::Set);
    template <typename T>
    T fileRead(bool* ok = {})
    {
        if (m_connected) {
            emit write(createParcel(m_address, AmkCmd::FileRead, sizeof(T)));
            if (wait()) {
                if (ok)
                    *ok = true;
                return fromHex<T>(1);
            }
        }
        if (ok)
            *ok = false;
        return {};
    }
    template <typename... Ts>
    bool fileWrite(Ts&&... data)
    {
        if (m_connected) {
            emit write(createParcel(m_address, AmkCmd::FileWrite, Elemer::Hex { data... }));
            if (wait() && success())
                return true;
        }
        return false;
    }
    bool fileClose();
    /* COM */
    QByteArray getVer(bool* ok = {});
    bool resetCpu();
};
