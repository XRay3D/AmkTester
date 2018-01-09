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
    BUFFER_OVERFLOW = 0xDF,
    WRONG_COMMAND = 0xEF,
    CRC_ERROR = 0xFF
};

//class CallBack {
//public:
//    CallBack(QSemaphore* semaphore)
//        : semaphore(semaphore)
//    {
//    }
//    virtual void RxPing(const QByteArray& data) = 0;
//    virtual void RxMeasurePin(const QByteArray& data) = 0;
//    virtual void RxGetCalibrationCoefficients(const QByteArray& data) = 0;
//    virtual void RxSetCalibrationCoefficients(const QByteArray& data) = 0;
//    virtual void RxBufferOverflow(const QByteArray& data) = 0;
//    virtual void RxWrongCommand(const QByteArray& data) = 0;
//    virtual void RxCrcError(const QByteArray& data) = 0;
//    QSemaphore* semaphore;
//};

class TesterPort;

class AmkTester : public QObject, private MyProtokol, /*private CallBack,*/ public COMMON_INTERFACES {
    Q_OBJECT
    friend class TesterPort;

public:
    AmkTester(QObject* parent = 0);
    ~AmkTester();

    bool Ping(const QString& portName = QString(), int baud = 9600, int addr = 0);

    bool MeasurePin(int pin);
    bool SetDefaultCalibrationCoefficients(quint8 Channel);
    bool GetCalibrationCoefficients(float& GradCoeff, int pin);
    bool SetCalibrationCoefficients(float& GradCoeff, int pin);
    bool SaveCalibrationCoefficients(quint8 Channel);

signals:
    void Open(int mode);
    void Close();
    void SetValue(const QVector<quint16>&);
    void Write(const QByteArray& data);

private:
    bool m_connected = false;
    bool m_result = false;
    int m_counter = 0;
    TesterPort* m_port;
    QMutex m_mutex;
    QSemaphore m_semaphore;
    QThread m_portThread;

    void RxPing(const QByteArray& data);
    void RxMeasurePin(const QByteArray& data);
    void RxGetCalibrationCoefficients(const QByteArray& data);
    void RxSetCalibrationCoefficients(const QByteArray& data);
    void RxBufferOverflow(const QByteArray& data);
    void RxWrongCommand(const QByteArray& data);
    void RxCrcError(const QByteArray& data);
};

class TesterPort : public QSerialPort, private MyProtokol {
    Q_OBJECT

public:
    TesterPort(AmkTester* t);
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);
    AmkTester* m_t;
    typedef void (AmkTester::*func)(const QByteArray&);
    QVector<func> m_f;

private:
    void Read();
    QByteArray m_data;
    QByteArray m_tmpData;
    QMutex m_mutex;
};

#endif // MY_PROTOCOL_H
