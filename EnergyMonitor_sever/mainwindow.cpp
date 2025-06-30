#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include "QGraphicsDropShadowEffect"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sever = new QTcpServer;//服务器对象
    sever->listen(QHostAddress::AnyIPv4,PORT);//监听
    setWindowTitle("服务器");
    //连接数据库
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("energymonitor");//设置打开system_database
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("159357");
    if(!db.open())//检测是否连接成功
    {
        //QMessageBox::information(this,"连接提示","连接成功");
        QMessageBox::warning(this,"连接提示","数据库连接失败！");
    }
    //信号与槽
    connect(sever,&QTcpServer::newConnection,this,&MainWindow::new_client_hander);//连接客户端
    init_ui();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_ui()
{
        // ================== 主窗口样式 ==================
        this->setStyleSheet(
            "QMainWindow {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2c3e50, stop:1 #3498db);"
            "}"

            // ================== 全局字体 ==================
            "QLabel {"
            "   color: #ecf0f1;"
            "   font-family: 'Microsoft YaHei';"
            "}"

            // ================== 输入框样式 ==================
            "QLineEdit {"
            "   background: rgba(255,255,255,0.9);"
            "   border: 2px solid #2980b9;"
            "   border-radius: 8px;"
            "   padding: 6px 12px;"
            "   font-size: 14px;"
            "   color: #2c3e50;"
            "}"
            "QLineEdit:focus {"
            "   border-color: #e74c3c;"
            "   background: #ffffff;"
            "}"

            // ================== 列表控件 ==================
            "QListWidget {"
            "   background: rgba(255,255,255,0.85);"
            "   border-radius: 10px;"
            "   padding: 8px;"
            "   font-size: 13px;"
            "   color: #34495e;"
            "   alternate-background-color: #f0f0f0;"
            "}"
            "QListWidget::item {"
            "   padding: 8px;"
            "   border-bottom: 1px solid #bdc3c7;"
            "}"
            "QListWidget::item:selected {"
            "   background: #3498db;"
            "   color: white;"
            "}"

            // ================== 标题样式 ==================
            "#title {"
            "   background: rgba(52, 152, 219, 0.8);"
            "   border-radius: 15px;"
            "   padding: 15px;"
            "   font-size: 24px !important;"
            "   font-weight: bold;"
            "   color: #ecf0f1;"
            "}"

            // ================== 状态栏 ==================
            "QStatusBar {"
            "   background: rgba(44, 62, 80, 0.9);"
            "   color: #bdc3c7;"
            "   font-size: 12px;"
            "}"
            );

        // ================== 布局优化 ==================

        // 调整布局间距
        ui->gridLayout->setContentsMargins(15, 15, 15, 15);
        ui->gridLayout_3->setVerticalSpacing(10);
        ui->gridLayout_2->setContentsMargins(0, 0, 0, 10);

        // 设置列表控件的交替行颜色
        ui->userlist->setAlternatingRowColors(true);
        ui->messagelist->setAlternatingRowColors(true);

        // ================== 动态效果 ==================
        // 添加输入框动画
        QGraphicsDropShadowEffect *inputEffect = new QGraphicsDropShadowEffect;
        inputEffect->setBlurRadius(15);
        inputEffect->setOffset(3);
        inputEffect->setColor(QColor(52, 152, 219, 150));
        ui->iplineEdit->setGraphicsEffect(inputEffect);
        ui->portlineEdit->setGraphicsEffect(inputEffect);
        ui->messageline->setGraphicsEffect(inputEffect);

        // 标题浮出效果
        QGraphicsOpacityEffect *titleOpacity = new QGraphicsOpacityEffect(ui->title);
        titleOpacity->setOpacity(0.95);
        ui->title->setGraphicsEffect(titleOpacity);

        // ================== 字体优化 ==================
        QFont titleFont("Microsoft YaHei", 18, QFont::Bold);
        ui->title->setFont(titleFont);

        QFont listFont("Segoe UI", 11);
        ui->userlist->setFont(listFont);
        ui->messagelist->setFont(listFont);

        // ================== 高级样式 ==================
        // 自定义滚动条
        QString scrollBarStyle =
            "QScrollBar:vertical {"
            "   border: none;"
            "   background: rgba(200,200,200,50);"
            "   width: 10px;"
            "   margin: 0px 0 0px 0;"
            "}"
            "QScrollBar::handle:vertical {"
            "   background: rgba(52, 152, 219, 150);"
            "   border-radius: 4px;"
            "}"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
            "   height: 0px;"
            "}";
        ui->userlist->setStyleSheet(scrollBarStyle);
        ui->messagelist->setStyleSheet(scrollBarStyle);
}

//添加信息记录
void MainWindow::add_item(QString &m)
{
    ui->messagelist->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));//添加消息时间
    ui->messagelist->addItem(m.toUtf8());
    //ui->messagelist->addItem("");//空一行，不然挤着难看
    ui->messagelist->scrollToBottom();
}

//ip#sql#login#password#...
//172.17.15.220#sql#login#user123#select password from users where username = 'user1';
//处理登录账号信息
void MainWindow::login_text(QStringList &sl)
{
    QString message="login#";
    QSqlQuery query;
    qDebug() << sl[4];
    if(query.exec(sl[4]))
    {
        if(query.next()==false)//没找到账号
        {
            message += "00";//00，未找到账号
        }
        else if(query.value(0)==sl[3])//找到了，传回密码
        {
            message += "01";//01，密码正确，登录成功
            QStringList s=sl[4].split('\'');//将字符串以'分割
            ui->userlist->addItem(s[1]);
        }
        else
        {
            message += "02";//02，密码错误
        }
    }
    fstl(this->mts)
    {
        if(sl[0]==(*it)->socket->peerAddress().toString())
        {
            qDebug() << message;
            ((*it)->socket->write((message).toUtf8()));
        }
    }
}

//处理注册账号消息
void MainWindow::enroll_text(QStringList &sl)
{
    QString message="enroll#";
    QSqlQuery query;
    //ip#sql#enroll#check#sql语句
    if(sl[3]=="check")//检查账号是否已注册
    {
        if(query.exec(sl[4]))//运行sql语句
        {
            if(query.next()!=false)//找到账号了
            {
                message += "00";//00,账号已注册
            }
            else message += "01";//账号未注册
        }
    }
    //ip#sql#enroll#账号#密码#sql语句
    else
    {
        if(query.exec(sl[5]))
        {
            message += "02";//注册成功
        }
        else
        {
            QString sql=QString("select * from user where id=%1;#").arg(sl[3]);
            if(query.exec(sql))
            {
                if(query.next()!=false)
                    message += "00";//00,账号已注册
            }
        }
    }
    fstl(this->mts)
    {
        if(sl[0]==(*it)->socket->peerAddress().toString())
        {
            qDebug() << message;
            ((*it)->socket->write((message).toUtf8()));
        }
    }
}

//处理修改信息
void MainWindow::alter_text(QStringList &sl)
{
    QSqlQuery query;
    QString message = "alter#";
    //ip#sql#alter/init#账号#sql语句
    if(sl[2] == "init")
    {
        message = "init#";
        if(query.exec(sl[4]))//初始化组件
        {
            while(query.next())
            {
                message += query.value(1).toString()+"#";//用户名
                if(query.value(3).toString().length()==0) message += "null#";
                else message+=query.value(3).toString()+"#";
                if(query.value(4).toString().length()==0) message += "null#";
                else message+=query.value(4).toString()+"#";
                if(query.value(5).toString().length()==0) message += "null";
                else message+=query.value(5).toString();
                //init#username#s_id#email#phone
            }
        }
    }
    else if(sl[2]=="alter")
    {
        if(query.exec(sl[4]))
        {
            message += "00";//修改成功
        }
        else message += "01";
    }
    fstl(this->mts)
    {
        if(sl[0]==(*it)->socket->peerAddress().toString())
        {
            ((*it)->socket->write((message).toUtf8()));
        }
    }
    qDebug() <<message;
}

//建立新线程
void MainWindow::new_client_hander()
{
    //建立tcp连接，连接也是对象，类型为socket
    QTcpSocket *socket = sever->nextPendingConnection();
    socket->peerAddress();//获取客户端地址
    socket->peerPort();//获取客户端端口号
    ui->iplineEdit->setText(socket->peerAddress().toString());
    ui->portlineEdit->setText(QString::number(socket->peerPort()));
    // //收到消息socket会发送readysocket信号
    // connect(socket,&QTcpSocket::readyRead,this,&Widget::message);

    //启动线程
    my_thread *t=new my_thread;
    t->get_socket(socket);
    t->start();
    mts.push_back(t);
    connect(t,&my_thread::message,this,&MainWindow::threadslot);
}

//接收线程传来的信号
void MainWindow::threadslot(QString m)
{
    qDebug() << m;
    QStringList sl=m.split('#');//将字符串以#分割
    if(sl[1]=="sql")
    {
        //ip#sql#login#password#...
        if(sl[2]=="login")
            this->login_text(sl);
        //ip#sql#enroll#check#sql语句
        //ip#sql#enroll#账号#密码#sql语句
        if(sl[2]=="enroll")
            this->enroll_text(sl);
        //ip#sql#alter#账号#sql语句
        if(sl[2]=="alter"||sl[2]=="init")
            this->alter_text(sl);
    }
    else if(sl[1]=="deconned")//发送下线信息
    {
        fstl(this->mts)
        {
            (*it)->socket->write(((*it)->get_name()+"已下线").toUtf8());
            delete(*it);
            mts.erase(it);
        }
    }
    ui->messageline->setText(m.toUtf8());
    add_item(m);
}
