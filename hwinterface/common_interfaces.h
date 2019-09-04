#ifndef COMMON_INTERFACES_H
#define COMMON_INTERFACES_H

#include <QString>

class CommonInterfaces {
public:
    virtual ~CommonInterfaces() = default;
    virtual bool Ping(const QString& portName = QString(), int baud = 9600, int addr = 0) = 0;
    virtual bool IsConnected() const { return m_connected; }

protected:
    bool m_connected = false;
};

#endif // COMMON_INTERFACES_H
