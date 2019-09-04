#ifndef KDS_H
#define KDS_H

#include "common_interfaces.h"
#include <QtSerialPort>
#include <elemer_ascii.h>

using namespace std;

class AmkPort;

class Amk : public QObject, public CommonInterfaces, public ElemerASCII {
    Q_OBJECT
    friend class AmkPort;

public:
    Amk();
    ~Amk();
    bool Ping(const QString& portName = QString(), int baud = 9600, int addr = 0);
    int getDev(int addr);
    bool setOut(int addr, int value);

private:
    QList<QByteArray> m_data;
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
    bool m_result = false;
    int m_counter = 0;
    AmkPort* m_port;
    QMutex m_mutex;
    QSemaphore m_semaphore;
    QThread m_portThread;
};

class AmkPort : public QSerialPort, public ElemerASCII {
    Q_OBJECT

public:
    AmkPort(Amk* kds);
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);
    Amk* m_kds;

private:
    void Read();
    QByteArray m_data;
    QMutex m_mutex;
};
#endif // KDS_H
