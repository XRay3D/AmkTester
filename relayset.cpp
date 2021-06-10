#include "relayset.h"
#include "devices/devices.h"
#include "pointedit.h"
#include "relaysetholder.h"
#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QShortcut>

#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip.hpp>
namespace rv = ranges::views;

RelaySet::RelaySet(QWidget* parent)
    : QWidget(parent)
    , leDescription(12)
    , buttons(12) //
{
}

void RelaySet::setupUi(int fl) {
    auto layAmk1 = new QFormLayout(this);
    layAmk1->setMargin(0);
    auto pbSetter = [this](QPushButton* pushButton, int i, const QString objName, bool fl) {
        auto shortcut = new QShortcut(this); // Инициализируем объект
        shortcut->setKey(fl ? QString("Shift+F%1").arg(i + 1) : QString("F%1").arg(i + 1)); // Устанавливаем код клавиши
        connect(shortcut, &QShortcut::activated, pushButton, &QPushButton::click); // цепляем обработчик нажатия клавиши
        pushButton->setCheckable(true);
        pushButton->setMinimumSize(70, 0);
        pushButton->setObjectName(QString(objName).arg(i));
        pushButton->setText(fl ? QString("Shift+F%1").arg(i + 1) : QString("F%1").arg(i + 1));
        connect(pushButton, &QPushButton::clicked, this, &RelaySet::switchSlot);
    };

    for(int i = 0; i < SetCount; ++i) {
        layAmk1->addRow(buttons[i] = new QPushButton(this), leDescription[i] = new QLineEdit(this));
        leDescription[i]->installEventFilter(this);
        pbSetter(buttons[i], i, "pushButton_%1_0", fl);
    }
}

const Point& RelaySet::currentPoint() const {
    for(auto b : buttons) {
        if(b->isChecked())
            return m_points[buttons.indexOf(b)];
    }
    static Point pt {};
    return pt;
}

bool RelaySet::eventFilter(QObject* obj, QEvent* event) {
    if(event->type() == QEvent::MouseButtonDblClick) {
        int m_numPoint = leDescription.indexOf(reinterpret_cast<QLineEdit*>(obj));
        if(m_numPoint > -1) {
            setsChanged = PointEdit(m_points[m_numPoint % SetCount], leDescription[m_numPoint], this).exec();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void RelaySet::setSets(RelaySetHolder* newSets) {
    sets = newSets;
    cbxTypeIndexChanged(0);
}

void RelaySet::switchSlot() {
    QPushButton* pushButton = qobject_cast<QPushButton*>(sender());
    int m_numPoint = buttons.indexOf(pushButton);
    qDebug() << "m_numPoint" << m_numPoint;
    for(int i {}; i < SetCount; ++i)
        if(m_numPoint != i)
            buttons[i]->setChecked(false);
        else
            pushButton->setChecked(true);
    if(Devices::kds1()->setRelay(m_points[m_numPoint % SetCount].Parcel)) {
        emit showMessage("Прибором КДС 1 успешно переключен!");
        return;
    }

    emit showMessage("Нет связи с прибором КДС!");
}

void RelaySet::cbxTypeIndexChanged(int index) {
    qDebug() << __FUNCTION__ << index;
    if(index < 0)
        return;
    sets->initPoints(m_points, index);
    for(const auto& [le, value] : rv::zip(leDescription, m_points))
        le->setText(value.Description);
}
