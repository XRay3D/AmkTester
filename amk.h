#ifndef AMK_H
#define AMK_H

#include "pointedit.h"

#include <QComboBox>
#include <QFormLayout>
#include <QJsonArray>

class AMK : public QWidget {
    Q_OBJECT

public:
    explicit AMK(QWidget* parent = nullptr);
    ~AMK();

    void CbTypeIndexChanged(int index);
    void loadSettings();
    void saveSettings();
    void setupUi();
    void SwitchSlot();

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

#endif // AMK_H
