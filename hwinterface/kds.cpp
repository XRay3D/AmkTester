#include "kds.h"

#include <QDebug>

KDS::KDS()
    : m_port(new KdsPort(this))
{

    m_port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, m_port, &QObject::deleteLater);
    connect(this, &KDS::Open, m_port, &KdsPort::Open);
    connect(this, &KDS::Close, m_port, &KdsPort::Close);
    connect(this, &KDS::Write, m_port, &KdsPort::Write);
    m_portThread.start(QThread::InheritPriority);
}

bool KDS::Ping(const QString& portName, int baud, int addr)
{
    Q_UNUSED(baud)
    Q_UNUSED(addr)
    QMutexLocker locker(&m_mutex);
    m_connected = true;
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

        if (getDev(0) != 25) {
            emit Close();
            m_connected = false;
        }
    } while (0);
    return m_connected;
}

int KDS::getDev(int addr)
{
    dev = 0;
    if (m_connected) {
        QByteArray data = QString(":%1;0;").arg(addr).toLocal8Bit();
        data.append(CalcCrc(data)).append('\r');
        Write(data);
        if (m_semaphore.tryAcquire(1, 1000))
            dev = getUintData(m_data);
    }
    return dev;
}

bool KDS::setOut(int addr, int value)
{
    if (m_connected) {
        QByteArray data = QString(":%1;4;%2;").arg(addr).arg(value).toLocal8Bit();
        data.append(CalcCrc(data)).append('\r');
        Write(data);
        if (m_semaphore.tryAcquire(1, 1000)) {
            return getSuccess(m_data);
        }
    }
    return false;
}

uint KDS::getUintData(QByteArray data)
{
    if (data.isEmpty())
        return false;

    int i = 0;

    while (data[0] != '!' && data.size())
        data = data.remove(0, 1);

    while (data[data.size() - 1] != '\r' && data.size())
        data = data.remove(data.size() - 1, 1);

    data = data.remove(data.size() - 1, 1);

    QList<QByteArray> list = data.split(';');
    data.clear();

    while (i < list.count() - 1)
        data.append(list[i++]).append(';');

    if (CalcCrc(data).toInt() == list.last().toInt() && list.count() > 2)
        return list.at(1).toUInt();

    return 0;
}

bool KDS::getSuccess(QByteArray data)
{
    if (data.isEmpty())
        return false;

    int i = 0;

    while (data[0] != '!' && data.size())
        data = data.remove(0, 1);

    while (data[data.size() - 1] != '\r' && data.size())
        data = data.remove(data.size() - 1, 1);

    data = data.remove(data.size() - 1, 1);

    QList<QByteArray> list = data.split(';');
    data.clear();

    while (i < list.count() - 1)
        data.append(list[i++]).append(';');

    if (CalcCrc(data).toInt() == list.last().toInt() && list.count() > 2)
        if (list.at(1) == "$0")
            return true;

    return false;
}

QString KDS::CalcCrc(QByteArray& parcel)
{
    uint crc = 0xFFFF;
    for (int i = 1; i < parcel.size(); i++) {
        crc ^= parcel[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return QString().setNum(crc);
}
///////////////////////////////////////////
/// \brief Port::Port
/// \param t
///
KdsPort::KdsPort(KDS* kds)
    : m_kds(kds)
{
    setBaudRate(Baud9600);
    setParity(NoParity);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
    connect(this, &QSerialPort::readyRead, this, &KdsPort::Read, Qt::DirectConnection);
}

void KdsPort::Open(int mode)
{
    if (open(static_cast<OpenMode>(mode)))
        m_kds->m_semaphore.release();
}

void KdsPort::Close()
{
    close();
    m_kds->m_semaphore.release();
}

void KdsPort::Write(const QByteArray& data) { write(data); }

void KdsPort::Read()
{
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    if (m_data[m_data.size() - 1] == '\r') {
        m_kds->m_data = m_data;
        m_data.clear();
        m_kds->m_semaphore.release();
    }
}
