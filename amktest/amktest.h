#ifndef AMKTEST_H
#define AMKTEST_H

#include "pointedit.h"
#include <QJsonArray>
#include <QWidget>

class QComboBox;
class QLineEdit;
class QPushButton;

class AmkTest : public QWidget {
    Q_OBJECT

public:
    explicit AmkTest(QWidget* parent = nullptr);
    ~AmkTest();

    void cbxTypeIndexChanged(int index);
    void loadSettings();
    void saveSettings();
    void setupUi();
    void switchSlot();

    PointEdit::Point m_points[12];
    QComboBox* cbType;
    QVector<QLineEdit*> le;
    QVector<QPushButton*> pb;
    int m_lastPointType = 0;
    int m_numPoint = 0;
    int lastIndex = 0;
    QJsonArray jsonArray;

    void setFl(bool value);

signals:
    void message(const QString&, int = 500);

private:
    bool fl = false;

    enum { Json };
    const int saveFormat = Json;

protected:
    bool eventFilter(QObject* obj, QEvent* event);
};

#endif // AMKTEST_H
