#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <enrollwidget.h>
#include <qmessagebox.h>//提示窗口
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QGroupBox>
//数据库
#include <QtSql/QSqlDatabase>//数据库头文件
#include <QSqlTableModel>
#include <qsqlquery.h>//操作数据库对象
#include <qtcpsocket.h>//网络
#include <qhostaddress.h>//ip地址类
//读取数据集文件
#include <QFile>
#include <QTextStream>
#include <QStringList>
//绘制图表
#include <vector>
#include <QtCharts/QPieSeries>//饼图和圆环图
#include <QtCharts/QLineSeries>//折线统计图
#include <QTableWidget>//表格
#include <QtCharts/QBarSeries>//柱状图
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QHorizontalBarSeries>//横向柱状图和横向柱状分组图
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChartView>
#include <QSplitter>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void get_logintext(QString &s,QString password,QString);//获取登录信息
    void get_enrolltext(QString &s,QString password,QString);//获取注册信息
    QString& get_ip();
    //整张表格
    void set_table(const QList<QStringList> &data);
    //获取当年数据接口
    QStringList getYearData(const QList<QStringList>& allData, int targetYear);
    //横向柱状图
    QChart* createHorizontalBarChart(const QList<QStringList> &data);
    //柱状图
    QChart* createBarChart(const QList<QStringList> &data);
    //折线统计图
    QChart* createLineChart(const QList<QStringList> &data);
    QChart* createRenewableEnergyTrendChart(const QList<QStringList>& data);
    QChart* createLowCarbonTrendChart(const QList<QStringList>& data);
    //创建饼图和圆环图
    QChart* createPieChart(const QList<QStringList> &data);
    //散点图
    QChart* createScatterChart(const QList<QStringList> &data);
    //获取数据集
    QList<QStringList> readCSV(const QString& filePath,
                                       int startRow = 4385,
                                       int endRow = 4407);

    //初始化布局
    void init_layout();
    void init_qss();
    void init_menubar();
    ~MainWindow();

private slots:
    void on_linkButton_clicked();//连接服务器
    void recieve_message();//接收服务器信息
    void recieve_alter(QString);//接收修改信息
    void connect_error(QAbstractSocket::SocketError);
    void disconnet();//断开连接提示
    void five_year();//最近五年情况分析
    void five_carbon();//近五年碳排放数据
    void five_bio();//近五年生物质能源
    void year_2022();//单个年份情况分析
    void year_2021();
    void year_2020();
    void year_2019();
    void year_2018();
    void read_China();
    void read_africa();
    void read_russia();
    void read_USA();

private:
    Ui::MainWindow *ui=NULL;
    QTcpSocket *socket=NULL;//网络对象指针
    QString user_id,username=NULL;//当前用户数据库id，用于信号传递
    int type=0;//查看当前属于什么模式
    int country=0;//查看当前是哪个国家
    QString ip = "127.0.0.1";
    QString port = "8000";
    QSqlTableModel *table=NULL;
    QChartView *chartView=NULL;//饼状图
    QChartView *lineChartView=NULL;//折线统计图
    QChartView *barChartView=NULL;//柱状图
    QChartView *HorizontalbarChartView=NULL;//横向柱状图
    QChartView *scatterChartView=NULL;//散点图
    QList<QStringList> data;//存储数据集信息
    std::vector<QChart*> tables;//01234饼折柱横散

signals:
    void login(QString qs);
    void enroll(QString qs);
};
#endif // MAINWINDOW_H

struct EnergyType {
    QString name;
    int index;  // 在数据中的列索引
    QColor color;
};
