#include "kdsdialog.h"
#include "hwinterface/interface.h"
#include "ui_kdsdialog.h"

#include <QAbstractButton>
#include <QDebug>
#include <QInputDialog>
#include <QMenu>
#include <QPushButton>

class KdsDataModel : public QAbstractTableModel {
    friend KdsDialog;

    static constexpr int rows = 5;
    int cols = 16;
    float m_answerData[5][16] = {};
    Kds* const kds;

public:
    KdsDataModel(Kds* kds, int cols, QObject* parent)
        : QAbstractTableModel(parent)
        , cols(cols)
        , kds(kds)
    {
    }
    ~KdsDataModel() { }

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override { return rows; }
    int columnCount(const QModelIndex& = {}) const override { return cols; }
    QVariant data(const QModelIndex& index, int role) const override
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return static_cast<double>(m_answerData[index.row()][index.column()]);
        else if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        return {};
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override
    {
        if (role == Qt::EditRole) {
            if (m_answerData[index.row()][index.column()] != value.toFloat()
                && kds->fileOpen()
                && kds->fileSeek(index.column() * 5 * 4 + index.row() * 4)
                && kds->fileWrite(value.toFloat())) {
                m_answerData[index.row()][index.column()] = value.toFloat();
                emit dataChanged(index, index, { role });
                return true;
            }
        } else if (role == Qt::DisplayRole) {
            m_answerData[index.row()][index.column()] = value.toFloat();
            emit dataChanged(index, index, { role });
            return true;
        }
        return {};
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        static auto hd = {
            "0 Ом",
            "40 Ом",
            "80 Ом",
            "160 Ом",
            "320 Ом",
        };
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Horizontal)
                return ++section;
            else
                return *(hd.begin() + section);
        } else if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
        return {};
    }
    Qt::ItemFlags flags(const QModelIndex&) const override
    {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
};

KdsDialog::KdsDialog(Kds* kds_, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::KdsDialog)
    , kds(kds_)
{
    ui->setupUi(this);
    for (int i = 0; i < ui->baudComboBox->count(); ++i)
        ui->baudComboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);

    ui->baudComboBox->setCurrentText(QString::number(kds->port()->baudRate()));
    ui->dataLineEdit->setText(QString("%1").arg(kds->getData(Kds::Data), 16, 2, QChar('0')));
    ui->versionLineEdit->setText(kds->getVer());
    ui->addressSpinBox->setValue(kds->address());
    ui->chCountSpinBox->setValue(kds->getData(Kds::ChCount));
    ui->serNumSpinBox->setValue(kds->getData(Kds::SerNum));

    //    kds->fileOpen();
    //    kds->fileSeek(0);
    //    kds->fileWrite(1.0f, 2.0f, 3.0f, 4.0f, 5.0f);
    //    kds->fileWrite(6.0f, 7.0f, 8.0f, 9.0f, 10.0f);
    //    kds->fileWrite(11.0f, 12.0f, 13.0f, 14.0f, 15.0f);
    //    kds->fileWrite(16.0f, 17.0f, 18.0f, 19.0f, 20.0f);
    //    kds->fileWrite(21.0f, 22.0f, 23.0f, 24.0f, 25.0f);
    //    kds->fileWrite(26.0f, 27.0f, 28.0f, 29.0f, 30.0f);
    //    kds->fileWrite(31.0f, 32.0f, 33.0f, 34.0f, 35.0f);
    //    kds->fileWrite(36.0f, 37.0f, 38.0f, 39.0f, 40.0f);
    //    kds->fileWrite(41.0f, 42.0f, 43.0f, 44.0f, 45.0f);
    //    kds->fileWrite(46.0f, 47.0f, 48.0f, 49.0f, 50.0f);
    //    kds->fileWrite(51.0f, 52.0f, 53.0f, 54.0f, 55.0f);
    //    kds->fileWrite(56.0f, 57.0f, 58.0f, 59.0f, 60.0f);
    //    kds->fileWrite(61.0f, 62.0f, 63.0f, 64.0f, 65.0f);
    //    kds->fileWrite(66.0f, 67.0f, 68.0f, 69.0f, 70.0f);
    //    kds->fileWrite(71.0f, 72.0f, 73.0f, 74.0f, 75.0f);
    //    kds->fileWrite(76.0f, 77.0f, 78.0f, 79.0f, 80.0f);

    { // setupp tableView
        ui->tableView->setModel(model = new KdsDataModel(kds, ui->chCountSpinBox->value(), ui->tableView));
        ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->tableView, &QWidget::customContextMenuRequested, [this](const QPoint& pos) {
            QMenu menu;
            menu.addAction("Set", [this] {
                auto selectedIndexes { ui->tableView->selectionModel()->selectedIndexes() };
                if (selectedIndexes.size()) {
                    bool ok;
                    auto val = QInputDialog::getDouble(this, "1", "2", selectedIndexes.front().data().toDouble(), -std::numeric_limits<float>::max(), +std::numeric_limits<float>::max(), 4, &ok);
                    if (ok)
                        for (auto& index : ui->tableView->selectionModel()->selectedIndexes())
                            model->setData(index, val);
                }
            });
            menu.exec(ui->tableView->viewport()->mapToGlobal(pos));
        });

        ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        int h = ui->baudComboBox->height() + ui->tableView->horizontalHeader()->height() + ui->tableView->rowHeight(0) * 5 + 20;
        setMinimumHeight(h);
        setMaximumHeight(h);
        kds->fileOpen();
        kds->fileSeek(0);

        float data[5];

        for (int channel = 0; channel < model->columnCount(); ++channel) {
            kds->fileRead(data);
            for (float& f : data)
                model->setData(model->createIndex(std::distance(data, &f), channel), f, Qt::DisplayRole);
        }
    }

    ui->buttonBox->button(QDialogButtonBox::Open)->setText("Чтение");
    ui->buttonBox->button(QDialogButtonBox::Apply)->setText("Запись");
    ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setText("Зав. уст.");

    connect(ui->buttonBox, &QDialogButtonBox::clicked, [this](QAbstractButton* button) {
        qDebug() << ui->buttonBox->buttonRole(button);
        switch (ui->buttonBox->standardButton(button)) {
        case QDialogButtonBox::Open:
            ui->baudComboBox->setCurrentText(QString::number(kds->port()->baudRate()));
            ui->dataLineEdit->setText(QString("%1").arg(kds->getData(Kds::Data), 16, 2, QChar('0')));
            ui->versionLineEdit->setText(kds->getVer());
            ui->addressSpinBox->setValue(kds->address());
            ui->chCountSpinBox->setValue(kds->getData(Kds::ChCount));
            ui->serNumSpinBox->setValue(kds->getData(Kds::SerNum));
            return;
        case QDialogButtonBox::Apply:
            onSerNumSpinBoxEditingFinished();
            thread()->sleep(1);
            onChCountSpinBoxEditingFinished();
            thread()->sleep(1);
            onAddressSpinBoxEditingFinished();
            thread()->sleep(1);
            onBaudComboBoxCurrentIndexChanged(ui->baudComboBox->currentIndex());
            thread()->sleep(1);
            onDataLineEditEditingFinished();
            return;
        case QDialogButtonBox::RestoreDefaults:
            ui->baudComboBox->setCurrentText("9600");
            ui->dataLineEdit->setText(QString("%1").arg(0, 16, 2, QChar('0')));
            ui->addressSpinBox->setValue(0);
            ui->chCountSpinBox->setValue(12);
            ui->serNumSpinBox->setValue(0);
            return;
        default:
            return;
        }
    });
    //    connect(ui->serNumSpinBox->findChild<QLineEdit*>(), &QLineEdit::returnPressed, this, &KdsDialog::onSerNumSpinBoxEditingFinished);
    //    connect(ui->chCountSpinBox->findChild<QLineEdit*>(), &QLineEdit::returnPressed, this, &KdsDialog::onChCountSpinBoxEditingFinished);
    //    connect(ui->addressSpinBox->findChild<QLineEdit*>(), &QLineEdit::returnPressed, this, &KdsDialog::onAddressSpinBoxEditingFinished);
    //    connect(ui->baudComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &KdsDialog::onBaudComboBoxCurrentIndexChanged);
    //    connect(ui->dataLineEdit, &QLineEdit::returnPressed, this, &KdsDialog::onDataLineEditEditingFinished);
}
KdsDialog::~KdsDialog()
{
    delete ui;
}

void KdsDialog::onSerNumSpinBoxEditingFinished()
{
    kds->writeSerNum(ui->serNumSpinBox->value());
}

void KdsDialog::onChCountSpinBoxEditingFinished()
{
    kds->writeChCount(ui->chCountSpinBox->value());
    //model->setColumnCount(ui->chCountSpinBox->value());
}

void KdsDialog::onAddressSpinBoxEditingFinished()
{
    kds->setAddress(ui->addressSpinBox->value());
}

void KdsDialog::onBaudComboBoxCurrentIndexChanged(int index)
{
    kds->setBaudRate(Elemer::Baud(index));
}

void KdsDialog::onDataLineEditEditingFinished()
{
    int data {};
    int ctr = 0x8000;
    for (const auto& c : ui->dataLineEdit->text()) {
        if (c == '1')
            data |= ctr;
        ctr >>= 1;
    }
    kds->writeRelay(data);
}
