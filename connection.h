#ifndef CONNECTION_H
#define CONNECTION_H

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

class CONNECTION : public QWidget {
    Q_OBJECT
public:
    explicit CONNECTION(QWidget* parent = nullptr);
    ~CONNECTION();
signals:

public slots:

private:
    QComboBox* cbxPortAmk;
    QComboBox* cbxPortTest;
    QGroupBox* gbxAmk;
    QGroupBox* gbxTest;
    QGridLayout* gridLayout;
    QPushButton* pbPingAmk;
    QPushButton* pbPingTest;
    QPushButton* pbUpdateCbxPort;
    QSpacerItem* verticalSpacer;
    QSpacerItem* verticalSpacer_2;
    QVBoxLayout* verticalLayout;
    QVBoxLayout* verticalLayout_2;

    void setupUi(QWidget* Form); // setupUi
    void retranslateUi(QWidget* Form); // retranslateUi

    bool CheckConnectionAmk();
    bool CheckConnectionTest();

    void UpdateCbxPort();
    void CheckConnection();

    // QWidget interface
protected:
    void showEvent(QShowEvent* event) override;
};

#endif // CONNECTION_H
