#ifndef MY_PROTOCOL_H
#define MY_PROTOCOL_H

#include "common_interfaces.h"
#include "myprotokol.h"
#include <QElapsedTimer>
#include <QMutex>
#include <QSemaphore>
#include <QSerialPort>
#include <QThread>
#include <QVector>

enum COMMAND {
    PING,
    MEASURE_PIN,
    GET_CALIBRATION_COEFFICIENTS,
    SET_CALIBRATION_COEFFICIENTS,
    BUFFER_OVERFLOW,
    WRONG_COMMAND,
    CRC_ERROR
};

class TesterPort;

class AmkTester : public QObject, private MyProtokol, /*private CallBack,*/ public CommonInterfaces {
    Q_OBJECT
    friend class TesterPort;

public:
    AmkTester(QObject* parent = 0);
    ~AmkTester();

    bool Ping(const QString& portName = QString(), int baud = 9600, int addr = 0);

    bool measurePin(int pin);
    bool setDefaultCalibrationCoefficients(quint8 pin);
    bool getCalibrationCoefficients(float& GradCoeff, int pin);
    bool setCalibrationCoefficients(float& GradCoeff, int pin);
    bool saveCalibrationCoefficients(quint8 pin);

signals:
    void open(int mode);
    void close();
    void write(const QByteArray& data);

    void measureReady(const QVector<quint16>&);

private:
    bool m_connected = false;
    bool m_result = false;
    int m_counter = 0;
    TesterPort* m_port;
    QMutex m_mutex;
    QSemaphore m_semaphore;
    QThread m_portThread;

    void reset();

    void rxPing(const QByteArray& data);
    void rxMeasurePin(const QByteArray& data);
    void rxGetCalibrationCoefficients(const QByteArray& data);
    void rxSetCalibrationCoefficients(const QByteArray& data);
    void rxBufferOverflow(const QByteArray& data);
    void rxWrongCommand(const QByteArray& data);
    void rxCrcError(const QByteArray& data);
};

class TesterPort : public QSerialPort, private MyProtokol {
    Q_OBJECT

public:
    TesterPort(AmkTester* t);
    void openSlot(int mode);
    void closeSlot();
    void writeSlot(const QByteArray& data);
    AmkTester* m_t;
    typedef void (AmkTester::*func)(const QByteArray&);
    QVector<func> m_f;

private:
    void readSlot();
    QByteArray m_data;
    QMutex m_mutex;
    qint64 counter = 0;
};

#endif // MY_PROTOCOL_H
