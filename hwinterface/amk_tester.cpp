#include "amk_tester.h"
#include <QDebug>

int id1 = qRegisterMetaType<QVector<quint16>>("QVector<quint16>");

AmkTester::AmkTester(QObject* parent)
    : QObject(parent)
    //    , CallBack(&m_semaphore)
    , m_port(new TesterPort(this))
{

    m_port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, m_port, &QObject::deleteLater);
    connect(this, &AmkTester::Open, m_port, &TesterPort::Open);
    connect(this, &AmkTester::Close, m_port, &TesterPort::Close);
    connect(this, &AmkTester::Write, m_port, &TesterPort::Write);
    m_portThread.start(QThread::InheritPriority);
}

AmkTester::~AmkTester()
{
    m_portThread.quit();
    m_portThread.wait();
}

bool AmkTester::Ping(const QString& portName, int baud, int addr)
{
    Q_UNUSED(baud)
    Q_UNUSED(addr)
    QMutexLocker locker(&m_mutex);
    m_connected = false;
    m_semaphore.acquire(m_semaphore.available());
    do {
        emit Close();
        if (!m_semaphore.tryAcquire(1, 1000))
            break;

        if (!portName.isEmpty())
            m_port->setPortName(portName);

        emit Open(QIODevice::ReadWrite);
        if (!m_semaphore.tryAcquire(1, 1000))
            break;

        emit Write(parcel(static_cast<quint8>(PING)));
        if (!m_semaphore.tryAcquire(1, 1000)) {
            emit Close();
            break;
        }
        m_connected = true;
    } while (0);
    return m_connected;
}

bool AmkTester::MeasurePin(int pin)
{
    QMutexLocker locker(&m_mutex);
    if (!m_connected)
        return false;
    m_result = false;
    m_semaphore.acquire(m_semaphore.available());
    emit Write(parcel(MEASURE_PIN, static_cast<quint8>(pin)));
    if (m_semaphore.tryAcquire(1, 1000))
        m_result = true;
    return m_result;
}

bool AmkTester::GetCalibrationCoefficients(float& GradCoeff, int pin)
{
    Q_UNUSED(GradCoeff)
    Q_UNUSED(pin)
    m_result = false;
    return m_result;
}

bool AmkTester::SetCalibrationCoefficients(float& GradCoeff, int pin)
{
    Q_UNUSED(GradCoeff)
    Q_UNUSED(pin)
    m_result = false;
    return m_result;
}

void AmkTester::RxPing(const QByteArray& data)
{
    Q_UNUSED(data)
    m_semaphore.release();
}

void AmkTester::RxMeasurePin(const QByteArray& data)
{
    const quint16* p = reinterpret_cast<const quint16*>(data.constData() + 4);
    emit SetValue(QVector<quint16>({ p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11] }));
    m_semaphore.release();
}

void AmkTester::RxGetCalibrationCoefficients(const QByteArray& data)
{
    Q_UNUSED(data)
    m_semaphore.release();
}

void AmkTester::RxSetCalibrationCoefficients(const QByteArray& data)
{
    Q_UNUSED(data)
    m_semaphore.release();
}

void AmkTester::RxBufferOverflow(const QByteArray& data)
{
    Q_UNUSED(data)
    qDebug() << "RxBufferOverflow" << data.toHex().toUpper();
    m_semaphore.release();
}

void AmkTester::RxWrongCommand(const QByteArray& data)
{
    Q_UNUSED(data)
    qDebug() << "RxWrongCommand" << data.toHex().toUpper();
    m_semaphore.release();
}

void AmkTester::RxCrcError(const QByteArray& data)
{
    Q_UNUSED(data)
    qDebug() << "RxCrcError" << data.toHex().toUpper();
    m_semaphore.release();
}

/////////////////////////////////////////
/// \brief SerialPort::SerialPort
/// \param amkTester
///
TesterPort::TesterPort(AmkTester* t)
    : m_t(t)
    , m_f(QVector<TesterPort::func>(0x100, &AmkTester::RxWrongCommand))
{
    m_f[PING] = &AmkTester::RxPing;
    m_f[MEASURE_PIN] = &AmkTester::RxMeasurePin;
    m_f[GET_CALIBRATION_COEFFICIENTS] = &AmkTester::RxGetCalibrationCoefficients;
    m_f[SET_CALIBRATION_COEFFICIENTS] = &AmkTester::RxSetCalibrationCoefficients;
    m_f[BUFFER_OVERFLOW] = &AmkTester::RxBufferOverflow;
    m_f[WRONG_COMMAND] = &AmkTester::RxWrongCommand;
    m_f[CRC_ERROR] = &AmkTester::RxCrcError;

    setBaudRate(Baud115200);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
    setParity(NoParity);

    //    //get the virtual table pointer of object obj
    //    int* vptr = *(int**)amkTester;
    //    // we shall call the function fn, but first the following assembly code
    //    //  is required to make obj as 'this' pointer as we shall call
    //    //  function fn() directly from the virtual table
    //    //__asm mov ecx, amkTester;
    //    amkTester;
    //    //function fn is the first entry of the virtual table, so it's vptr[0]
    //    ((void (*)(const QByteArray&))vptr[0])(QByteArray("0123456789"));

    //    typedef void (AmkTester::*func)(const QByteArray&);
    //    CallBack* ptr = t;
    //    func* vptr = *(func**)(ptr);
    //    (t->*vptr[0])(QByteArray("0123456789"));
    connect(this, &QSerialPort::readyRead, this, &TesterPort::Read, Qt::DirectConnection);
}

void TesterPort::Open(int mode)
{
    if (open(static_cast<OpenMode>(mode)))
        m_t->m_semaphore.release();
}

void TesterPort::Close()
{
    close();
    m_t->m_semaphore.release();
}

void TesterPort::Write(const QByteArray& data) { write(data); }

void TesterPort::Read()
{
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    for (int i = 0; i < m_data.size() - 3; ++i) {
        if (m_data.at(i) == -86 && m_data.at(i + 1) == 85) {
            if ((i + m_data[i + 2]) <= m_data.size()) {
                m_tmpData = m_data.mid(i, m_data[i + 2]);
                quint8 cmd = *(quint8*)(m_tmpData.constData() + 3);
                if (checkParcel(m_tmpData))
                    (m_t->*m_f[cmd])(m_tmpData);
                else
                    (m_t->*m_f[CRC_ERROR])(m_tmpData);
                m_data.remove(0, i + m_data[i + 2]);
                i = -1;
            }
        }
    }
}
