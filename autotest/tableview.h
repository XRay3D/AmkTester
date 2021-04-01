#pragma once

#include <QTableView>

class AutoTestModel;
class Header;

class TableView : public QTableView {
    Q_OBJECT
public:
    explicit TableView(QWidget* parent = nullptr);
    ~TableView();

    AutoTestModel* initCheckBox();

private:
    void createCornerCheckBox(Header* header);
};
