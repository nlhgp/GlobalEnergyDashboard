#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    setWindowTitle("登录");
    init_ui();
    //将密码框设置为隐藏
    ui->password_line->setEchoMode(QLineEdit::Password);
    //创建主页面，用于连接服务器
    port = new MainWindow;
    ui->ipline->setText(port->get_ip());
    //创建注册页面
    w1=new EnrollWidget;
    //四个参数，谁发出信号，发出什么信号，发信号给谁，怎么处理
    //谁发出信号就是ui中的哪个成员
    //发出什么信号可以去类里边找，比如id_line就去/帮助/LineEdit/信号 找
    connect(ui->name_line,SIGNAL(returnPressed()),this,SLOT(switch_line()));
    connect(ui->password_line,&QLineEdit::returnPressed,this,&login::on_DownloadButton_clicked);
    connect(ui->loginButton,&QPushButton::clicked,this,&login::on_DownloadButton_clicked);
    connect(ui->canseebox,SIGNAL(stateChanged(int)),this,SLOT(switch_hidepassword()));
    connect(this->port,SIGNAL(login(QString)),this,SLOT(login_sign(QString)));//登录账号
    connect(this->w1,&EnrollWidget::enroll,this,&login::enroll_to_tcp);//向服务器发送注册账号信息
    connect(this->port,&MainWindow::enroll,this,&login::enroll_sign);//注册账号
}

login::~login()
{
    delete ui;
}

void login::init_ui() {
    //-------------------------------------------
    // 1. 设置全局样式表
    //-------------------------------------------
    QString styleSheet = R"(
        /* 基础全局样式 */
        QWidget {
            background-color: #0A0F2B; /* 深空蓝背景 */
            font-family: "Microsoft YaHei", "Segoe UI";
        }


        /* 标题标签 - 霓虹渐变文字 */
        QLabel#title_label {
            color: #7B61FF;
            font-size: 12px;
            qproperty-alignment: AlignCenter;
        }

        /* 输入框 - 全息边框效果 */
        QLineEdit {
            background-color: rgba(16, 22, 54, 0.9);
            border: 2px solid #3A3F6B;
            border-radius: 8px;
            color: #E0E0FF;
            padding: 8px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 2px solid qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00F7FF, stop:1 #7B61FF);
        }

        /* 按钮 - 动态渐变效果 */
        QPushButton {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #2A2F5C, stop:1 #1A1F4C);
            border: 1px solid #4A4F7B;
            border-radius: 6px;
            color: #7B61FF;
            padding: 8px 16px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #3A3F7C, stop:1 #2A2F6C);
            border-color: #7B61FF;
        }
        QPushButton:pressed {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #1A1F4C, stop:1 #0A0F2B);
        }

        /* 登录按钮特殊样式 */
        QPushButton#loginButton {
            font-size: 16px;
            color: #00F7FF;
            border: 2px solid #7B61FF;
        }

        /* 复选框 - 未来感设计 */
        QCheckBox {
            color: #7B61FF;
            spacing: 5px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 2px solid #4A4F7B;
            border-radius: 4px;
            background: rgba(16, 22, 54, 0.9);
        }
        QCheckBox::indicator:checked {
            background-color: #7B61FF;
            image: url(:/icons/check_cyan.svg); /* 自定义勾选图标 */
        }

        QMessageBox {
            color: #E0E0FF;
        }

        /* 服务器IP标签 */
        QLabel#iplabel {
            color: #7B61FF;
            font-size: 12px;
            qproperty-alignment: AlignCenter;
        }
    )";
    this->setStyleSheet(styleSheet);

    //-------------------------------------------
    // 2. 布局优化
    //-------------------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20); // 设置边距
    mainLayout->setSpacing(15); // 设置组件间距

    // 标题
    mainLayout->addWidget(ui->title_label);

    // 用户名输入框
    QHBoxLayout *usernameLayout = new QHBoxLayout();
    usernameLayout->addWidget(ui->name_label);
    usernameLayout->addWidget(ui->name_line);
    mainLayout->addLayout(usernameLayout);

    // 密码输入框
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->addWidget(ui->password_label);
    passwordLayout->addWidget(ui->password_line);
    mainLayout->addLayout(passwordLayout);

    // 显示密码复选框
    QHBoxLayout *checkboxLayout = new QHBoxLayout();
    checkboxLayout->addStretch();
    checkboxLayout->addWidget(ui->canseebox);
    mainLayout->addLayout(checkboxLayout);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(ui->EnrollButton);
    buttonLayout->addWidget(ui->loginButton);
    buttonLayout->addWidget(ui->RetrieveButton);
    mainLayout->addLayout(buttonLayout);

    // 服务器IP输入框
    QHBoxLayout *ipLayout = new QHBoxLayout();
    ipLayout->addWidget(ui->iplabel);
    ipLayout->addWidget(ui->ipline);
    mainLayout->addLayout(ipLayout);

    // 设置主布局
    this->setLayout(mainLayout);

    //-------------------------------------------
    // 3. 动态特效
    //-------------------------------------------
    // 登录按钮悬浮特效
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(ui->loginButton);
    shadow->setColor(QColor(123, 97, 255, 150));
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 3);
    ui->loginButton->setGraphicsEffect(shadow);

    // 输入框占位符美化
    ui->name_line->setPlaceholderText("请输入用户名");
    ui->name_line->setStyleSheet("QLineEdit::placeholder { color: #4A4F7B; }");
    ui->password_line->setPlaceholderText("请输入密码");
    ui->password_line->setStyleSheet("QLineEdit::placeholder { color: #4A4F7B; }");
    ui->ipline->setPlaceholderText("请输入服务器IP");
    ui->ipline->setStyleSheet("QLineEdit::placeholder { color: #4A4F7B; }");
}

//获取信号连接
void login::get_socket(QTcpSocket *s)
{
    this->socket=s;
}

//账号输入完毕后输入回车将光标切换到密码栏中
void login::switch_line()
{
    ui->password_line->setFocus();
}

//隐藏密码
void login::switch_hidepassword()
{
    if(ui->canseebox->isChecked())
        ui->password_line->setEchoMode(QLineEdit::Normal);
    else
        ui->password_line->setEchoMode(QLineEdit::Password);
}

//登录账号按钮
void login::on_DownloadButton_clicked()
{
    //获取lineedit数据
    QString name = ui->name_line->text();
    QString password = ui->password_line->text();
    QString sql;
    // QSqlQuery query;
    if(name.length()==0)
    {
        QMessageBox::information(this,"ERROR!","用户名不能为空");
        return;
    }
    else if(password.length()==0)
    {
        QMessageBox::information(this,"ERROR!","密码不能为空");
        return;
    }
    sql=QString("select password from users where username = '%1';").arg(name);
    port->get_logintext(sql,ui->password_line->text(),ui->ipline->text());//向客户端传递信号
}

//注册账号
void login::on_EnrollButton_clicked()
{
    ui->name_line->clear();
    ui->password_line->clear();
    w1->show();
}

//向客户端传递注册信息,S是账号#密码
void login::enroll_to_tcp(QString sql,QString s)
{
    port->get_enrolltext(sql,s,ui->ipline->text());
}

void login::login_sign(QString sign)
{
    if(sign=="00")
    {
        QMessageBox::information(this,"ERROR!","账号未注册\n请先注册账号");
    }
    else if(sign=="01")
    {
        //QMessageBox::information(this,"","登陆成功！");
        port->show();//客户端
        this->close();
    }
    else
        QMessageBox::information(this,"登陆失败","密码错误！");
}

void login::enroll_sign(QString sign)
{
    this->w1->get_enroll_sign(sign);
}

//127.0.0.1#sql#login#2#select password from users where id = 2;

// //找回密码
// void login::on_RetrieveButton_clicked()
// {
//     ui->password_line->clear();
//     this->w2=new RetrieveForm;
//     w2->init_id(ui->id_line->text());
//     w2->show();
// }
