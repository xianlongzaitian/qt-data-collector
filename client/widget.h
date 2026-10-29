#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QTcpSocket;          // 前向声明（.h 里只用指针）

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

private slots:
    // 自动连接的槽（名字必须是 on_<对象名>_<信号名>）
    void on_connect_clicked();
    void on_cancel_clicked();



private:
    Ui::Widget *ui;
    QTcpSocket *socket = nullptr;
};
#endif // WIDGET_H
