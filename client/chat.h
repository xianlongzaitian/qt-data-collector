#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QTcpSocket>

class QTimer;

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QTcpSocket* s,QWidget *parent = nullptr);
    ~Chat();

private slots:
    void on_sendButton_clicked();
    void on_autoButton_clicked();
    void onAutoSend();

private:
    Ui::Chat *ui;
    QTcpSocket* socket = nullptr;
    QTimer *timer = nullptr;
};

#endif // CHAT_H
