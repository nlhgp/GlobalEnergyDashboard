#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMovie"
#include "QToolTip"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QTcpSocket;//创建socket对象
    setWindowTitle("客户端");
    ui->portline->setText(port);
    //获取ip地址和端口号
    //向服务器主机发起连接
    //两个参数是hostaddress对象的构造函数和port转换成短整型
    //socket->connectToHost(QHostAddress(ip),port.toShort());
    /*信号与槽*/
    //连接服务器成功socket对象会发出信号
    //connect(socket,&QTcpSocket::connected,this,&MainWindow::init_user);
    //如果连接失败，QTcpSocket会发出errorOccurred()信号
    connect(socket,&QTcpSocket::errorOccurred,this,&MainWindow::connect_error);
    //连接断开也会发出信号
    connect(socket,&QTcpSocket::disconnected,this,&MainWindow::disconnet);
    //接收信号
    connect(socket,&QTcpSocket::readyRead,this,&MainWindow::recieve_message);//接收服务器信息
    //菜单栏
    connect(ui->act_2022,&QAction::triggered,this,&MainWindow::year_2022);
    connect(ui->act_2021,&QAction::triggered,this,&MainWindow::year_2021);
    connect(ui->act_2020,&QAction::triggered,this,&MainWindow::year_2020);
    connect(ui->act_2019,&QAction::triggered,this,&MainWindow::year_2019);
    connect(ui->act_2018,&QAction::triggered,this,&MainWindow::year_2018);
    connect(ui->act_5,&QAction::triggered,this,&MainWindow::five_year);//分析近5年情况
    connect(ui->act_5_c,&QAction::triggered,this,&MainWindow::five_carbon);
    connect(ui->act_5_h,&QAction::triggered,this,&MainWindow::five_bio);
    connect(ui->actionChina,&QAction::triggered,this,&MainWindow::read_China);
    connect(ui->actionAfrica,&QAction::triggered,this,&MainWindow::read_africa);
    connect(ui->actionRussia,&QAction::triggered,this,&MainWindow::read_russia);
    connect(ui->actionUSA,&QAction::triggered,this,&MainWindow::read_USA);

    //读取csv文件
    data = readCSV("D:/QT/keketong/World Energy Consumption.csv");
    // 设置饼状图
    QChart *chart1 = createPieChart(data);
    tables.push_back(chart1);
    chartView = new QChartView(chart1);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 创建折线统计图
    QChart *chart2=createLineChart(data);
    tables.push_back(chart2);
    lineChartView = new QChartView(chart2);
    lineChartView->setRenderHint(QPainter::Antialiasing);

    //创建柱状统计图
    QChart *chart3=createBarChart(data);
    tables.push_back(chart3);
    barChartView = new QChartView(chart3);
    barChartView->setRenderHint(QPainter::Antialiasing);

    // 创建横向柱状图
    QChart *chart4 = createHorizontalBarChart(data);
    tables.push_back(chart4);
    HorizontalbarChartView = new QChartView(chart4);
    HorizontalbarChartView->setRenderHint(QPainter::Antialiasing);

    //放映地图
    // QPixmap map("D:/QT/keketong/map.jpg");
    // ui->photolabel->setPixmap(map.scaled(ui->photolabel->size(), Qt::KeepAspectRatio));
    QMovie *movie = new QMovie("D:/QT/keketong/GIF1.gif");
    ui->photolabel->setMovie(movie); // 设置要显示的GIF动画图片
    movie->start(); // 启动动画
    set_table(data);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::read_China()
{
    country = 0;
    data=readCSV("D:/QT/keketong/World Energy Consumption.csv");
    set_table(data);//表格
    QMovie *movie = new QMovie("D:/QT/keketong/GIF1.gif");
    ui->photolabel->setMovie(movie); // 设置要显示的GIF动画图片
    movie->start(); // 启动动画
    lineChartView = new QChartView(createLineChart(data));//折线图
    lineChartView->setRenderHint(QPainter::Antialiasing);
    year_2022();
    socket->write((this->username+"#counry#China").toUtf8());
}

//读取非洲
void MainWindow::read_africa()
{
    country = 1;
    data=readCSV("D:/QT/keketong/World Energy Consumption.csv",147,269);
    set_table(data);
    QPixmap map("D:/QT/keketong/africa1.jpg");
    ui->photolabel->setPixmap(map.scaled(ui->photolabel->size(), Qt::KeepAspectRatio));
    lineChartView = new QChartView(createLineChart(data));
    lineChartView->setRenderHint(QPainter::Antialiasing);
    year_2022();
    socket->write((this->username+"#counry#Africa").toUtf8());
}

//读取俄罗斯
void MainWindow::read_russia()
{
    country = 2;
    data=readCSV("D:/QT/keketong/World Energy Consumption.csv",16090,16947);
    set_table(data);
    QPixmap map("D:/QT/keketong/Russia.jpg");
    ui->photolabel->setPixmap(map.scaled(ui->photolabel->size(), Qt::KeepAspectRatio));
    lineChartView = new QChartView(createLineChart(data));
    lineChartView->setRenderHint(QPainter::Antialiasing);
    year_2022();
    socket->write((this->username+"#counry#Russia").toUtf8());
}

//读取美国
void MainWindow::read_USA()
{
    country = 3;
    data=readCSV("D:/QT/keketong/World Energy Consumption.csv",20590,20712);
    set_table(data);
    QPixmap map("D:/QT/keketong/USA.png");
    ui->photolabel->setPixmap(map.scaled(ui->photolabel->size(), Qt::KeepAspectRatio));
    lineChartView = new QChartView(createLineChart(data));
    lineChartView->setRenderHint(QPainter::Antialiasing);
    year_2022();
    socket->write((this->username+"#counry#USA").toUtf8());
}

void MainWindow::five_year()//化石能源
{
    this->type=55;
    this->chartView->chart()->setTitle("能源消耗种类分析");
    this->HorizontalbarChartView->chart()->setTitle("能源消耗占比分析");
    this->barChartView->chart()->setTitle("能源消耗趋势");
    // this->chartView->chart()->setTitle("糖尿病患者年龄分析");
    // this->HorizontalbarChartView->chart()->setTitle("患病种类分析");
    // this->barChartView->chart()->setTitle("患病因素分析");
    init_qss();
    socket->write((this->username+"#energy_type#Fossil_fuels").toUtf8());
}

void MainWindow::five_carbon()//低碳能源
{
    this->type=56;
    this->chartView->chart()->setTitle("近五年碳排放来源分析");
    this->HorizontalbarChartView->chart()->setTitle("低碳能源消费量分析（归一化）");
    this->barChartView->chart()->setTitle("5年内碳排放情况（2018-2022）");
    init_qss();
    socket->write((this->username+"#energy_type#Low_carbon").toUtf8());
}

void MainWindow::five_bio()//可再生能源
{
    this->type=57;
    this->chartView->chart()->setTitle("能源消费结构分析");
    this->HorizontalbarChartView->chart()->setTitle("可再生能源消费量分析（归一化）");
    this->barChartView->chart()->setTitle("可再生能源消耗趋势（2018-2022）");
    init_qss();
    socket->write((this->username+"#energy_type#Renewable_energy").toUtf8());
}

void MainWindow::year_2022() {
    this->type = 22;

    // 保存旧指针
    QChart *oldChart = tables[0];
    QChartView *oldChartView = chartView;

    this->barChartView->chart()->setTitle("能耗成分对比");

    // 创建新饼图
    tables[0] = createPieChart(data);
    chartView = new QChartView(tables[0]);
    chartView->setRenderHint(QPainter::Antialiasing);

    //横向柱状图
    tables[3] = createHorizontalBarChart(data);
    HorizontalbarChartView = new QChartView(tables[3]);
    HorizontalbarChartView->setRenderHint(QPainter::Antialiasing);

    //柱状图
    barChartView = new QChartView(createBarChart(data));
    barChartView->setRenderHint(QPainter::Antialiasing);

    // 删除旧指针
    if (oldChart) {
        delete oldChart;
        oldChart = nullptr;
    }
    if (oldChartView) {
        delete oldChartView;
        oldChartView = nullptr;
    }

    // 更新界面
    init_layout();
    socket->write((this->username+"#year#2022").toUtf8());
}

void MainWindow::year_2021() {
    this->type = 21;

    // 保存旧指针
    QChart *oldChart = tables[0];
    QChartView *oldChartView = chartView;

    this->barChartView->chart()->setTitle("能耗成分对比");

    // 创建新图表
    tables[0] = createPieChart(data);
    chartView = new QChartView(tables[0]);
    chartView->setRenderHint(QPainter::Antialiasing);

    //横向柱状图
    tables[3] = createHorizontalBarChart(data);
    HorizontalbarChartView = new QChartView(tables[3]);
    HorizontalbarChartView->setRenderHint(QPainter::Antialiasing);

    //柱状图
    barChartView = new QChartView(createBarChart(data));
    barChartView->setRenderHint(QPainter::Antialiasing);

    // 删除旧指针
    if (oldChart) {
        delete oldChart;
        oldChart = nullptr;
    }
    if (oldChartView) {
        delete oldChartView;
        oldChartView = nullptr;
    }

    // 更新界面
    init_layout();
    socket->write((this->username+"#year#2021").toUtf8());
}

void MainWindow::year_2020() {
    this->type = 20;

    // 保存旧指针
    QChart *oldChart = tables[0];
    QChartView *oldChartView = chartView;

    this->barChartView->chart()->setTitle("能耗成分对比");

    // 创建新图表
    tables[0] = createPieChart(data);
    chartView = new QChartView(tables[0]);
    chartView->setRenderHint(QPainter::Antialiasing);

    //横向柱状图
    tables[3] = createHorizontalBarChart(data);
    HorizontalbarChartView = new QChartView(tables[3]);
    HorizontalbarChartView->setRenderHint(QPainter::Antialiasing);

    //柱状图
    barChartView = new QChartView(createBarChart(data));
    barChartView->setRenderHint(QPainter::Antialiasing);

    // 删除旧指针
    if (oldChart) {
        delete oldChart;
        oldChart = nullptr;
    }
    if (oldChartView) {
        delete oldChartView;
        oldChartView = nullptr;
    }

    // 更新界面
    init_layout();
    socket->write((this->username+"#year#2020").toUtf8());
}

void MainWindow::year_2019()
{
    this->type=19;
    // 保存旧指针
    QChart *oldChart = tables[0];
    QChartView *oldChartView = chartView;

    this->barChartView->chart()->setTitle("能耗成分对比");

    // 创建新图表
    tables[0] = createPieChart(data);
    chartView = new QChartView(tables[0]);
    chartView->setRenderHint(QPainter::Antialiasing);

    tables[3] = createHorizontalBarChart(data);
    HorizontalbarChartView = new QChartView(tables[3]);
    HorizontalbarChartView->setRenderHint(QPainter::Antialiasing);

    //柱状图
    barChartView = new QChartView(createBarChart(data));
    barChartView->setRenderHint(QPainter::Antialiasing);

    // 删除旧指针
    if (oldChart) {
        delete oldChart;
        oldChart = nullptr;
    }
    if (oldChartView) {
        delete oldChartView;
        oldChartView = nullptr;
    }

    // 更新界面
    init_layout();
    socket->write((this->username+"#year#2019").toUtf8());
}

void MainWindow::year_2018()
{
    this->type=18;
    // 保存旧指针
    QChart *oldChart = tables[0];
    QChartView *oldChartView = chartView;

    this->barChartView->chart()->setTitle("能耗成分对比");

    // 创建新图表
    tables[0] = createPieChart(data);
    chartView = new QChartView(tables[0]);
    chartView->setRenderHint(QPainter::Antialiasing);

    tables[3] = createHorizontalBarChart(data);
    HorizontalbarChartView = new QChartView(tables[3]);
    HorizontalbarChartView->setRenderHint(QPainter::Antialiasing);

    //柱状图
    barChartView = new QChartView(createBarChart(data));
    barChartView->setRenderHint(QPainter::Antialiasing);

    // 删除旧指针
    if (oldChart) {
        delete oldChart;
        oldChart = nullptr;
    }
    if (oldChartView) {
        delete oldChartView;
        oldChartView = nullptr;
    }

    // 更新界面
    init_layout();
    socket->write((this->username+"#year#2018").toUtf8());
}

//换个布局
void MainWindow::init_qss()
{
    // 清理旧的中央部件
    QWidget *oldCentralWidget = this->centralWidget();
        if (oldCentralWidget) {
            oldCentralWidget->deleteLater(); // 延迟删除
        }

        // 创建新的中央部件
        QWidget *centralWidget = new QWidget(this);
        this->setCentralWidget(centralWidget);

        //创建散点图
        QChart *chart5 = createScatterChart(data);
        if(this->type==56)//低碳
        {
            chart5=createLowCarbonTrendChart(data) ;
        }
        if(this->type==57)//可再生
        {
            chart5 = createRenewableEnergyTrendChart(data);
        }
        tables.push_back(chart5);
        scatterChartView = new QChartView(chart5);
        scatterChartView->setRenderHint(QPainter::Antialiasing);

        // 设置全局样式表
        QString styleSheet = R"(
        /* 基础全局样式 */
        QWidget {
            background-color: #0A0F2B; /* 深空蓝背景 */
            font-family: "Microsoft YaHei", "Segoe UI";
            color: #E0E0FF; /* 浅灰色文字 */
        }

        /* 分组框样式 */
        QGroupBox {
            border: 2px solid qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00F7FF, stop:1 #7B61FF); /* 霓虹渐变边框 */
            border-radius: 8px;
            margin-top: 10px;
            padding: 10px;
            background-color: rgba(16, 22, 54, 0.9); /* 半透明深蓝背景 */
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            color: #00F7FF; /* 青色标题 */
            font-size: 16px;
            font-weight: bold;
            text-transform: uppercase; /* 英文大写 */
        }

        /* 图表视图样式 */
        QChartView {
            background-color: transparent; /* 透明背景 */
        }
        QChart {
            background-color: transparent; /* 透明背景 */
        }

        /* 表格样式 */
        QTableView {
            background-color: rgba(11, 17, 43, 0.8);
            alternate-background-color: rgba(23, 30, 66, 0.6);
            color: #E0E0FF;
            border: 1px solid #3A3F6B;
            selection-background-color: #7B61FF;
        }
        QHeaderView::section {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2A2F5C, stop:1 #1A1F4C);
            color: #7B61FF;
            border: 1px solid #4A4F7B;
            padding: 6px;
            font-weight: bold;
        }

        /* 输入框样式 */
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

        /* 按钮样式 */
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

        /* 复选框样式 */
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

        /* 悬浮提示特效 */
        QToolTip {
            background-color: #1A1F4C;
            color: #00F7FF;
            border: 1px solid #7B61FF;
            border-radius: 4px;
        }
    )";
        this->setStyleSheet(styleSheet);

        //-------------------------------------------
        // 2. 图表细节美化
        //-------------------------------------------
        // 统一图表主题
        //qDebug() << (tables[4]==NULL);
        //qDebug() << scatterChartView;
        QList<QChartView*> charts = { chartView, lineChartView, barChartView, HorizontalbarChartView,scatterChartView};
        for (auto chartView : charts) {
            if(chartView && chartView->chart())
            {
            chartView->chart()->setTheme(QChart::ChartThemeDark); // 深色主题
            chartView->chart()->setBackgroundBrush(Qt::NoBrush);  // 透明背景
            chartView->chart()->setTitleBrush(QBrush(Qt::white)); // 白色标题
            chartView->chart()->setTitleFont(QFont("微软雅黑", 12, QFont::Bold));
            chartView->setRenderHint(QPainter::Antialiasing);     // 抗锯齿
            }
        }


        //-------------------------------------------
        // 1. 主布局结构（上下两层）
        //-------------------------------------------
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(15, 15, 15, 15); // 整体边距
        mainLayout->setSpacing(15); // 设置组件间距

        // 上层布局（核心数据展示）
        QHBoxLayout *topLayout = new QHBoxLayout();
        mainLayout->addLayout(topLayout, 50); // 60%高度

        // 下层布局（辅助分析）
        QHBoxLayout *bottomLayout = new QHBoxLayout();
        mainLayout->addLayout(bottomLayout, 50); // 40%高度

        //-------------------------------------------
        // 2. 上层布局 - 核心数据展示
        //-------------------------------------------
        // 左侧：全国热力分布（地图）
        QGroupBox *geoMapBox = new QGroupBox("全国热力分布", centralWidget);
        QVBoxLayout *geoLayout = new QVBoxLayout(geoMapBox);
        geoLayout->addWidget(ui->photolabel); // 假设这是地图组件
        ui->photolabel->setScaledContents(true);
        topLayout->addWidget(geoMapBox, 60); // 60%宽度

        // 右侧：实时数据流分析（饼图）
        QGroupBox *realtimePieBox = new QGroupBox("实时数据流分析", centralWidget);
        QVBoxLayout *pieLayout = new QVBoxLayout(realtimePieBox);
        pieLayout->addWidget(chartView);
        topLayout->addWidget(realtimePieBox, 40); // 40%宽度

        //3.下层

        // 左侧：预测趋势（折线图）
        QGroupBox *trendBox = new QGroupBox("预测趋势", centralWidget);
        QVBoxLayout *trendLayout = new QVBoxLayout(trendBox);
        trendLayout->setContentsMargins(0, 0, 0, 0);
        trendLayout->setSpacing(0);
        lineChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        trendLayout->addWidget(lineChartView);
        bottomLayout->addWidget(trendBox, 20); // 30%宽度

        // 中间：维度对比（横向柱状图）
        QGroupBox *compareBox = new QGroupBox("维度对比", centralWidget);
        QVBoxLayout *compareLayout = new QVBoxLayout(compareBox);
        compareLayout->setContentsMargins(0, 0, 0, 0);
        compareLayout->setSpacing(0);
        HorizontalbarChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        compareLayout->addWidget(HorizontalbarChartView);
        bottomLayout->addWidget(compareBox, 20); // 20%宽度

        // 右侧：分类统计（柱状图）
        QGroupBox *categoryBox = new QGroupBox("分类统计", centralWidget);
        QVBoxLayout *categoryLayout = new QVBoxLayout(categoryBox);
        categoryLayout->setContentsMargins(0, 0, 0, 0);
        categoryLayout->setSpacing(0);
        barChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        categoryLayout->addWidget(barChartView);
        bottomLayout->addWidget(categoryBox, 30); // 20%宽度

        // 新增：散点图
        QGroupBox *scatterBox = new QGroupBox("能源消耗分布", centralWidget);
        QVBoxLayout *scatterLayout = new QVBoxLayout(scatterBox);
        scatterLayout->setContentsMargins(0, 0, 0, 0);
        scatterLayout->setSpacing(0);
        if(scatterChartView&&scatterChartView->chart())
        {scatterChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            scatterLayout->addWidget(scatterChartView);}
        bottomLayout->addWidget(scatterBox, 30); // 30%宽度

        //-------------------------------------------
        // 4. 表格布局 - 实时数据矩阵
        //-------------------------------------------
        // 表格放置在右侧，与饼图并列
        QGroupBox *dataMatrixBox = new QGroupBox("实时数据矩阵", centralWidget);
        QVBoxLayout *matrixLayout = new QVBoxLayout(dataMatrixBox);
        matrixLayout->addWidget(ui->tableWidget);
        topLayout->addWidget(dataMatrixBox, 40); // 40%宽度
}

//初始化页面
void MainWindow::init_layout() {

    QWidget *oldCentralWidget = this->centralWidget();
    if (oldCentralWidget) {
        oldCentralWidget->deleteLater(); // 延迟删除
    }

    // 创建中心部件并设置主布局
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    //-------------------------------------------
    // 1. 高级科技感样式表
    //-------------------------------------------
    QString styleSheet = R"(
        /* 基础全局样式 */
        QWidget {
            background-color: #0A0F2B; /* 深空蓝背景 */
            font-family: "Segoe UI", "微软雅黑";
        }

        /* 分组框 - 全息边框效果 */
        QGroupBox {
            border: 2px solid qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #00F7FF, stop:0.5 #7B61FF, stop:1 #FF00F7); /* 霓虹渐变边框 */
            border-radius: 8px;
            margin-top: 20px;
            padding: 15px;
            background-color: rgba(16, 22, 54, 0.9); /* 半透明深蓝 */
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            color: #00F7FF; /* 青色标题 */
            font-size: 16px;
            font-weight: bold;
            text-transform: uppercase; /* 英文大写 */
        }

        /* 表格 - 赛博朋克风格 */
        QTableView {
            background-color: rgba(11, 17, 43, 0.8);
            alternate-background-color: rgba(23, 30, 66, 0.6);
            color: #E0E0FF;
            border: 1px solid #3A3F6B;
            selection-background-color: #7B61FF;
        }
        QHeaderView::section {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2A2F5C, stop:1 #1A1F4C);
            color: #7B61FF;
            border: 1px solid #4A4F7B;
            padding: 6px;
            font-weight: bold;
        }

        /* 悬浮提示特效 */
        QToolTip {
            background-color: #1A1F4C;
            color: #00F7FF;
            border: 1px solid #7B61FF;
            border-radius: 4px;
        }
    )";
    centralWidget->setStyleSheet(styleSheet);

    //-------------------------------------------
    // 2. 主布局结构（三列黄金分割）
    //-------------------------------------------
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15); // 整体边距

    // 左侧分析区（35%宽度）
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(20);
    mainLayout->addLayout(leftLayout, 30);

    // 中间核心区（50%宽度）
    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->setSpacing(20);
    mainLayout->addLayout(centerLayout, 34);

    // 右侧详情区（15%宽度）
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(20);
    mainLayout->addLayout(rightLayout, 36);

    //-------------------------------------------
    // 3. 左侧分析区 - 实时数据流
    //-------------------------------------------
    // 动态饼图（带光泽效果）
    QGroupBox *realtimePieBox = new QGroupBox("实时数据流分析", centralWidget);
    QVBoxLayout *pieLayout = new QVBoxLayout(realtimePieBox);
    pieLayout->addWidget(chartView);
    leftLayout->addWidget(realtimePieBox);

    // 趋势预测图（3D折线）
    QGroupBox *trendBox = new QGroupBox("预测趋势", centralWidget);
    QVBoxLayout *trendLayout = new QVBoxLayout(trendBox);
    trendLayout->addWidget(lineChartView);
    leftLayout->addWidget(trendBox);

    //-------------------------------------------
    // 4. 中间核心区 - 数据中枢
    //-------------------------------------------
    // 全屏地图可视化
    QGroupBox *geoMapBox = new QGroupBox("全国热力分布", centralWidget);
    QVBoxLayout *geoLayout = new QVBoxLayout(geoMapBox);
    geoLayout->addWidget(ui->photolabel); // 假设这是地图组件
    ui->photolabel->setScaledContents(true);
    centerLayout->addWidget(geoMapBox, 60); // 60%高度

    // 动态数据矩阵
    QGroupBox *dataMatrixBox = new QGroupBox("实时数据矩阵", centralWidget);
    QVBoxLayout *matrixLayout = new QVBoxLayout(dataMatrixBox);
    matrixLayout->addWidget(ui->tableWidget);
    centerLayout->addWidget(dataMatrixBox, 40); // 40%高度

    //-------------------------------------------
    // 5. 右侧详情区 - 数据维度
    //-------------------------------------------
    // 横向对比分析
    QGroupBox *compareBox = new QGroupBox("维度对比", centralWidget);
    QVBoxLayout *compareLayout = new QVBoxLayout(compareBox);
    compareLayout->addWidget(HorizontalbarChartView);
    rightLayout->addWidget(compareBox, 60); // 60%高度

    // 分类统计
    QGroupBox *categoryBox = new QGroupBox("分类统计", centralWidget);
    QVBoxLayout *categoryLayout = new QVBoxLayout(categoryBox);
    categoryLayout->addWidget(barChartView);
    rightLayout->addWidget(categoryBox, 40); // 40%高度

    //-------------------------------------------
    // 6. 高级图表渲染（需要Qt 5.12+）
    //-------------------------------------------
    QList<QChartView*> allCharts = { chartView, lineChartView, barChartView, HorizontalbarChartView };
    for (auto chartView : allCharts) {
        // 启用抗锯齿和透明背景
        chartView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        chartView->chart()->setBackgroundBrush(Qt::NoBrush);

        // 统一霓虹主题
        chartView->chart()->setTheme(QChart::ChartThemeDark);

        // 动态光晕标题
        QFont titleFont("Roboto Mono", 14, QFont::Bold);
        titleFont.setLetterSpacing(QFont::AbsoluteSpacing, 2);
        chartView->chart()->setTitleFont(titleFont);
        chartView->chart()->setTitleBrush(QBrush(QColor("#7B61FF")));
    }

    // 特殊处理饼图的光晕效果
    if (auto pieSeries = qobject_cast<QPieSeries*>(chartView->chart()->series().first())) {
        pieSeries->setPieSize(0.8); // 留出空间显示光晕
        for (auto slice : pieSeries->slices()) {
            slice->setBorderColor(QColor(123, 97, 255, 150)); // 紫色光晕
            slice->setBorderWidth(2);
        }
    }
}

//初始化菜单栏
void MainWindow::init_menubar() {
    //-------------------------------------------
    // 1. 菜单栏全局样式（关键修改部分）
    //-------------------------------------------
    QString menuStyle = R"(
        /* 菜单栏基础样式 */
        QMenuBar {
            background-color: #0A0F2B;        /* 深空蓝背景 */
            border-bottom: 2px solid #2A2F5C; /* 底部装饰线 */
            padding: 2px;
        }

        /* 菜单项常规状态 */
        QMenuBar::item {
            color: #7B61FF;                   /* 科技紫文字 */
            background: transparent;
            padding: 4px 12px;
            border-radius: 4px;
            margin: 2px;
        }

        /* 菜单项悬浮状态 */
        QMenuBar::item:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2A2F5C, stop:1 #1A1F4C);
            border: 1px solid #7B61FF;
        }

        /* 菜单项选中状态 */
        QMenuBar::item:selected {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3A3F7C, stop:1 #2A2F6C);
        }

        /* 下拉菜单样式 */
        QMenu {
            background-color: #1A1F4C;        /* 暗蓝色背景 */
            border: 2px solid qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00F7FF, stop:1 #7B61FF); /* 霓虹渐变边框 */
            color: #E0E0FF;
            padding: 5px;
        }

        /* 菜单项样式 */
        QMenu::item {
            padding: 6px 30px 6px 20px;       /* 增加右侧留空 */
            border-radius: 3px;
            min-width: 120px;
        }

        /* 菜单项悬浮效果 */
        QMenu::item:hover {
            background-color: #2A2F5C;
            color: #00F7FF;
        }

        /* 菜单项选中效果 */
        QMenu::item:selected {
            background-color: #3A3F7C;
        }

        /* 分隔符样式 */
        QMenu::separator {
            height: 1px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00F7FF, stop:1 #7B61FF);
            margin: 4px 8px;
        }
    )";
    ui->menuBar->setStyleSheet(menuStyle);


    //-------------------------------------------
    //4. 确保字体可见性
    //-------------------------------------------
    QFont menuFont("Microsoft YaHei", 10);
    menuFont.setWeight(QFont::Medium);  // 中等粗细提升可读性
    ui->menuBar->setFont(menuFont);
}

QString& MainWindow::get_ip()
{
    return ip;
}

//断开连接时
void MainWindow::disconnet()
{
    if(this->type!=0)
        QMessageBox::information(this,"","服务器连接异常");
}

//连接出错时返回
void MainWindow::connect_error(QAbstractSocket::SocketError error)
{
    QString worning="错误代码：\n";
    int a=error;
    char c = '0'+a;
    worning += c;
    QMessageBox::information(this,"连接失败",worning);
}

//获取登录信息
void MainWindow::get_logintext(QString &s,QString password,QString ip)
{
    //sql#login#password#...
    QString message="sql#login#";
    ui->ipline->setText(ip);
    this->ip=ip;
    socket->abort();
    socket->connectToHost(QHostAddress(ip),port.toShort());
    message += password+"#";
    message += s;
    this->socket->write(message.toUtf8());
}

//接收修改信息，转发至服务器
void MainWindow::recieve_alter(QString message)
{
    this->socket->write(message.toUtf8());
}

//获取注册信息,S是账号#密码或者check
void MainWindow::get_enrolltext(QString &sql,QString s,QString ip)
{
    QString message="sql#enroll#";
    message += s + "#";
    message += sql;
    if(this->type==0)
    {
        ui->ipline->setText(ip);
        this->ip=ip;
        socket->abort();
        socket->connectToHost(QHostAddress(ip),port.toShort());
    }
    //sql#enroll#check#sql语句
    //sql#enroll#账号#密码#sql语句
    this->socket->write(message.toUtf8());
    // qDebug() << message;
}

// //初始化用户信息
// void MainWindow::init_user()
// {
//     QString message="init#";
//     message+=username;
//     this->socket->write(message.toUtf8());
// }

//连接服务器
void MainWindow::on_linkButton_clicked()
{
    //断开连接，防止报错
    socket->abort();
    socket->connectToHost(QHostAddress(ui->ipline->text()),ui->portline->text().toShort());
}

//接收服务器信息
void MainWindow::recieve_message()
{
    QString message = socket->readAll();
    qDebug() << message;
    QString chat;
    QStringList sl=message.split('#');
    if(sl[0]=="login")//login#00
    {
        emit login(sl[1]);
        if(sl[1]=="01")
        {
            this->type=1;//修改为默认模式
            // 显示图表
            chartView->show();
            lineChartView->show();
            barChartView->show();
            HorizontalbarChartView->show();
            init_layout();
            init_menubar();
        }
    }
    else if(sl[0]=="enroll")
    {
        emit enroll(sl[1]);
    }
    else if(sl[0]=="init"||sl[0]=="alter")
    {
        this->username=sl[1];
    }
}

//初始化表格
void MainWindow::set_table(const QList<QStringList> &data)
{
    if (data.isEmpty()) {
        qWarning() << "数据为空，无法设置表格";
        return;
    }

    // 获取表格控件
    QTableWidget *table = ui->tableWidget;

    // 清除现有内容
    table->clear();

    // 设置行列数（行数=数据条数，列数=第一条数据的字段数）
    table->setRowCount(data.size());
    table->setColumnCount(data.first().size());

    // 设置水平表头（能源指标名称）
    QStringList headers = {
        // 基础信息
        "年份", "ISO代码", "人口", "GDP(美元)",

        // 生物燃料相关
        "生物燃料消费变化百分比", "生物燃料消费变化(太瓦时)", "人均生物燃料消费量",
        "生物燃料消费总量", "人均生物燃料发电量", "生物燃料发电总量",
        "生物燃料发电占比", "生物燃料能源占比",

        // 电力与碳排放
        "电力碳强度",

        // 煤炭相关
        "煤炭消费变化百分比", "煤炭消费变化(太瓦时)", "人均煤炭消费量",
        "煤炭消费总量", "人均煤炭发电量", "煤炭发电总量",
        "煤炭产量变化百分比", "煤炭产量变化(太瓦时)", "人均煤炭产量",
        "煤炭总产量", "煤炭发电占比", "煤炭能源占比",

        // 电力相关
        "电力需求", "发电总量", "电力能源占比",

        // 能源消费总体
        "能源消费变化百分比", "能源消费变化(太瓦时)", "人均能源消费量",
        "单位GDP能耗",

        // 化石燃料
        "化石燃料消费变化百分比", "化石燃料消费变化(太瓦时)", "人均化石燃料发电量",
        "化石燃料发电总量", "人均化石燃料能源消费量", "化石燃料消费总量",
        "化石燃料发电占比", "化石燃料能源占比",

        // 天然气
        "天然气消费变化百分比", "天然气消费变化(太瓦时)", "天然气消费总量",
        "人均天然气发电量", "天然气发电总量", "人均天然气能源消费量",
        "天然气产量变化百分比", "天然气产量变化(太瓦时)", "人均天然气产量",
        "天然气总产量", "天然气发电占比", "天然气能源占比",

        // 温室气体
        "温室气体排放量",

        // 水电
        "水电消费变化百分比", "水电消费变化(太瓦时)", "水电消费总量",
        "人均水电发电量", "水电发电总量", "人均水电能源消费量",
        "水电发电占比", "水电能源占比",

        // 低碳能源
        "低碳能源消费变化百分比", "低碳能源消费变化(太瓦时)", "低碳能源消费总量",
        "人均低碳能源发电量", "低碳能源发电总量", "人均低碳能源消费量",
        "低碳能源发电占比", "低碳能源占比",

        // 电力贸易
        "电力净进口量", "净进口电力占需求比例",

        // 核能
        "核能消费变化百分比", "核能消费变化(太瓦时)", "核能消费总量",
        "人均核能发电量", "核能发电总量", "人均核能能源消费量",
        "核能发电占比", "核能能源占比",

        // 石油
        "石油消费变化百分比", "石油消费变化(太瓦时)", "石油消费总量",
        "人均石油发电量", "石油发电总量", "人均石油能源消费量",
        "石油产量变化百分比", "石油产量变化(太瓦时)", "人均石油产量",
        "石油总产量", "石油发电占比", "石油能源占比",

        // 其他可再生能源
        "其他可再生能源消费量", "其他可再生能源发电量",
        "非生物燃料的其他可再生能源发电量", "其他可再生能源消费变化百分比",
        "其他可再生能源消费变化(太瓦时)", "人均其他可再生能源发电量",
        "人均非生物燃料的其他可再生能源发电量", "人均其他可再生能源消费量",
        "其他可再生能源发电占比", "非生物燃料的其他可再生能源发电占比",
        "其他可再生能源能源占比",

        // 电力与能源消费
        "人均电力消费量", "一次能源消费总量",

        // 可再生能源总体
        "可再生能源消费变化百分比", "可再生能源消费变化(太瓦时)", "可再生能源消费总量",
        "人均可再生能源发电量", "可再生能源发电总量", "人均可再生能源消费量",
        "可再生能源发电占比", "可再生能源能源占比",

        // 太阳能
        "太阳能消费变化百分比", "太阳能消费变化(太瓦时)", "太阳能消费总量",
        "人均太阳能发电量", "太阳能发电总量", "人均太阳能能源消费量",
        "太阳能发电占比", "太阳能能源占比",

        // 风能
        "风能消费变化百分比", "风能消费变化(太瓦时)", "风能消费总量",
        "人均风能发电量", "风能发电总量", "人均风能能源消费量",
        "风能发电占比", "风能能源占比"
    };

    table->setHorizontalHeaderLabels(headers);


    // 填充数据
    // 倒序填充数据（从最后一行开始）
    for (int srcRow = 0; srcRow < data.size(); ++srcRow) {
        int dstRow = data.size() - 1 - srcRow; // 倒序计算目标行
        const QStringList &rowData = data[srcRow];

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);

            // 数值列右对齐
            if (col >= 5) { // 从第6列开始认为是数值数据
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            }

            table->setItem(dstRow, col, item);
        }
    }

    // 冻结前3列（年份、人口、GDP）
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    table->setColumnWidth(0, 80);  // 年份列宽
    table->setColumnWidth(1, 120); // 人口列宽
    table->setColumnWidth(2, 150); // GDP列宽
}


//统一接口
    QStringList MainWindow::getYearData(const QList<QStringList>& allData, int targetYear)
    {
        for (const QStringList& row : allData) {
            if (row.size() > 1 && row[0].toInt() == targetYear) { // 第2列是年份
                return row;
            }
        }
        return QStringList();
    }

    //横向柱状图
    QChart* MainWindow::createHorizontalBarChart(const QList<QStringList>& allData)
    {
        // 1. 验证年份选择
        if (this->type < 0 || this->type > 22) {
            qWarning() << "无效年份类型（0-22对应2000-2022）";
            return nullptr;
        }
        int targetYear = 2000 + this->type;

        // 2. 提取目标年份数据
        QStringList yearData;
        for (const QStringList& row : data) {
            if (row.size() > 1 && row[0].toInt() == targetYear) {
                yearData = row;
                break;
            }
        }
        if (yearData.isEmpty()) {
            qWarning() << "未找到" << targetYear << "年数据";
            return nullptr;
        }

        // 3. 定义能源组分（名称、数据列索引、颜色、原始单位）
        struct EnergyComponent {
            QString name;
            int dataIndex;
            QColor color;
            QString unit; // 原始单位
        };

        const QList<EnergyComponent> components = {
            {"煤炭",   15, QColor("#e6194b"), "万吨"},
            {"石油",   87, QColor("#3cb44b"), "万桶"},
            {"天然气", 41, QColor("#ffe119"), "亿立方米"},
            {"水电",   57, QColor("#4363d8"), ""},
            {"核能",   72, QColor("#f58231"), ""},
            {"太阳能", 100, QColor("#911eb4"), ""},
            {"风能",   120, QColor("#46f0f0"), ""}
        };

        // //生活方式、肥胖、精神、药物或化学物质、遗传、自身免疫、病毒感染
        // const QList<EnergyComponent> components = {
        //     {"生活方式",   15, QColor("#e6194b"), ""},
        //     {"肥胖",   87, QColor("#3cb44b"), ""},
        //     {"精神", 41, QColor("#ffe119"), ""},
        //     {"药物或化学物质",   57, QColor("#4363d8"), ""},
        //     {"遗传",   72, QColor("#f58231"), ""},
        //     {"自身免疫", 100, QColor("#911eb4"), ""},
        //     {"病毒感染",   120, QColor("#46f0f0"), ""}
        // };


        // 4. 数据无量纲化（Min-Max归一化）
        QVector<double> rawValues;
        for (const auto& comp : components) {
            if (comp.dataIndex < yearData.size()) {
                rawValues.append(yearData[comp.dataIndex].toDouble());
            }
        }

        if (rawValues.isEmpty()) return nullptr;

        // 计算归一化值（0-1范围）
        auto minMax = std::minmax_element(rawValues.begin(), rawValues.end());
        double range = *minMax.second - *minMax.first;
        QVector<double> normalizedValues;
        for (double val : rawValues) {
            normalizedValues.append(range > 0 ? (val - *minMax.first) / range : 0.5);
        }

        // 5. 创建横向柱状图系列
        QHorizontalBarSeries* series = new QHorizontalBarSeries();
        series->setBarWidth(0.6);

        QBarSet* set = new QBarSet("消费量(无量纲化)");
        //QBarSet* set = new QBarSet("患病指标(无量纲化)");

        // 6. 添加数据（带自定义标签）
        for (int i = 0; i < components.size(); ++i) {
            if (components[i].dataIndex >= yearData.size()) continue;
            *set << normalizedValues[i];
            set->setColor(components[i].color);
            // 自定义标签显示原始值和单位
            //set->setLabel(QString("%1 %2").arg(rawValues[i]).arg(components[i].unit));
        }
        series->append(set);

        // 7. 创建图表
        QChart* chart = new QChart();
        chart->addSeries(series);
        //chart->setTitle(QString("%1年能源组分对比（归一化值）").arg(targetYear));
        chart->setTitle(QString("%1年患病因素对比（归一化值）").arg(targetYear));
        chart->setAnimationOptions(QChart::SeriesAnimations);

        // 8. 坐标轴设置
        QBarCategoryAxis* axisY = new QBarCategoryAxis();
        QStringList categories;
        for (const auto& comp : components) {
            categories << comp.name;
        }
        axisY->append(categories);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        QValueAxis* axisX = new QValueAxis();
        axisX->setTitleText("归一化值（0-1）");
        axisX->setRange(0, 1.1); // 留出10%空白
        axisX->setTickCount(6);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        // 9. 标签样式
        series->setLabelsVisible(true);
        series->setLabelsPosition(QBarSeries::LabelsOutsideEnd);
        series->setLabelsFormat(""); // 显示自定义标签

        // 10. 图例说明
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);

        chart->legend()->setVisible(false); // 确保图例可见
        return chart;
    }


//柱状图
    QChart* MainWindow::createBarChart(const QList<QStringList>& allData)
    {
        // 验证type范围（0=2000, 22=2022）
        if (this->type < 0 || this->type > 22) {
            qWarning() << "无效的年份类型，应在0-22范围内";
            return nullptr;
        }

        int targetYear = 2000 + this->type;
        QStringList yearData = getYearData(allData, targetYear);
        if (yearData.isEmpty()) {
            qWarning() << "未找到" << targetYear << "年数据";
            return nullptr;
        }

        // 能源类型定义（名称、数据列索引、颜色）
        const QList<EnergyType> energyTypes = {
            {"煤炭", 15, QColor("#e6194b")},
            {"石油", 87, QColor("#3cb44b")},
            {"天然气", 41, QColor("#ffe119")},
            {"水力发电", 57, QColor("#4363d8")},
            {"太阳能", 123, QColor("#911eb4")}
        };

        QBarSeries* series = new QBarSeries();
        series->setBarWidth(0.7);
        QBarSet* set = new QBarSet("能源消费量");

        // 添加数据
        for (const EnergyType& energy : energyTypes) {
            if (energy.index < yearData.size()) {
                bool ok;
                double value = yearData[energy.index].toDouble(&ok);
                if (ok) {
                    *set << value;
                    set->setColor(energy.color);
                }
            }
        }
        series->append(set);

        QChart* chart = new QChart();
        chart->addSeries(series);
        chart->setTitle(QString("%1年能源消费量（太瓦时）").arg(targetYear));

        // X轴（能源类型）
        QBarCategoryAxis* axisX = new QBarCategoryAxis();
        QStringList categories;
        for (const EnergyType& energy : energyTypes) {
            categories << energy.name;
        }
        // categories << "生活方式" << "肥胖" << "精神" << "遗传" << "自身免疫";
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        // Y轴（数值）
        QValueAxis* axisY = new QValueAxis();
        //axisY->setTitleText("消费量（太瓦时）");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        series->setLabelsVisible(true);
        series->setLabelsFormat("@value");
        chart->setAnimationOptions(QChart::AllAnimations); // 启用动画
        return chart;
    }


//折线统计图
QChart* MainWindow::createLineChart(const QList<QStringList> &data) {
    QMap<QString, double> *totalEnergyByYear = new QMap<QString, double>; // 键是年份，值是总能源消耗量

    for (const QStringList &row : data) {
        if(row[1]=="year") continue;
        if (row.size() > 11) {
            QString year = row[0];
            double totalEnergy = 0;//当年消耗值之和
            // 从第3列开始累加能源消耗量
            for (int i = 10; i < row.size(); ++i) {
                if (!row[i].isEmpty())
                    totalEnergy += row[i].toDouble();
            }
            (*totalEnergyByYear)[year] = totalEnergy;
        }
    }
    // 创建折线图系列
    QLineSeries *series = new QLineSeries();
    series->setName("能源消耗总量");
    // series->setName("患病人数（百）");

    // 添加数据点（年份, 总能源消耗量）
    for (auto it = totalEnergyByYear->begin(); it != totalEnergyByYear->end(); ++it) {
        series->append(it.key().toDouble(), it.value());
    }

    // 创建图表
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::AllAnimations); // 启用动画
    chart->setTitle("能耗趋势总结");
    // chart->setTitle("患病人数趋势总结");

    // 创建坐标轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Year");
    axisX->setLabelFormat("%d"); // 年份为整数
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("能源消耗量");
    chart->addAxis(axisY, Qt::AlignLeft);

    // 将系列附加到坐标轴
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    return chart;
}
//低碳能源
QChart* MainWindow::createLowCarbonTrendChart(const QList<QStringList>& data)
{
    // 1. 准备数据容器
    QVector<double> years;
    QMap<QString, QVector<double>> energyData = {
        {"水电", {}},
        {"核能", {}},
        {"太阳能", {}},
        {"风能", {}},
        {"生物燃料", {}}
    };

    // 2. 填充数据（假设列索引：水电=57, 核能=79, 太阳能=123, 风能=131, 生物燃料=7）
    for (const QStringList& row : data) {
        if (row.size() > 1) {
            bool ok;
            int year = row[0].toInt(&ok);
            if (ok && year >= 2000 && year <= 2022) {
                years.append(year);
                energyData["水电"].append(row[56].toDouble());
                energyData["核能"].append(row[78].toDouble());
                energyData["太阳能"].append(row[122].toDouble());
                energyData["风能"].append(row[123].toDouble());
                energyData["生物燃料"].append(row[6].toDouble());
            }
        }
    }

    if (years.isEmpty()) {
        qWarning() << "无有效数据";
        return nullptr;
    }

    // 3. 创建折线图系列
    QChart* chart = new QChart();
    chart->setTitle("低碳能源消费趋势（2000-2022）");

    // 颜色定义（与饼图保持一致）
    const QMap<QString, QColor> colors = {
        {"水电", QColor("#4363d8")},
        {"核能", QColor("#f58231")},
        {"太阳能", QColor("#911eb4")},
        {"风能", QColor("#46f0f0")},
        {"生物燃料", QColor("#f032e6")}
    };

    // 4. 添加各能源系列
    for (const QString& key : energyData.keys()) {
        QLineSeries* series = new QLineSeries();
        series->setName(key);
        series->setColor(colors[key]);

        for (int i = 0; i < years.size(); ++i) {
            series->append(years[i], energyData[key][i]);
        }

        chart->addSeries(series);
    }

    // 5. 坐标轴设置
    QValueAxis* axisX = new QValueAxis();
    axisX->setTitleText("年份");
    axisX->setRange(2000, 2022);
    axisX->setTickCount(years.size() / 5 + 1);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("消费量（太瓦时）");
    chart->addAxis(axisY, Qt::AlignLeft);

    // 绑定坐标轴
    for (QAbstractSeries* series : chart->series()) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    // 6. 图例和交互设置
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    return chart;
}
//可再生能源
QChart* MainWindow::createRenewableEnergyTrendChart(const QList<QStringList>& data)
{
    // 1. 准备历史数据（2015-2022）
    QVector<double> years, hydroData, solarData, windData, totalData;
    for (const QStringList &row : data) {
        if (row.size() > 1) {
            bool ok;
            int year = row[0].toInt(&ok);
            if (ok && year >= 2015 && year <= 2022) {
                years.append(year);

                // 可再生能源数据列（示例索引，需根据实际数据调整）
                hydroData.append(row[56].toDouble());   // 水电
                solarData.append(row[122].toDouble());  // 太阳能
                windData.append(row[123].toDouble());   // 风能
                totalData.append(hydroData.last() + solarData.last() + windData.last());
            }
        }
    }

    if (years.isEmpty()) {
        qWarning() << "历史数据不足";
        return nullptr;
    }

    // 2. 数据预测（预测2023-2025）
    auto linearRegression = [](const QVector<double>& x, const QVector<double>& y) {
        double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
        int n = x.size();
        for (int i = 0; i < n; ++i) {
            sumX += x[i];
            sumY += y[i];
            sumXY += x[i] * y[i];
            sumXX += x[i] * x[i];
        }
        double slope = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
        double intercept = (sumY - slope * sumX) / n;
        return [=](double x) { return slope * x + intercept; };
    };

    auto predictTotal = linearRegression(years, totalData);

    // 3. 创建系列
    QLineSeries *hydroSeries = new QLineSeries();
    QLineSeries *solarSeries = new QLineSeries();
    QLineSeries *windSeries = new QLineSeries();
    QLineSeries *totalSeries = new QLineSeries();
    QScatterSeries *predSeries = new QScatterSeries();

    // 4. 样式设置
    hydroSeries->setName("水电");
    hydroSeries->setColor(QColor("#4363d8"));  // 蓝色
    solarSeries->setName("太阳能");
    solarSeries->setColor(QColor("#f58231"));  // 橙色
    windSeries->setName("风能");
    windSeries->setColor(QColor("#46f0f0"));   // 青色
    totalSeries->setName("可再生能源总量");
    totalSeries->setColor(Qt::darkGreen);
    totalSeries->setPen(QPen(Qt::darkGreen, 2, Qt::DashLine));

    predSeries->setName("预测值");
    predSeries->setColor(Qt::red);
    predSeries->setMarkerSize(10);

    // 5. 填充数据
    for (int i = 0; i < years.size(); ++i) {
        hydroSeries->append(years[i], hydroData[i]);
        solarSeries->append(years[i], solarData[i]);
        windSeries->append(years[i], windData[i]);
        totalSeries->append(years[i], totalData[i]);
    }

    // 添加预测点（2023-2025）
    for (int year = 2023; year <= 2025; ++year) {
        double predValue = predictTotal(year);
        predSeries->append(year, predValue);
        totalSeries->append(year, predValue);  // 延长趋势线
    }

    // 6. 创建图表
    QChart *chart = new QChart();
    chart->addSeries(hydroSeries);
    chart->addSeries(solarSeries);
    chart->addSeries(windSeries);
    chart->addSeries(totalSeries);
    chart->addSeries(predSeries);
    chart->setTitle("可再生能源消费趋势（2015-2025）");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // 7. 坐标轴设置
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("年份");
    axisX->setRange(2014.5, 2025.5);
    axisX->setTickCount(5);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("消费量（亿千瓦时）");
    axisY->setRange(1000, 3500);
    chart->addAxis(axisY, Qt::AlignLeft);

    // 绑定坐标轴
    for (QAbstractSeries *series : chart->series()) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    // 8. 图例与交互
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    //悬停显示数值
    connect(predSeries, &QScatterSeries::hovered, [](const QPointF &point, bool state) {
        if (state) QToolTip::showText(QCursor::pos(), QString("预测: %1 亿千瓦时").arg(point.y()));
    });

    return chart;
}

//调用python函数
double getPythonPrediction(const QVector<double>& last5YearsData) {
    QProcess python;
    python.start("python", QStringList() << "predict_energy.py"
                                         << QString::number(last5YearsData[0]) );
    python.waitForFinished();
    return python.readAllStandardOutput().toDouble();
}

//创建饼状图
QChart* MainWindow::createPieChart(const QList<QStringList> &data)
{
    if (data.isEmpty()) {
        qWarning() << "数据为空，无法创建饼图";
        return nullptr;
    }

    if(this->type==0) this->type=22;

    // 检查type范围(0-22对应2000-2022)
    int targetYear = 2000 + this->type;
    if (this->type < 0 || this->type > 22) {
        qWarning() << "无效的type值，应在0-22范围内";
        return nullptr;
    }

    // 查找对应年份的数据
    QStringList yearData;
    bool yearFound = false;
    for (const QStringList &row : data) {
        if (row.size() > 0 && row[0].toInt() == targetYear) {
            yearData = row;
            yearFound = true;
            break;
        }
    }

    if (!yearFound) {
        qWarning() << "未找到" << targetYear << "年的数据";
        return nullptr;
    }

    // 创建饼图系列
    QPieSeries *series = new QPieSeries();
    series->setPieSize(0.7);

    // 能源类型及其在数据中的索引位置
    struct EnergyType {
        QString name;
        int index;  // 在数据中的列索引
        QColor color;
    };

    const QList<EnergyType> energyTypes = {
        {"煤炭", 15, QColor("#e6194b")},      // 煤炭能源占比
        {"石油", 87, QColor("#3cb44b")},      // 石油能源占比
        {"天然气", 41, QColor("#ffe119")},    // 天然气能源占比
        {"水力发电", 57, QColor("#4363d8")},      // 水电能源占比
        {"核能", 79, QColor("#f58231")},     // 核能能源占比
        {"太阳能", 123, QColor("#911eb4")},    // 太阳能能源占比
        {"风能", 131, QColor("#46f0f0")},      // 风能能源占比
        {"生物燃料", 7, QColor("#f032e6")}    // 生物燃料能源占比
    };

    // const QList<EnergyType> energyTypes = {
    //     {">50", 15, QColor("#e6194b")},      // 煤炭能源占比
    //     {"46-50", 87, QColor("#3cb44b")},      // 石油能源占比
    //     {"41-45", 41, QColor("#ffe119")},    // 天然气能源占比
    //     {"36-40", 57, QColor("#4363d8")},      // 水电能源占比
    //     {"31-35", 79, QColor("#f58231")},     // 核能能源占比
    //     {"20-30", 123, QColor("#911eb4")},    // 太阳能能源占比
    //     {"11-20", 131, QColor("#46f0f0")},      // 风能能源占比
    //     {"0-10", 7, QColor("#f032e6")}    // 生物燃料能源占比
    // };

    // 添加各能源切片
    for (const EnergyType &energy : energyTypes) {
        if (energy.index < yearData.size()) {
            bool ok;
            double value = yearData[energy.index].toDouble(&ok);
            if (ok && value > 0) {
                QPieSlice *slice = series->append(energy.name, value);
                slice->setLabelVisible(false);//是否显示旁边的小字
                slice->setLabelPosition(QPieSlice::LabelOutside);
                slice->setLabel(QString("%1\n").arg(energy.name));
                slice->setBrush(energy.color);

                // 当占比过小时，调整标签位置避免重叠
                if (value < 5) {
                    slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
                }
            }
        }
    }

    // 创建图表
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("%1年能源消费结构").arg(targetYear));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // 设置图例样式
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setFont(QFont("Microsoft YaHei", 8));

    // 添加占比总和标注
    double totalPercent = 0;
    for (QPieSlice *slice : series->slices()) {
        totalPercent += slice->percentage() * 100;
    }

    if (totalPercent < 99.9) {
        QPieSlice *otherSlice = series->append("其他", 100 - totalPercent);
        otherSlice->setColor(QColor("#bcbcbc"));
        otherSlice->setLabelVisible();
        otherSlice->setLabel("其他\n" + QString::number(100 - totalPercent, 'f', 1) + "%");
    }

    return chart;
}

//散点图
QChart* MainWindow::createScatterChart(const QList<QStringList> &data)
{
    // 1. 准备历史数据（2015-2022）
    QVector<double> years, coalData, oilData, gasData;
    for (const QStringList &row : data) {
        if (row.size() > 1) {
            bool ok;
            int year = row[0].toInt(&ok);
            if (ok && year >= 2015 && year <= 2022) { // 使用8年数据训练
                years.append(year);
                coalData.append(row[14].toDouble());
                oilData.append(row[86].toDouble());
                gasData.append(row[40].toDouble());
            }
        }
    }

    if (years.isEmpty()) {
        qWarning() << "历史数据不足";
        return nullptr;
    }

    // 2. 线性回归预测
    auto linearRegression = [](const QVector<double>& x, const QVector<double>& y) {
        double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
        int n = x.size();
        for (int i = 0; i < n; ++i) {
            sumX += x[i];
            sumY += y[i];
            sumXY += x[i] * y[i];
            sumXX += x[i] * x[i];
        }
        double slope = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
        double intercept = (sumY - slope * sumX) / n;
        return [=](double x) { return slope * x + intercept; };
    };

    // 3. 训练模型
    auto coalPredict = linearRegression(years, coalData);
    auto oilPredict = linearRegression(years, oilData);
    auto gasPredict = linearRegression(years, gasData);

    // 4. 创建系列
    QScatterSeries *histCoal = new QScatterSeries();
    QScatterSeries *histOil = new QScatterSeries();
    QScatterSeries *histGas = new QScatterSeries();
    QScatterSeries *predCoal = new QScatterSeries();
    QScatterSeries *predOil = new QScatterSeries();
    QScatterSeries *predGas = new QScatterSeries();

    // 5. 样式设置
    histCoal->setName("煤炭(历史)");
    histCoal->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    histCoal->setColor(QColor("#e6194b"));
    histCoal->setMarkerSize(12);

    predCoal->setName("煤炭(预测)");
    predCoal->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    predCoal->setColor(QColor("#e6194b"));
    predCoal->setMarkerSize(12);
    predCoal->setBorderColor(Qt::black); // 预测点加边框

    // 石油和天然气同理设置...
    histOil->setName("");
    histOil->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    histOil->setColor(QColor("#e6194b"));
    histOil->setMarkerSize(12);

    predOil->setName("天然气(预测)");
    predOil->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    predOil->setColor(QColor("#e6194b"));
    predOil->setMarkerSize(12);
    predOil->setBorderColor(Qt::black); // 预测点加边框

    histGas->setName("");
    histGas->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    histGas->setColor(QColor("#e6194b"));
    histGas->setMarkerSize(12);

    predGas->setName("石油(预测)");
    predGas->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    predGas->setColor(QColor("#e6194b"));
    predGas->setMarkerSize(12);
    predGas->setBorderColor(Qt::black); // 预测点加边框

    // 6. 填充数据
    for (int i = 0; i < years.size(); ++i) {
        histCoal->append(years[i], coalData[i]);
        histOil->append(years[i], oilData[i]);
        histGas->append(years[i], gasData[i]);
    }

    // 添加预测点（2023-2025）
    for (int year = 2023; year <= 2025; ++year) {
        predCoal->append(year, coalPredict(year));
        predOil->append(year, oilPredict(year));
        predGas->append(year, gasPredict(year));
    }

    // 7. 创建图表
    QChart *chart = new QChart();
    chart->addSeries(histCoal);
    chart->addSeries(histOil);
    chart->addSeries(histGas);
    chart->addSeries(predCoal);
    chart->addSeries(predOil);
    chart->addSeries(predGas);
    chart->setTitle("化石燃料消费量趋势与预测 (2016-2025)");
    // chart->setTitle("糖尿病患者人数趋势预测");

    // 8. 坐标轴设置
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("年份");
    axisX->setRange(2016.0, 2025.0);
    axisX->setTickCount(7);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("消费量（太瓦时）");
    axisY->setRange(15000.0, 18750.0);
    chart->addAxis(axisY, Qt::AlignLeft);

    // 绑定坐标轴
    for (QAbstractSeries *s : chart->series()) {
        s->attachAxis(axisX);
        s->attachAxis(axisY);
    }

    // 9. 添加趋势线
    auto addTrendLine = [chart](QScatterSeries *src, QColor color) {
        QLineSeries *trend = new QLineSeries();
        trend->setColor(color);
        trend->setName(src->name() + "趋势");
        for (auto p : src->points()) trend->append(p);
        chart->addSeries(trend);
        //trend->attachAxis(chart->axisX());
        //trend->attachAxis(chart->axisY());
    };

    addTrendLine(histCoal, QColor("#e6194b"));
    addTrendLine(histOil, QColor("#3cb44b"));
    addTrendLine(histGas, QColor("#ffe119"));

    return chart;
}


//读取csv文件
QList<QStringList> MainWindow::readCSV(const QString& filePath,
                                       int startRow,
                                       int endRow)
{
    QList<QStringList> result;

    QFile file(filePath);

    if(country==1) {startRow=147;endRow=269;}

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "无法打开文件:" << filePath;
        return result;
    }

    QTextStream in(&file);
    int currentRow = 0;

    // 读取文件直到找到起始行
    while (!in.atEnd() && currentRow < startRow - 1)
    {
        in.readLine();
        currentRow++;
    }

    // 读取目标行范围
    while (!in.atEnd() && currentRow <= endRow - 1)
    {
        currentRow++;
        QString line = in.readLine();
        QStringList fields = line.split(",");

        // 检查是否为中国的数据
        if (fields.size() > 0)
        {
            QStringList record;

            // 添加基础信息
            record << fields[1]; // year
            record << fields[3]; // population
            record << fields[4]; // gdp

            // 添加能源数据 (从第5列开始)
            for (int i = 5; i < fields.size(); ++i)
            {
                QString value = fields[i].trimmed();
                record << (value.isEmpty() ? "NA" : value);
            }

            result.append(record);
        }
    }
    file.close();
    return result;
}
