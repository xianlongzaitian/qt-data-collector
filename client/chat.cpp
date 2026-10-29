#include "chat.h"
#include "ui_chat.h"

#include <QMessageBox>
#include <QTimer>
#include <QRandomGenerator>

Chat::Chat(QTcpSocket* s,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Chat)
{
    ui->setupUi(this);
    socket = s;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Chat::onAutoSend);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_sendButton_clicked()
{
    // 控件名和界面上的顺序不一致：lineEdit_2=id, lineEdit=device,
    // lineEdit_3=temp, lineEdit_4=humi
    const QString device = ui->lineEdit->text().trimmed();
    const QString temp   = ui->lineEdit_3->text().trimmed();
    const QString humi   = ui->lineEdit_4->text().trimmed();

    if (device.isEmpty() || temp.isEmpty() || humi.isEmpty()) {
        QMessageBox::warning(this, "提示", "device / temp / humi 都要填");
        return;
    }

    socket->write(QString("%1,%2,%3").arg(device, temp, humi).toUtf8());
}

void Chat::on_autoButton_clicked()
{
    if (timer->isActive()) {
        timer->stop();
        ui->autoButton->setText("开始自动上报");
    } else {
        timer->start(1000);
        ui->autoButton->setText("停止自动上报");
    }
}

void Chat::onAutoSend()
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        timer->stop();
        ui->autoButton->setText("开始自动上报");
        return;
    }

    QString device = ui->lineEdit->text().trimmed();
    if (device.isEmpty())
        device = "sensor01";

    const double temp = 25.0 + QRandomGenerator::global()->bounded(50) / 10.0;
    const double humi = 55.0 + QRandomGenerator::global()->bounded(100) / 10.0;

    const QString line = QString("%1,%2,%3")
                             .arg(device)
                             .arg(temp, 0, 'f', 1)
                             .arg(humi, 0, 'f', 1);

    socket->write(line.toUtf8());
}
