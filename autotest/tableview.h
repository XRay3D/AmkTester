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

    void addRow(const QString& name);
    void setPattern(int** pData, const PointEdit::Point& pt1, const PointEdit::Point& pt2);

    void init();
    void initCheckBox();
    void initRadioButton();
    void initCheckBoxRadioButton();

private:
    Model* m_model;
    void createCornerCheckBox(Header* header);
};

#endif // TABLE_H
