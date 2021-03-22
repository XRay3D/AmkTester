#pragma once

#include <QtSerialPort>
#include <ed_device.h>

class Kds_ final : public Elemer::AsciiDevice {
    Q_OBJECT
    static constexpr int bauds[] { 300, 600, 1200, 2400, 4800, 9600, 19200 };
    enum { WriteDelay = 2000 };

public:
    Kds_();
    Elemer::DeviceType type() const override { return Elemer::KDS; }

    enum /*class*/ RetCcode : uint8_t {
        OK,
        UNKNOWN_CMD,
        UNNOWN_DATA_TYPE,
        EEPROM_ERROR,
        EEPROM_ADDRES_TOO_BIG
    };

    // clang-format off
    enum class Cmd : uint8_t {
        /* KDS */
        GetType         = 0,
        GetData         = 1,
        WriteDevAddr    = 2,
        WriteBaud       = 3,
        SetRelay        = 4,
        WriteRelay      = 5,
        WriteSerNum     = 6,
        WriteChCount    = 7,
        /* DEV */
        GetProtocolType = 32,
        WriteDevAddress = 33,
        WriteDevBaud    = 34,
        /* FILE */
        //FileOpen        = 40,
        //FileSeek        = 41,
        //FileRead        = 42,
        //FileWrite       = 43,
        //FileClose       = 44,
        /* COM */
        GetVer          = 0XFE,
        ResetCpu        = 0XFF,
    };
    // clang-format on

    enum DataType : uint8_t {
        In,
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
    /* COM */
    QByteArray getVer(bool* ok = {});
    bool resetCpu();
};
