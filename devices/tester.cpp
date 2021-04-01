#include "tester.h"
#include <QDebug>
#include <QTimerEvent>

const int id1 = qRegisterMetaType<QVector<quint16>>("QVector<quint16>");
const int id2 = qRegisterMetaType<Pins>("Pins");

Tester::Tester(QObject* parent)
    : QObject(parent)
    , m_port(new TesterPort(this))
    , parcels{
          parcel(MEASURE_PIN, static_cast<quint8>(0x0)),
          parcel(MEASURE_PIN, static_cast<quint8>(0x1)),
          parcel(MEASURE_PIN, static_cast<quint8>(0x2)),
          parcel(MEASURE_PIN, static_cast<quint8>(0x3)),
          parcel(MEASURE_PIN, static_cast<quint8>(0x4)),
          parcel(MEASURE_PIN, static_cast<quint8>(0x5)),
          parcel(MEASURE_PIN, static_cast<quint8>(0x6)),
          parcel(MEASURE_PIN, static_cast<quint8>(0x7)),
          parcel(MEASURE_PIN, static_cast<quint8>(0x8)),
          parcel(MEASURE_PIN, static_cast<quint8>(0x9)),
          parcel(MEASURE_PIN, static_cast<quint8>(0xA))} //
{
    m_port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, m_port, &QObject::deleteLater);
    m_portThread.start(QThread::NormalPriority);
}

Tester::~Tester() {
    stop();
    m_portThread.quit();
    m_portThread.wait();
}

bool Tester::ping(const QString& portName, int baud, int addr) {
    Q_UNUSED(baud)
    Q_UNUSED(addr)
    QMutexLocker locker(&m_mutex);
    m_connected = false;
    reset();
    do {
        emit close();
        if(!m_semaphore.tryAcquire(1, 1000))
            break;

        if(!portName.isEmpty())
            m_port->setPortName(portName);

        emit open(QIODevice::ReadWrite);
        if(!m_semaphore.tryAcquire(1, 1000))
            break;

        emit write(parcel(static_cast<quint8>(PING)));
        if(!m_semaphore.tryAcquire(1, 100)) {
            emit close();
            break;
        }

        m_connected = true;
    } while(0);
    return m_connected;
}

bool Tester::measurePin(int pin) {
    QMutexLocker locker(&m_mutex);
    if(!m_connected)
        return false;
    reset();
    emit write(parcel(MEASURE_PIN, static_cast<quint8>(pin)));
    if(m_semaphore.tryAcquire(1, 1000))
        return true;
    return {};
}

bool Tester::measureAll() {
    QMutexLocker locker(&m_mutex);
    m_semPv.acquire(m_semPv.available());
    for(auto& parcel : parcels) {
        reset();
        emit write(parcel);
        if(!m_semaphore.tryAcquire(1, 1000))
            return false;
    }
    return true;
}

bool Tester::getCalibrationCoefficients(float& /*GradCoeff*/, int /*pin*/) {
    QMutexLocker locker(&m_mutex);
    if(!m_connected)
        return false;
    return {};
}

bool Tester::setCalibrationCoefficients(float& /*GradCoeff*/, int /*pin*/) {
    QMutexLocker locker(&m_mutex);
    if(!m_connected)
        return false;
    return {};
}

Pins Tester::pinsValue() const { return m_pins; }

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
        int data[Pins::Count + 1];
        int operator[](int idx) const noexcept { return data[idx]; }
    };
    static auto setDataS = [this](const Data& value, const uint8_t row) noexcept {
        enum { NoiseThreshold = 0xF };
        constexpr float RRef = 1000.; // 1000 опорное сопротивление
        int resistance[Pins::Count]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        int count = 0;
        for(int column = row - 1, index = row + 1; column > -1 || index < (m_count ? m_count : Pins::Count); --column, ++index) {
            float value1 = value[row];
            if(column > -1 && value[column] > NoiseThreshold) {
                float value2 = value[column];
                ++count;
                resistance[column] = (value1 > value2) ? ((value1 - value2) / value2) * RRef
                                                       : ((value2 - value1) / value1) * RRef;
                if(resistance[row] < 0)
                    ++resistance[row];
            }
            if(index < Pins::Count && value[index] > NoiseThreshold) {
                float value2 = value[index];
                ++count;
                resistance[index] = (value1 > value2) ? ((value1 - value2) / value2) * RRef
                                                      : ((value2 - value1) / value1) * RRef;
                if(resistance[row] < 0)
                    ++resistance[row];
            }
        }

        for(int column = 0; column < Pins::Count; ++column) {
            if(count && resistance[column] > 0 && column < row)
                resistance[column] /= count;

            //            if(rez[column] <= 0) {
            m_pins[row][column] = resistance[column];
            //            } else if(rez[column] < 800) { //990
            //                m_pins[row][column] = 4E-16 * pow(rez[column], 6) - 2E-12 * pow(rez[column], 5) + 3E-09 * pow(rez[column], 4) - 3E-06 * pow(rez[column], 3) + 0.0017 * pow(rez[column], 2) + 0.5962 * rez[column] + 0.1349;
            //            } else /*if (rez[column] < 10000) { //1000
            //                    m_pins[row][column] = 8E-11 * pow(rez[column], 3) - 2E-06 * pow(rez[column], 2) + 1.0048 * rez[column] - 9.1202;
            //                } else*/
            //            { //10000
            //                //m_pins[row][column] = -4E-12 * pow(rez[column], 3) - 2E-08 * pow(rez[column], 2) + 0.9816 * rez[column] + 154.98;
            //                m_pins[row][column] = 1E-26 * pow(rez[column], 6) - 2E-21 * pow(rez[column], 5) - 9E-17 * pow(rez[column], 4) + 2E-11 * pow(rez[column], 3) - 1E-06 * pow(rez[column], 2) + 1.0111 * rez[column] - 25.673;
            //            }
        }
    };
    QElapsedTimer t;
    t.start();
    for(uint8_t i = 0; i < Pins::Count; ++i) {
        setDataS({
                     dataMatrix[i][0],
                     dataMatrix[i][1],
                     dataMatrix[i][2],
                     dataMatrix[i][3],
                     dataMatrix[i][4],
                     dataMatrix[i][5],
                     dataMatrix[i][6],
                     dataMatrix[i][7],
                     dataMatrix[i][8],
                     dataMatrix[i][9],
                     dataMatrix[i][10],
                 },
                 i);
    }
    //qDebug() << __FUNCTION__ << (t.nsecsElapsed() * 0.001) << "us";
}

void Tester::rxPing(const QByteArray& /*data*/) {
    qDebug() << "rxPing";
}

void Tester::rxMeasurePin(const QByteArray& data) {
    const MData* d = pValue<const MData>(data);
    memcpy(dataMatrix[d->ch], d, Pins::Count * 2);
    if(d->ch == Pins::Count - 1) {
        calcRes();
        emit measureReadyAll(m_pins);
    }
    m_semaphore.release();
}

void Tester::rxGetCalibrationCoefficients(const QByteArray& /*data*/) {
    qDebug() << "rxGetCalibrationCoefficients";
}

void Tester::rxSetCalibrationCoefficients(const QByteArray& /*data*/) {
    qDebug() << "rxSetCalibrationCoefficients";
}

void Tester::rxBufferOverflow(const QByteArray& data) {
    qDebug() << "rxBufferOverflow" << data.toHex().toUpper();
}

void Tester::rxWrongCommand(const QByteArray& data) {
    qDebug() << "rxWrongCommand" << data.toHex().toUpper();
}

void Tester::rxCrcError(const QByteArray& data) {
    qDebug() << "rxCrcError" << data.toHex().toUpper();
}

/////////////////////////////////////////
/// \brief SerialPort::SerialPort
/// \param AmkTest
///
TesterPort::TesterPort(Tester* t)
    : m_t(t)
    , m_f(QVector<TesterPort::func>(0x100, &Tester::rxWrongCommand)) {
    m_f[PING] = &Tester::rxPing;
    m_f[MEASURE_PIN] = &Tester::rxMeasurePin;
    m_f[GET_CALIBRATION_COEFFICIENTS] = &Tester::rxGetCalibrationCoefficients;
    m_f[SET_CALIBRATION_COEFFICIENTS] = &Tester::rxSetCalibrationCoefficients;
    m_f[BUFFER_OVERFLOW] = &Tester::rxBufferOverflow;
    m_f[WRONG_COMMAND] = &Tester::rxWrongCommand;
    m_f[CRC_ERROR] = &Tester::rxCrcError;

    setBaudRate(Baud115200);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
    setParity(NoParity);

    connect(t, &Tester::open, this, &TesterPort::openSlot);
    connect(t, &Tester::close, this, &TesterPort::closeSlot);
    connect(t, &Tester::write, this, &TesterPort::writeSlot);
    //    //get the virtual table pointer of object obj
    //    int* vptr = *(int**)AmkTest;
    //    // we shall call the function fn, but first the following assembly code
    //    //  is required to make obj as 'this' pointer as we shall call
    //    //  function fn() directly from the virtual table
    //    //__asm mov ecx, AmkTest;
    //    AmkTest;
    //    //function fn is the first entry of the virtual table, so it's vptr[0]
    //    ((void (*)(const QByteArray&))vptr[0])(QByteArray("0123456789"));

    //    typedef void (AmkTest::*func)(const QByteArray&);
    //    CallBack* ptr = t;
    //    func* vptr = *(func**)(ptr);
    //    (t->*vptr[0])(QByteArray("0123456789"));
    connect(this, &QSerialPort::readyRead, this, &TesterPort::readSlot, Qt::DirectConnection);
}

void TesterPort::openSlot(int mode) {
    QMutexLocker locker(&m_mutex);
    if(open(static_cast<OpenMode>(mode)))
        m_t->m_semaphore.release();
}

void TesterPort::closeSlot() {
    QMutexLocker locker(&m_mutex);
    close();
    m_t->m_semaphore.release();
}

void TesterPort::writeSlot(const QByteArray& data) {
    QMutexLocker locker(&m_mutex);
    counter += write(data);
    //qDebug() << counter;
}

void TesterPort::readSlot() {
    QMutexLocker locker(&m_mutex);
    m_answerData.append(readAll());
    for(int i = 0; i < m_answerData.size() - 3; ++i) {
        const Parcel* const d = reinterpret_cast<const Parcel*>(m_answerData.constData() + i);
        if(d->start == RX && d->len <= m_answerData.size()) {
            QByteArray tmpData = m_answerData.mid(i, d->len);
            counter += tmpData.size();
            if(checkParcel(tmpData))
                (m_t->*m_f[d->cmd])(tmpData);
            else
                (m_t->*m_f[CRC_ERROR])(tmpData);

            m_t->m_semaphore.release();
            m_answerData.remove(0, i + d->len);
            i = 0;
        }
        //        if (m_answerData.at(i) == -86 && m_answerData.at(i + 1) == 85) {
        //            if ((i + m_answerData[i + 2]) <= m_answerData.size()) {
        //                m_tmpData = m_answerData.mid(i, m_answerData[i + 2]);
        //                quint8 cmd = *(quint8*)(m_tmpData.constData() + 3);
        //                if (checkParcel(m_tmpData))
        //                    (m_t->*m_f[cmd])(m_tmpData);
        //                else
        //                    (m_t->*m_f[CRC_ERROR])(m_tmpData);
        //                m_answerData.remove(0, i + m_answerData[i + 2]);
        //                i = -1;
        //            }
        //        }
    }
}

void Tester::timerEvent(QTimerEvent* event) {
    static int ctr;
    if(event->timerId() == timerId) {
        measurePin(ctr++);
        if(ctr == Pins::Count)
            ctr = 0;
    }
}
