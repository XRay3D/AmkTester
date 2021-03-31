#ifndef TABLE_H
#define TABLE_H

#include "amktest/pointedit.h"
#include "model.h"
#include <QTableView>

class Header;
class QAbstractButton;

class TableView : public QTableView {
    Q_OBJECT
public:
    explicit TableView(QWidget* parent = nullptr);
    ~TableView();
    Model* model() const;

    void clear();
    void addRow(const QString& name);
    void setPattern(const Pins& pData, const Point& pt1, const Point& pt2);

    void init();
    void initCheckBox();
    void initCheckBox2();
    void initRadioButton();
    void initCheckBoxRadioButton();

private:
    Model* m_model;
    void createCornerCheckBox(Header* header, Header* header2 = nullptr);
};

#endif // TABLE_H
