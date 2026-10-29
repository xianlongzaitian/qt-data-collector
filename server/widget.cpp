#include "widget.h"
#include "ui_widget.h"

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    server = new QTcpServer(this);

    // listen 失败（端口被占用）时不会有任何提示，必须检查返回值
    const quint16 listenPort = 8888;
    if (server->listen(QHostAddress::AnyIPv4, listenPort)) {
        setWindowTitle(QString("服务器 - 监听中 :%1").arg(listenPort));
    } else {
        setWindowTitle("服务器 - 监听失败");
        QMessageBox::critical(this, "监听失败",
                              QString("端口 %1 可能已被占用：\n%2")
                                  .arg(listenPort)
                                  .arg(server->errorString()));
    }

    db = QSqlDatabase::addDatabase("QODBC");
    const QString connStr = "DRIVER={MySQL ODBC 26.10 Unicode Driver};"
                            "SERVER=127.0.0.1;"
                            "DATABASE=device_data;"
                            "USER=root;"
                            "PASSWORD=admin;"
                            "PORT=3306;";
    db.setDatabaseName(connStr);

    if (db.open()) {
        model = new QSqlQueryModel(this);
        ui->tableView->setModel(model);
        refreshTable();
    } else {
        QMessageBox::warning(this, "信息", "数据库打开失败：" + db.lastError().text());
        qDebug() << db.lastError();
    }

    connect(server, &QTcpServer::newConnection, this, &Widget::newClientHandle);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::newClientHandle()
{
    QTcpSocket *socket = server->nextPendingConnection();
    sockets.append(socket);

    qDebug() << "新客户端:" << socket->peerAddress().toString()
             << socket->peerPort() << "当前共" << sockets.size();

    ui->IpLineEdit->setText(QString("已连接 %1 个客户端").arg(sockets.size()));
    ui->lineEdit_2->setText(socket->peerAddress().toString());

    connect(socket, &QTcpSocket::readyRead, this, &Widget::clientInfoSlot);
    connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
        sockets.removeAll(socket);
        qDebug() << "客户端断开，剩余" << sockets.size();
        socket->deleteLater();
    });
}

void Widget::clientInfoSlot()
{
    // 多客户端时靠 sender() 判断数据来自哪个连接
    QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
    if (!s)
        return;

    const QString line = QString::fromUtf8(s->readAll()).trimmed();
    if (line.isEmpty())
        return;

    qDebug() << "收到数据:" << line;

    // 协议格式: device,temp,humi
    const QStringList parts = line.split(',');
    if (parts.size() != 3) {
        qDebug() << "格式错误:" << line;
        return;
    }

    const QString device = parts[0].trimmed();
    bool okTemp = false;
    bool okHumi = false;
    const double temp = parts[1].trimmed().toDouble(&okTemp);
    const double humi = parts[2].trimmed().toDouble(&okHumi);

    if (device.isEmpty() || !okTemp || !okHumi) {
        qDebug() << "数据非法:" << line;
        return;
    }

    QSqlQuery q;
    q.prepare("insert into device (device, temp, humi) values (?, ?, ?)");
    q.addBindValue(device);
    q.addBindValue(temp);
    q.addBindValue(humi);

    if (!q.exec()) {
        qDebug() << "入库失败:" << q.lastError().text();
        return;
    }

    refreshTable();
}

void Widget::refreshTable()
{
    model->setQuery("SELECT Id, device, temp, humi FROM device "
                    "ORDER BY Id DESC LIMIT 100");

    if (model->lastError().isValid()) {
        qDebug() << "查询失败:" << model->lastError().text();
        return;
    }

    model->setHeaderData(0, Qt::Horizontal, "编号");
    model->setHeaderData(1, Qt::Horizontal, "设备");
    model->setHeaderData(2, Qt::Horizontal, "温度");
    model->setHeaderData(3, Qt::Horizontal, "湿度");
}
