#pragma once

#include <QJsonArray>
#include <qglobal.h>

struct Point;
class RelaySetHolder {
    QJsonArray jsonArray;

public:
    RelaySetHolder();
    void loadSets();
    void saveSets();

    QStringList sets();
    void initPoints(Point* points, int index);
};
