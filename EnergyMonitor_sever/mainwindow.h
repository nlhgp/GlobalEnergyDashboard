#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <qtcpsocket.h>
#include <QtSql/QSqlDatabase>//数据库头文件
#include <qsqlquery.h>//操作数据库对象
#include <QMessageBox>
#include <my_thread.h>//线程
#include <QList>//容器
#define fstl(act) for(auto it=act.begin();it!=act.end();it++)
#define PORT 8000

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void new_client_hander();
    void threadslot(QString ba);
    void login_text(QStringList &sl);
    void enroll_text(QStringList &sl);
    void alter_text(QStringList &sl);

public:
    MainWindow(QWidget *parent = nullptr);
    void init_ui();
    void add_item(QString &m);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpServer *sever;
    QList<my_thread*> mts;
    QSqlDatabase db;
};
#endif // MAINWINDOW_H
