#ifndef ENROLLWIDGET_H
#define ENROLLWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QProcess>
#include <qtcpsocket.h>

namespace Ui {
class EnrollWidget;
}

class EnrollWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EnrollWidget(QWidget *parent = nullptr);
    ~EnrollWidget();
    void get_enroll_sign(QString sign);//处理注册账号信息
    void init_ui();
    int model=0;

private slots://槽函数都要加slots
    void on_checkpushButton_clicked();
    void on_EnrollpushButton_clicked();
    void switch_line();
    void switch_hidepassword();

private:
    Ui::EnrollWidget *ui;
    QTcpSocket *socket;

signals:
    void enroll(QString,QString);
};

#endif // ENROLLWIDGET_H
