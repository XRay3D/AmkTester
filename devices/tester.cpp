#include "tester.h"
#include <QDebug>
#include <QTimerEvent>

using namespace XrProtokol;

const int id1 = qRegisterMetaType<ResistanceMatrix>("ResistanceMatrix");

Tester::Tester(QObject* parent)
    : Device(parent) //
{
    registerCallback<MeasurePin>(&Tester::ioRxMeasurePin);
}

Tester::~Tester() { stop(); }

Type Tester::type() const { return XrProtokol::Type::AMK_Tester; }

bool Tester::measurePin(int pin) {
    QMutexLocker locker(&m_mutex);
    if(!m_connected)
        return false;
    reset();
    emit write(parcel(MeasurePin, static_cast<quint8>(pin)));
    if(m_semaphore.tryAcquire(1, 1000))
        return true;
    return {};
}

bool Tester::measureAll() {
    QMutexLocker locker(&m_mutex);
    for(auto& parcel : parcels) {
        reset();
        emit write(parcel);
        if(!m_semaphore.tryAcquire(1, 1000))
            return false;
    }
    return true;
}

bool Tester::setDefaultCalibrationCoefficients() { return {}; }

bool Tester::getCalibrationCoefficients(CalibCoeff& /*GradCoeff*/) { return {}; }

bool Tester::setCalibrationCoefficients(const CalibCoeff& /*GradCoeff*/) { return {}; }

ResistanceMatrix Tester::resistance() const { return m_resistance; }

void Tester::start() {
    if(!timerId)
        timerId = startTimer(10);
}

void Tester::stop() {
    if(timerId)
        killTimer(timerId);
    timerId = 0;
}

void Tester::startStop(bool fl) { fl ? start() : stop(); }

void Tester::reset() { m_semaphore.acquire(m_semaphore.available()); }

void Tester::calcRes() {
    struct Data {
        int data[ResistanceMatrix::Size + 1];
        int operator[](int idx) const noexcept { return data[idx]; }
    };
    static auto setDataS = [this](const Data& value, const uint8_t row) noexcept {
        enum { NoiseThreshold = 0xF };
        constexpr float RRef = 1000.; // 1000 опорное сопротивление
        int resistance[ResistanceMatrix::Size]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        int count = 0;
        for(int column = row - 1, index = row + 1; column > -1 || index < (m_count ? m_count : ResistanceMatrix::Size); --column, ++index) {
            float value1 = value[row];
            if(column > -1 && value[column] > NoiseThreshold) {
                float value2 = value[column];
                ++count;
                resistance[column] = (value1 > value2) ? ((value1 - value2) / value2) * RRef
                                                       : ((value2 - value1) / value1) * RRef;
                if(resistance[row] < 0)
                    ++resistance[row];
            }
            if(index < ResistanceMatrix::Size && value[index] > NoiseThreshold) {
                float value2 = value[index];
                ++count;
                resistance[index] = (value1 > value2) ? ((value1 - value2) / value2) * RRef
                                                      : ((value2 - value1) / value1) * RRef;
                if(resistance[row] < 0)
                    ++resistance[row];
            }
        }

        for(int column = 0; column < ResistanceMatrix::Size; ++column) {
            if(count && resistance[column] > 0 && column < row)
                resistance[column] /= count;

            //            if(resistance[column] <= 0)
            m_resistance[row][column] = resistance[column];
            //            else //990
            //                m_resistance[row][column]
            //                    = 7E-22 * pow(resistance[column], 6)
            //                    - 2E-17 * pow(resistance[column], 5)
            //                    + 3E-13 * pow(resistance[column], 4)
            //                    - 2E-09 * pow(resistance[column], 3)
            //                    + 4E-06 * pow(resistance[column], 2)
            //                    + 1.0005 * resistance[column];
        }
    };
    QElapsedTimer t;
    t.start();
    for(uint8_t i = 0; i < ResistanceMatrix::Size; ++i) {
        setDataS({
                     accumulator[i][0],
                     accumulator[i][1],
                     accumulator[i][2],
                     accumulator[i][3],
                     accumulator[i][4],
                     accumulator[i][5],
                     accumulator[i][6],
                     accumulator[i][7],
                     accumulator[i][8],
                     accumulator[i][9],
                     accumulator[i][10],
                 },
                 i);
    }
    //qDebug() << __FUNCTION__ << (t.nsecsElapsed() * 0.001) << "us";
}

void Tester::ioRxMeasurePin(const QByteArray& data) {
    const MData* d = pValue<const MData>(data);
    memcpy(accumulator[d->ch], d, ResistanceMatrix::Size * 2);
    if(d->ch == ResistanceMatrix::Size - 1) {
        calcRes();
        emit resistanceReady(m_resistance);
    }
    m_semaphore.release();
}

void Tester::ioRxGetCalibrationCoefficients(const QByteArray& /*data*/) {
    qDebug() << __FUNCTION__;
}

void Tester::ioRxSetCalibrationCoefficients(const QByteArray& /*data*/) {
    qDebug() << __FUNCTION__;
}

void Tester::timerEvent(QTimerEvent* event) {
    static int ctr;
    if(event->timerId() == timerId) {
        measurePin(ctr++);
        if(ctr == ResistanceMatrix::Size)
            ctr = 0;
    }
}
