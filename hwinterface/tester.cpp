#include "tester.h"
#include <QDebug>

const int id1 = qRegisterMetaType<QVector<quint16>>("QVector<quint16>");
const int id2 = qRegisterMetaType<PinsValue>("PinsValue");

Tester::Tester(QObject* parent)
    : QObject(parent)
    , m_port(new TesterPort(this))
{
    m_port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, m_port, &QObject::deleteLater);
    m_portThread.start(QThread::InheritPriority);
}

Tester::~Tester()
{
    m_portThread.quit();
    m_portThread.wait();
}

bool Tester::Ping(const QString& portName, int baud, int addr)
{
    Q_UNUSED(baud)
    Q_UNUSED(addr)
    QMutexLocker locker(&m_mutex);
    m_connected = false;
    reset();
    do {
        emit close();
        if (!m_semaphore.tryAcquire(1, 1000))
            break;

        if (!portName.isEmpty())
            m_port->setPortName(portName);

        emit open(QIODevice::ReadWrite);
        if (!m_semaphore.tryAcquire(1, 1000))
            break;

        emit write(parcel(static_cast<quint8>(PING)));
        if (!m_semaphore.tryAcquire(1, 100)) {
            emit close();
            break;
        }

        m_connected = true;
    } while (0);
    return m_connected;
}

bool Tester::measurePin(int pin)
{
    QMutexLocker locker(&m_mutex);
    all = false;
    if (!m_connected)
        return false;
    reset();
    emit write(parcel(MEASURE_PIN, static_cast<quint8>(pin)));
    if (m_semaphore.tryAcquire(1, 1000))
        m_result = true;
    return m_result;
}

bool Tester::measureAll()
{
    QMutexLocker locker(&m_mutex);
    m_semPv.acquire(m_semPv.available());
    all = true;
    bool ok = true;
    for (int pin = 0; pin < 11 && ok; ++pin) {
        reset();
        emit write(parcel(MEASURE_PIN, static_cast<quint8>(pin)));
        ok &= m_semaphore.tryAcquire(1, 1000);
        if (!ok)
            return (all = false);
    }
    return ok;
}

bool Tester::getCalibrationCoefficients(float& /*GradCoeff*/, int /*pin*/)
{
    QMutexLocker locker(&m_mutex);
    if (!m_connected)
        return false;
    return (m_result = false);
}

bool Tester::setCalibrationCoefficients(float& /*GradCoeff*/, int /*pin*/)
{
    QMutexLocker locker(&m_mutex);
    if (!m_connected)
        return false;
    return (m_result = false);
}

PinsValue Tester::pinsValue() const
{
    m_semPv.tryAcquire(1, 10000);
    return m_pinsValue;
}

void Tester::reset()
{
    m_result = false;
    m_semaphore.acquire(m_semaphore.available());
}

void Tester::rxPing(const QByteArray& /*data*/)
{
    qDebug() << "rxPing";
}

void Tester::rxMeasurePin(const QByteArray& data)
{
    const Parcel_t* d = reinterpret_cast<const Parcel_t*>(data.data());
    const quint16* p = reinterpret_cast<const quint16*>(d->data);
    if (all) {
        for (int i = 0; i < 11; ++i)
            m_pinsValue.data[p[11]][i] = p[i];
        if (p[11] == 10) {
            emit measureReadyAll(m_pinsValue);
            m_semPv.release();
        }
    } else
        emit measureReady({ p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11] });
}

void Tester::rxGetCalibrationCoefficients(const QByteArray& /*data*/)
{
    qDebug() << "rxGetCalibrationCoefficients";
}

void Tester::rxSetCalibrationCoefficients(const QByteArray& /*data*/)
{
    qDebug() << "rxSetCalibrationCoefficients";
}

void Tester::rxBufferOverflow(const QByteArray& data)
{
    qDebug() << "rxBufferOverflow" << data.toHex().toUpper();
}

void Tester::rxWrongCommand(const QByteArray& data)
{
    qDebug() << "rxWrongCommand" << data.toHex().toUpper();
}

void Tester::rxCrcError(const QByteArray& data)
{
    qDebug() << "rxCrcError" << data.toHex().toUpper();
}

/////////////////////////////////////////
/// \brief SerialPort::SerialPort
/// \param AmkTest
///
TesterPort::TesterPort(Tester* t)
    : m_t(t)
    , m_f(QVector<TesterPort::func>(0x100, &Tester::rxWrongCommand))
{
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

void TesterPort::openSlot(int mode)
{
    QMutexLocker locker(&m_mutex);
    if (open(static_cast<OpenMode>(mode)))
        m_t->m_semaphore.release();
}

void TesterPort::closeSlot()
{
    QMutexLocker locker(&m_mutex);
    close();
    m_t->m_semaphore.release();
}

void TesterPort::writeSlot(const QByteArray& data)
{
    QMutexLocker locker(&m_mutex);
    counter += write(data);
    //qDebug() << counter;
}

void TesterPort::readSlot()
{
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    for (int i = 0; i < m_data.size() - 3; ++i) {
        const Parcel_t* const d = reinterpret_cast<const Parcel_t*>(m_data.constData() + i);
        if (d->start == RX && d->len <= m_data.size()) {
            QByteArray tmpData = m_data.mid(i, d->len);
            counter += tmpData.size();
            if (checkParcel(tmpData))
                (m_t->*m_f[d->cmd])(tmpData);
            else
                (m_t->*m_f[CRC_ERROR])(tmpData);

            m_t->m_semaphore.release();
            m_data.remove(0, i + d->len);
            i = 0;
        }
        //        if (m_data.at(i) == -86 && m_data.at(i + 1) == 85) {
        //            if ((i + m_data[i + 2]) <= m_data.size()) {
        //                m_tmpData = m_data.mid(i, m_data[i + 2]);
        //                quint8 cmd = *(quint8*)(m_tmpData.constData() + 3);
        //                if (checkParcel(m_tmpData))
        //                    (m_t->*m_f[cmd])(m_tmpData);
        //                else
        //                    (m_t->*m_f[CRC_ERROR])(m_tmpData);
        //                m_data.remove(0, i + m_data[i + 2]);
        //                i = -1;
        //            }
        //        }
    }
}
