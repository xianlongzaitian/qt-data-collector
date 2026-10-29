#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

    void newClientHandle();
    void clientInfoSlot();
    void refreshTable();

private:
    Ui::Widget *ui;
    QTcpServer* server;
    QList<QTcpSocket*> sockets;
    QSqlDatabase db;
    QSqlQueryModel* model;
};
#endif // WIDGET_H
