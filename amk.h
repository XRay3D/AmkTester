#ifndef AMK_H
#define AMK_H

#include "pointedit.h"

#include <QComboBox>
#include <QFormLayout>

class AMK : public QWidget {
    Q_OBJECT

public:
    explicit AMK(QWidget* parent = 0);
    ~AMK();

    void CbTypeCurrentIndexChanged(int index);
    void loadSettings();
    void saveSettings();
    void setupUi(QWidget* Form); // setupUi
    void SwitchSlot(int pointNum);

    PointEdit::Point m_points[12];
    QComboBox* cbType;
    QVector<QLineEdit*> le;
    QVector<QPushButton*> pb;
    int m_lastPointType = 0;
    int m_numPoint = 0;

protected:
    bool eventFilter(QObject* obj, QEvent* event);
};

#endif // AMK_H
