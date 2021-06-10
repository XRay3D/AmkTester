#pragma once

#include "resistancematrix.h"
#include "xrdevice.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QIcon>
#include <QMutex>
#include <QSemaphore>
#include <QSerialPort>
#include <QThread>
#include <QVector>
#include <commoninterfaces.h>

enum TesterCmd {
    MeasurePin = 1,
    GET_CALIBRATION_COEFFICIENTS,
    SET_CALIBRATION_COEFFICIENTS,
};

#pragma pack(push, 1)
struct MData {
    uint16_t data[ResistanceMatrix::Size];
    uint16_t ch;
};
struct CalibCoeff {
    float cc[7] {1.0f};
};
#pragma pack(pop)

class Tester final : public XrProtokol::Device {
    Q_OBJECT
    static inline const QByteArray parcels[ResistanceMatrix::Size] {
        parcel(MeasurePin, static_cast<quint8>(0x0)),
        parcel(MeasurePin, static_cast<quint8>(0x1)),
        parcel(MeasurePin, static_cast<quint8>(0x2)),
        parcel(MeasurePin, static_cast<quint8>(0x3)),
        parcel(MeasurePin, static_cast<quint8>(0x4)),
        parcel(MeasurePin, static_cast<quint8>(0x5)),
        parcel(MeasurePin, static_cast<quint8>(0x6)),
        parcel(MeasurePin, static_cast<quint8>(0x7)),
        parcel(MeasurePin, static_cast<quint8>(0x8)),
        parcel(MeasurePin, static_cast<quint8>(0x9)),
        parcel(MeasurePin, static_cast<quint8>(0xA))};

signals:
    void resistanceReady(const ResistanceMatrix&);

public:
    Tester(QObject* parent = nullptr);
    ~Tester() override;
    XrProtokol::Type type() const override;

    bool measurePin(int pin);
    bool measureAll();
    bool setDefaultCalibrationCoefficients();
    bool getCalibrationCoefficients(CalibCoeff& GradCoeff);
    bool setCalibrationCoefficients(const CalibCoeff& GradCoeff);
    ResistanceMatrix resistance() const;

    void start();
    void stop();
    void startStop(bool fl);

private:
    QMutex m_mutex;
    ResistanceMatrix m_resistance;

    int timerId = 0;
    uint16_t accumulator[ResistanceMatrix::Size][ResistanceMatrix::Size] {};
    uint8_t m_count;

    void reset() override;
    void calcRes();

    void ioRxMeasurePin(const QByteArray& data);
    void ioRxGetCalibrationCoefficients(const QByteArray& data);
    void ioRxSetCalibrationCoefficients(const QByteArray& data);

protected:
    void timerEvent(QTimerEvent* event) override;
};
