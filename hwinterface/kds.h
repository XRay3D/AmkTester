#ifndef KDS_H
#define KDS_H

#include "common_interfaces.h"
#include <QtSerialPort>

using namespace std;

class KdsPort;

class KDS : public QObject, public COMMON_INTERFACES {
    Q_OBJECT
    friend class KdsPort;

public:
    KDS();
    bool Ping(const QString& portName = QString(), int baud = 9600, int addr = 0);
    int getDev(int addr);
    bool setOut(int addr, int value);

private:
    uint getUintData(QByteArray data);
    bool getSuccess(QByteArray data);
    QString CalcCrc(QByteArray& parcel);
    QByteArray m_data;

    int dev;
    int serNum;
    int address;
    int chMum;
    int baudRate;
    int outPin;
    int out;

signals:
    void Open(int mode);
    void Close();
    void SetValue(const QVector<quint16>&);
    void Write(const QByteArray& data);

private:
    bool m_connected = false;
    bool m_result = false;
    int m_counter = 0;
    KdsPort* m_port;
    QMutex m_mutex;
    QSemaphore m_semaphore;
    QThread m_portThread;
};

class KdsPort : public QSerialPort {
    Q_OBJECT

public:
    KdsPort(KDS* kds);
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);
    KDS* m_kds;

private:
    void Read();
    QByteArray m_data;
    QMutex m_mutex;
};
#endif // KDS_H
