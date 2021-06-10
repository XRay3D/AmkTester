#include "relaysetholder.h"
#include "pointedit.h"

#include <QDebug>
#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip.hpp>
namespace rv = ranges::views;

RelaySetHolder::RelaySetHolder() {
}

void RelaySetHolder::loadSets() {
    QFile file(QStringLiteral("data.json"));
    while(!file.open(QIODevice::ReadOnly)) {
        QFile saveFile(QStringLiteral("data.json"));
        if(!saveFile.open(QIODevice::WriteOnly)) {
            qWarning() << __FUNCTION__ << saveFile.errorString();
            return;
        }
        saveFile.write("[{\"data\":["
                       "{\"name\":\"0Ом\",\"value\":128},"
                       "{\"name\":\"40Ом\",\"value\":256},"
                       "{\"name\":\"80Ом\",\"value\":16},"
                       "{\"name\":\"160Ом\",\"value\":32},"
                       "{\"name\":\"320Ом\",\"value\":64},"
                       "{\"name\":\"+10В\",\"value\":512},"
                       "{\"name\":\"-10В\",\"value\":1536},"
                       "{\"name\":\"+IРМТ\",\"value\":2},"
                       "{\"name\":\"-IРМТ\",\"value\":1026},"
                       "{\"name\":\"+UРМТ\",\"value\":1},"
                       "{\"name\":\"-UРМТ\",\"value\":1025},"
                       "{\"name\":\"0\",\"value\":0}],\"name\":\"КДС\"},"
                       "{\"data\":["
                       "{\"name\":\"20мА-0Ом\",\"value\":640},"
                       "{\"name\":\"20мА-400Ом\",\"value\":768},"
                       "{\"name\":\"20мА-500Ом\",\"value\":528},"
                       "{\"name\":\"20мА-1кОм\",\"value\":544},"
                       "{\"name\":\"20мА-2кОм\",\"value\":576},"
                       "{\"name\":\"5мА-0Ом\",\"value\":132},"
                       "{\"name\":\"5мА-400Ом\",\"value\":260},"
                       "{\"name\":\"5мА-500Ом\",\"value\":20},"
                       "{\"name\":\"5мА-1кОм\",\"value\":36},"
                       "{\"name\":\"5мА-2кОм\",\"value\":68},"
                       "{\"name\":\"0\",\"value\":0},"
                       "{\"name\":\"0\",\"value\":0}],\"name\":\"MN\"},"
                       "{\"data\":["
                       "{\"name\":\"1\",\"value\":1},"
                       "{\"name\":\"2\",\"value\":2},"
                       "{\"name\":\"3\",\"value\":4},"
                       "{\"name\":\"4\",\"value\":8},"
                       "{\"name\":\"5\",\"value\":16},"
                       "{\"name\":\"6\",\"value\":32},"
                       "{\"name\":\"7\",\"value\":64},"
                       "{\"name\":\"8\",\"value\":128},"
                       "{\"name\":\"9\",\"value\":256},"
                       "{\"name\":\"10\",\"value\":512},"
                       "{\"name\":\"11\",\"value\":1024},"
                       "{\"name\":\"12\",\"value\":2048}],\"name\":\"HC\"},"
                       "{\"data\":["
                       "{\"name\":\"+IТМ\",\"value\":8},"
                       "{\"name\":\"-IТМ\",\"value\":1032},"
                       "{\"name\":\"+UТМ\",\"value\":4},"
                       "{\"name\":\"-UТМ\",\"value\":1028},"
                       "{\"name\":\"+10В\",\"value\":512},"
                       "{\"name\":\"-10В\",\"value\":1536},"
                       "{\"name\":\"+IРМТ\",\"value\":2},"
                       "{\"name\":\"-IРМТ\",\"value\":1026},"
                       "{\"name\":\"+UРМТ\",\"value\":1},"
                       "{\"name\":\"-UРМТ\",\"value\":1025},"
                       "{\"name\":\"0\",\"value\":0},"
                       "{\"name\":\"0\",\"value\":0}],\"name\":\"КДС старый\"},"
                       "{\"data\":["
                       "{\"name\":\"1\",\"value\":1},"
                       "{\"name\":\"2\",\"value\":2},"
                       "{\"name\":\"4\",\"value\":4},"
                       "{\"name\":\"8\",\"value\":8},"
                       "{\"name\":\"16\",\"value\":16},"
                       "{\"name\":\"32\",\"value\":32},"
                       "{\"name\":\"64\",\"value\":64},"
                       "{\"name\":\"128\",\"value\":128},"
                       "{\"name\":\"256\",\"value\":256},"
                       "{\"name\":\"512\",\"value\":512},"
                       "{\"name\":\"1024\",\"value\":1024},"
                       "{\"name\":\"0\",\"value\":0}],\"name\":\"TEST\"}]");
        qWarning() << __FUNCTION__ << file.errorString();
    }

    QByteArray array(file.readAll());
    QJsonParseError err;
    QJsonDocument loadDoc(QJsonDocument::fromJson(array, &err));
    jsonArray = loadDoc.array();
    qDebug() << err.errorString();

    //    for(const QJsonValue& value : qAsConst(jsonArray)) {
    //        QJsonObject object(value.toObject());
    //        cbxAmkSet->addItem(object["name"].toString());
    //    }

    //    int idxCtr = 0;

    //    for(const QJsonValue& value : jsonArray[lastIndex].toObject()["data"].toArray()) {
    //        QJsonObject object2(value.toObject());
    //        m_points[idxCtr] = Point(value.toObject());
    //        m_points[idxCtr] = Point(value.toObject());
    //        leDescription[idxCtr /* */]->setText(m_points[idxCtr].Description);
    //        leDescription[idxCtr + SetCount]->setText(m_points[idxCtr].Description);
    //        ++idxCtr;
    //    }

    //    connect(cbxAmkSet, qOverload<int>(&QComboBox::currentIndexChanged), this, &RelaySet::cbxTypeIndexChanged);
}

void RelaySetHolder::saveSets() {
    //    if(!setsChanged)
    //        return;

    //    QFile saveFile(QStringLiteral("data_.json"));

    //    if(!saveFile.open(QIODevice::WriteOnly)) {
    //        qWarning() << __FUNCTION__ << saveFile.errorString();
    //        return;
    //    }

    //    QJsonObject levelObject;
    //    levelObject["name"] = cbxAmkSet->itemText(lastIndex);
    //    QJsonArray array;
    //    for(Point& p : m_points) {
    //        QJsonObject jsonObject;
    //        jsonObject["name"] = p.Description;
    //        jsonObject["value"] = p.Parcel;
    //        array.append(jsonObject);
    //    }
    //    levelObject["data"] = array;

    //    if(cbxAmkSet->count() > jsonArray.count())
    //        jsonArray.append(levelObject);
    //    else
    //        jsonArray[lastIndex] = levelObject;

    //    QJsonDocument saveDoc(jsonArray);
    //    saveFile.write(saveDoc.toJson());
}

QStringList RelaySetHolder::sets() {
    QStringList list;
    auto& array = qAsConst(jsonArray);
    list.reserve(array.size());
    for(const QJsonValue& value : array)
        list.append(value.toObject()["name"].toString());
    return list;
}

void RelaySetHolder::initPoints(Point* points, int index) {
    auto array{ jsonArray[index].toObject()["data"].toArray() };
    for(auto [idx, value] : rv::zip(rv::iota(0), array))
        points[idx] = Point(value.toObject());
}
