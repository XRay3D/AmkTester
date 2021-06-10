#include "header.h"

#include <QFile>
#include <QMouseEvent>
#include <QPainter>

Header::Header(Qt::Orientation orientation, std::vector<ModelData>& data, QWidget* parent)
    : QHeaderView(orientation, parent)
    , m_data {data} //
{
    connect(this, &QHeaderView::sectionCountChanged, [this](int /*oldCount*/, int newCount) {
        m_checkRect.resize(newCount);
        m_checked.resize(newCount);
        for(int index = 0; index < count(); ++index)
            m_checked[index] = m_data[index].use;
        emit onCheckedV(m_checked);
    });
    setSectionsClickable(true);
    setHighlightSections(true);
}

Header::~Header() { }

void Header::setAll(bool checked) {
    for(int index = 0; index < count(); ++index) {
        if(m_data[index].use != checked) {
            m_data[index].use = checked;
            emit onChecked(index);
            updateSection(index);
        }
        m_checked[index] = m_data[index].use;
    }
    emit onCheckedV(m_checked);
}

void Header::togle(int index) {
    m_data[index].use = !m_data[index].use;
    for(int index = 0; index < count(); ++index)
        m_checked[index] = m_data[index].use;
    updateSection(index);
    emit onCheckedV(m_checked);
    emit onChecked(index);
}

void Header::setSingle(int index) {
    for(int i = 0, fl = i == index; i < count(); fl = ++i == index) {
        if(m_data[i].use != fl) {
            m_data[i].use = fl;
            emit onChecked(i);
            updateSection(i);
        }
        m_checked[index] = m_data[index].use;
    }
    emit onCheckedV(m_checked);
}

QRect Header::getRect(const QRect& rect) {
    return QRect(
        rect.left() + XOffset,
        rect.top() + (rect.height() - DelegateSize) / 2,
        DelegateSize,
        DelegateSize);
}

void Header::mouseMoveEvent(QMouseEvent* event) {
    mousePos = event->pos();
    int indexAt = logicalIndexAt(mousePos);
    if(m_checkRect[indexAt].contains(mousePos))
        updateSection(indexAt);
    static int index;
    do {
        if(index == indexAt)
            break;
        index = logicalIndexAt(event->pos());
        if(index < 0)
            break;
        if(event->buttons() != Qt::RightButton)
            break;
        if(orientation() == Qt::Horizontal) {
            //setSingle(index);
        } else
            togle(index);
        event->accept();
        return;
    } while(0);
    QHeaderView::mouseMoveEvent(event);
}

void Header::mousePressEvent(QMouseEvent* event) {
    int index = logicalIndexAt(event->pos());
    do {
        if(index < 0)
            break;
        if(!m_checkRect[index].contains(event->pos()) && event->buttons() != Qt::RightButton)
            break;
        if(orientation() == Qt::Horizontal)
            setSingle(index);
        else
            togle(index);
        event->accept();
        return;
    } while(0);
    QHeaderView::mousePressEvent(event);
}

void Header::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const {
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    QStyleOptionButton option;
    m_checkRect[logicalIndex] = option.rect = getRect(rect);

    option.state = m_data[logicalIndex].use
        ? QStyle::State_On
        : QStyle::State_Off;

    option.state |= isEnabled()
        ? QStyle::State_Enabled
        : QStyle::State_None;

    option.state |= m_checkRect[logicalIndex].contains(mousePos)
        ? QStyle::State_MouseOver
        : QStyle::State_None;

    if(orientation() == Qt::Horizontal)
        style()->drawPrimitive(QStyle::PE_IndicatorRadioButton, &option, painter);
    else
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
}
