#include "widget.h"
#include "ui_widget.h"
#include "chat.h"

#include <QMessageBox>
#include <QHostAddress>
#include <QTcpSocket>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);

    // 信号槽只在构造函数里连一次
    //    不要放在按钮槽里 —— 否则每点一次"连接"就多连一条，
    //    一个信号会触发好几次槽
    connect(socket, &QTcpSocket::connected, this, [this]() {
        ui->logEdit->append(QString("== 已连接 %1:%2 ==")
                                .arg(ui->IplineEdit->text())
                                .arg(ui->portlineEdit->text()));
        this->hide();
        Chat* chat=new Chat(socket);
        chat->setAttribute(Qt::WA_DeleteOnClose);
        chat->show();
    });

    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        ui->logEdit->append("== 连接已断开 ==");
    });


    connect(socket, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        ui->logEdit->append("== 出错: " + socket->errorString() + " ==");
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_connect_clicked()
{
    const QString ip   = ui->IplineEdit->text().trimmed();
    const QString port = ui->portlineEdit->text().trimmed();

    if (ip.isEmpty() || port.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写服务器地址和端口");
        return;
    }

    bool ok = false;
    const quint16 portNum = port.toUShort(&ok);   // 端口范围 0~65535，要用 toUShort

    if (!ok || portNum == 0) {
        QMessageBox::warning(this, "提示", "端口号不合法（应为 1~65535）");
        return;
    }

    // connectToHost 接受 QHostAddress 或 QString(主机名)，传 QString 时域名也能用
    socket->connectToHost(ip, portNum);
    ui->logEdit->append(QString("正在连接 %1:%2 ...").arg(ip).arg(portNum));

}

void Widget::on_cancel_clicked()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
        ui->logEdit->append("已主动断开");
    } else {
        close();
    }
}

