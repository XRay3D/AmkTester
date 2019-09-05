#ifndef AUTOTEST_H
#define AUTOTEST_H

#include <QObject>
#include <QSemaphore>

class Model;

class AutoTest : public QObject {
    Q_OBJECT
public:
    AutoTest(QObject* parent = nullptr);
    virtual ~AutoTest();
    void start(Model* model);
    void stop();
    QSemaphore sem;
signals:
    void finished();
    void message(const QString&);

private:
    Model* m_model = nullptr;
};

#endif // AUTOTEST_H
