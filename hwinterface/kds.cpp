#include "kds.h"

#include <QDebug>

Amk::Amk()
    : Elemer::AsciiDevice()

//    : m_port(new AmkPort(this))
{

    //    m_port->moveToThread(&m_portThread);
    //    connect(&m_portThread, &QThread::finished, m_port, &QObject::deleteLater);
    //    connect(this, &Amk::Open, m_port, &AmkPort::Open);
    //    connect(this, &Amk::Close, m_port, &AmkPort::Close);
    //    connect(this, &Amk::Write, m_port, &AmkPort::Write);
    //    m_portThread.start(QThread::InheritPriority);
}

//Amk::~Amk()
//{
//    //    m_portThread.quit();
//    //    m_portThread.wait();
//}

//bool Amk::ping(const QString& portName, int baud, int addr)
//{
//    Q_UNUSED(baud)
//    Q_UNUSED(addr)
//    QMutexLocker locker(&m_mutex);
//    m_connected = true;
//    m_semaphore.acquire(m_semaphore.available());
//    do {
//        emit Close();
//        if (!m_semaphore.tryAcquire(1, 1000))
//            break;

//        if (!portName.isEmpty())
//            m_port->setPortName(portName);

//        emit Open(QIODevice::ReadWrite);
//        if (!m_semaphore.tryAcquire(1, 1000))
//            break;

//        if (getDev(0) != 25) {
//            emit Close();
//            m_connected = false;
//        }
//    } while (0);
//    return m_connected;
//}

//int Amk::getDev(int addr)
//{
//    dev = 0;
//    if (m_connected) {
//        Write(createParcel({ addr, 0 }));
//        if (m_semaphore.tryAcquire(1, 1000))
//            dev = m_data[1].toInt();
//    }
//    return dev;
//}

bool Amk::setOut(int addr, int value)
{
    if (m_connected) {
        write(createParcel(addr, 4, value));
        if (wait() && success())
            return true;
    }
    return false;
}

//uint KDS::getUintData(QByteArray data)
//{
//    if (data.isEmpty())
//        return false;

//    int i = 0;

//    while (data[0] != '!' && data.size())
//        data = data.remove(0, 1);

//    while (data[data.size() - 1] != '\r' && data.size())
//        data = data.remove(data.size() - 1, 1);

//    data = data.remove(data.size() - 1, 1);

//    QList<QByteArray> list = data.split(';');
//    data.clear();

//    while (i < list.count() - 1)
//        data.append(list[i++]).append(';');

//    if (CalcCrc(data).toInt() == list.last().toInt() && list.count() > 2)
//        return list.at(1).toUInt();

//    return 0;
//}

//bool KDS::getSuccess(QByteArray data)
//{
//    if (data.isEmpty())
//        return false;

//    int i = 0;

//    while (data[0] != '!' && data.size())
//        data = data.remove(0, 1);

//    while (data[data.size() - 1] != '\r' && data.size())
//        data = data.remove(data.size() - 1, 1);

//    data = data.remove(data.size() - 1, 1);

//    QList<QByteArray> list = data.split(';');
//    data.clear();

//    while (i < list.count() - 1)
//        data.append(list[i++]).append(';');

//    if (CalcCrc(data).toInt() == list.last().toInt() && list.count() > 2)
//        if (list.at(1) == "$0")
//            return true;

//    return false;
//}

///////////////////////////////////////////
/// \brief Port::Port
/// \param t
///
//AmkPort::AmkPort(Amk* kds)
//    : m_kds(kds)
//{
//    setBaudRate(Baud9600);
//    setParity(NoParity);
//    setDataBits(Data8);
//    setFlowControl(NoFlowControl);
//    connect(this, &QSerialPort::readyRead, this, &AmkPort::Read, Qt::DirectConnection);
//}

//void AmkPort::Open(int mode)
//{
//    if (open(static_cast<OpenMode>(mode))) {
//        setDataTerminalReady(false);
//        setRequestToSend(true);
//        m_kds->m_semaphore.release();
//    }
//}

//void AmkPort::Close()
//{
//    close();
//    m_kds->m_semaphore.release();
//}

//void AmkPort::Write(const QByteArray& data)
//{
//    write(data);
//    //qDebug() << "Write" << data;
//}

//void AmkPort::Read()
//{
//    QMutexLocker locker(&m_mutex);
//    m_data.append(readAll());
//    qDebug() << Q_FUNC_INFO << m_data;
//    if (m_data[m_data.size() - 1] == '\r' && checkParcel(m_data, m_kds->m_data)) {
//        //qDebug() << "Read" << m_data;
//        m_data.clear();
//        m_kds->m_semaphore.release();
//    }
//}
