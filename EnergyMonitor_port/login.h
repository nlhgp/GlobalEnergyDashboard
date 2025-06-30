#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <mainwindow.h>
#include <QGraphicsDropShadowEffect>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    int type = 0;
    void get_socket(QTcpSocket *s);//传递连接
    void init_ui();

private slots://所有的槽函数声明都要加上slots
    void on_DownloadButton_clicked();
    void on_EnrollButton_clicked();
    void switch_line();
    void switch_hidepassword();
    void enroll_to_tcp(QString s1,QString s2);
    void login_sign(QString sign);//接收服务器登录信息
    void enroll_sign(QString sign);//接收服务器注册信息

private:
    Ui::login *ui;
    EnrollWidget *w1=NULL;//注册子窗口
    MainWindow *port;//客户端
    QTcpSocket *socket;//连接
};

#endif // LOGIN_H
