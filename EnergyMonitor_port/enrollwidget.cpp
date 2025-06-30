#include "enrollwidget.h"
#include "ui_enrollwidget.h"

EnrollWidget::EnrollWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EnrollWidget)
{
    ui->setupUi(this);
    //将密码框设置成隐藏模式
    ui->passwordlineEdit->setEchoMode(QLineEdit::Password);
    ui->againlineEdit->setEchoMode(QLineEdit::Password);
    setWindowTitle("注册");
    //连接信号与槽
    //点击‘检测是否可用’则调用on_checkpushButton_clicked()
    connect(ui->checkpushButton,&QPushButton::clicked,this,&EnrollWidget::on_checkpushButton_clicked);
    //在行输入框内按回车就转到下一行，最后一行按下回车后就直接视为点击注册
    connect(ui->namelineEdit,SIGNAL(returnPressed()),this,SLOT(switch_line()));
    connect(ui->passwordlineEdit,SIGNAL(returnPressed()),this,SLOT(switch_line()));
    connect(ui->againlineEdit,&QLineEdit::returnPressed,this,&EnrollWidget::on_EnrollpushButton_clicked);
    //按下注册键即开始注册
    connect(ui->FinishEnroll, &QPushButton::clicked,this, &EnrollWidget::on_EnrollpushButton_clicked);
    //勾选可见后显示密码
    connect(ui->canseebox,SIGNAL(stateChanged(int)),this,SLOT(switch_hidepassword()));
    init_ui();
}

EnrollWidget::~EnrollWidget()
{
    delete ui;
}

//隐藏密码
void EnrollWidget::switch_hidepassword()
{
    if(ui->canseebox->isChecked())
    {
        ui->passwordlineEdit->setEchoMode(QLineEdit::Normal);
        ui->againlineEdit->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->passwordlineEdit->setEchoMode(QLineEdit::Password);
        ui->againlineEdit->setEchoMode(QLineEdit::Password);
    }
}

//初始化样式
void EnrollWidget::init_ui() {
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
        QLabel {
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
}

//注册账号
void EnrollWidget::on_EnrollpushButton_clicked()
{
    QString sql;
    //从密码文本框获取字符串
    QString password=ui->passwordlineEdit->text();
    QString name=ui->namelineEdit->text();
    this->model=2;
    if(ui->passwordlineEdit->text().length()==0)
    {
        QMessageBox::information(this,"ERROR!","密码不能为空");
        return;
    }
    sql=QString("INSERT INTO users (username, password, role) VALUES('%1','%2','normal');#").arg(name).arg(password);
    emit enroll(sql,(name+"#")+ password);
}

//按下回车后切换输入框
void EnrollWidget::switch_line()
{
    //在账号框按回车就跳到密码框
    if(focusWidget()==ui->namelineEdit)
    {
        ui->passwordlineEdit->setFocus();
    }
    //在密码框按回车就跳到重复框
    else if(focusWidget()==ui->passwordlineEdit)
    {
        ui->againlineEdit->setFocus();
    }
}

//检查账号是否已注册
void EnrollWidget::on_checkpushButton_clicked()
{
    QString sql;
    QString s = ui->namelineEdit->text();
    if(ui->namelineEdit->text().length()==0)
    {
        QMessageBox::information(this,"ERROR!","用户名不能为空");
    }
    if(ui->passwordlineEdit->text() != ui->againlineEdit->text())
    {
        QMessageBox::information(this,"ERROR!","两次输入密码不一致！");
    }
    sql=QString("select * from users where username=%1;#").arg(s);
    emit enroll(sql,"check");
}

void EnrollWidget::get_enroll_sign(QString sign)
{
    if(sign=="00")
    {
        QMessageBox::information(this,"ERROR!","账号已注册\n请重新指定账号");
    }
    else if(sign=="01")
    {
        if(this->model!=2)
            QMessageBox::information(this,"","账号未注册\n该账号可用");
        this->model=1;
    }
    else if(sign=="02")
    {
        QMessageBox::information(this,"","注册成功！");
        this->close();
    }
    else
    {
        QMessageBox::information(this,"","注册失败");
        qDebug() << sign;
    }
}

