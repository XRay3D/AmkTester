#ifndef MY_PROTOCOL_H
#define MY_PROTOCOL_H

#include "myprotokol.h"
#include <QElapsedTimer>
#include <QMutex>
#include <QSemaphore>
#include <QSerialPort>
#include <QThread>
#include <QVector>
#include <commoninterfaces.h>

enum COMMAND {
    PING,
    MEASURE_PIN,
    GET_CALIBRATION_COEFFICIENTS,
    SET_CALIBRATION_COEFFICIENTS,
    BUFFER_OVERFLOW,
    WRONG_COMMAND,
    CRC_ERROR
};

struct PinsValue {
    PinsValue() { }
    int data[11][11];
};

class TesterPort;

class Tester : public QObject, private MyProtokol, /*private CallBack,*/ public CommonInterfaces {
    Q_OBJECT
    friend class TesterPort;

public:
    Tester(QObject* parent = nullptr);
    ~Tester() override;

    bool ping(const QString& portName = QString(), int baud = 9600, int addr = 0) override;

    bool measurePin(int pin);
    bool measureAll();
    bool setDefaultCalibrationCoefficients(quint8 pin);
    bool getCalibrationCoefficients(float& GradCoeff, int pin);
    bool setCalibrationCoefficients(float& GradCoeff, int pin);
    bool saveCalibrationCoefficients(quint8 pin);
    PinsValue pinsValue() const;

signals:
    void open(int mode) override;
    void close() override;
    void write(const QByteArray& data);
    void measureReady(const QVector<quint16>&);
    void measureReadyAll(const PinsValue&);

private:
    bool m_result = false;
    bool all = false;
    int m_counter = 0;
    TesterPort* m_port;
    QMutex m_mutex;
    QSemaphore m_semaphore;
    mutable QSemaphore m_semPv;
    QThread m_portThread;
    PinsValue m_pinsValue;

    void reset() override;

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
    TesterPort(Tester* t);
    void openSlot(int mode);
    void closeSlot();
    void writeSlot(const QByteArray& data);
    Tester* m_t;
    typedef void (Tester::*func)(const QByteArray&);
    QVector<func> m_f;

private:
    void readSlot();
    QByteArray m_answerData;
    QMutex m_mutex;
    qint64 counter = 0;
};

Q_DECLARE_METATYPE(QVector<quint16>)

#endif // MY_PROTOCOL_H
