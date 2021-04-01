#ifndef MY_PROTOCOL_H
#define MY_PROTOCOL_H

#include "myprotokol.h"
#include "pins.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QIcon>
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

#pragma pack(push, 1)
struct MData {
    uint16_t data[Pins::Count];
    uint16_t ch;
};
#pragma pack(pop)

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
    Pins pinsValue() const;

    void start();
    void stop();
    void startStop(bool fl);

signals:
    void open(int mode) override;
    void close() override;
    void write(const QByteArray& data);
    void measureReady(const QVector<quint16>&);
    void measureReadyAll(const Pins&);

private:
    Pins m_pins;
    QMutex m_mutex;
    QSemaphore m_semaphore;
    QThread m_portThread;
    TesterPort* m_port;
    uint16_t dataMatrix[Pins::Count][Pins::Count]{};
    int m_counter = 0;
    mutable QSemaphore m_semPv;
    uint8_t m_count;
    int timerId = 0;
    const QByteArray parcels[Pins::Count];

    void reset() override;
    void calcRes();

    void rxPing(const QByteArray& data);
    void rxMeasurePin(const QByteArray& data);
    void rxGetCalibrationCoefficients(const QByteArray& data);
    void rxSetCalibrationCoefficients(const QByteArray& data);
    void rxBufferOverflow(const QByteArray& data);
    void rxWrongCommand(const QByteArray& data);
    void rxCrcError(const QByteArray& data);

protected:
    void timerEvent(QTimerEvent* event) override;
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
