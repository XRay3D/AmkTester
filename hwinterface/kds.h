#pragma once

#include <QtSerialPort>
#include <ascii_device.h>

class AmkPort;

class Amk : public Elemer::AsciiDevice {
    Q_OBJECT

public:
    Amk();
    //    ~Amk();
    //    bool ping(const QString& portName = QString(), int baud = 9600, int addr = 0);
    //    int getDev(int addr);
    bool setOut(int addr, int value);
    virtual Elemer::DeviceType type() const override { return Elemer::KDS; }

private:
    QList<QByteArray> m_data;
    //    int dev;
    //    int serNum;
    //    int address;
    //    int chMum;
    //    int baudRate;
    //    int outPin;
    //    int out;

    //signals:
    //    void Open(int mode);
    //    void Close();
    //    void SetValue(const QVector<quint16>&);
    //    void Write(const QByteArray& data);

    //private:
    //    bool m_result = false;
    //    int m_counter = 0;
    //    AmkPort* m_port;
    //    QMutex m_mutex;
    //    QSemaphore m_semaphore;
    //    QThread m_portThread;
};
