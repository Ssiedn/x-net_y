#include "adminwindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QWidget>
#include <QGridLayout>
#include <QListWidget>
#include <QLabel>
#include <QDir>
#include <QFileInfoList>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QSpacerItem>
AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //标题
    setWindowTitle("管理员界面");
    setWindowIcon(QPixmap("://UI_simulation/Resources/system.png"));
    QString toolstyleSheet =
        "QToolBar {"
        "    background: #ECE9E9;"   // 工具栏背景色
        "    border-radius: 5px;" // 圆角边框
        "}"
        "QToolButton {"
        "    background: #ECE9E9;"   // 工具按钮背景色
        "    padding: 5px;" // 按钮内边距
        "}"
        "QToolButton:pressed {"
        "    background: #E0E0E0;"   // 按钮被按下时的背景色
        "}"
        "QToolBar::separator {"
        "   background: darkgray;"
        "   width: 1px;"  /* 设置宽度来加粗分隔符 */
        "}";
    // 创建工具栏
    toolBar = addToolBar("模型下载工具栏");
    toolBar->setIconSize(QSize(25,25));
    toolBar->setStyleSheet(toolstyleSheet);
    //创建“模型导入”动作
    QAction *importModelAction = new QAction(QIcon("://UI_simulation/Resources/import.png"), "模型导入", this);
    toolBar->addAction(importModelAction);
    connect(importModelAction, &QAction::triggered, this, [=]() {
        QWidget *currentPage = stackedWidget->currentWidget();

        // 比较并输出当前页面的信息
        if (currentPage == stackedWidget->widget(0)) {
            uploadModel(terrainSaveDirectory, "选择地形模型文件", "terrainList");
        } else if (currentPage == stackedWidget->widget(1)) {
            uploadModel(nodeSaveDirectory, "选择节点模型文件", "nodeList");
        }
    });

    //创建“模型删除动作”
    QAction *deleteModelAction = new QAction(QIcon("://UI_simulation/Resources/delete.png"), "模型删除", this);
    toolBar->addAction(deleteModelAction);
    connect(deleteModelAction, &QAction::triggered, this, [=]() {
        // 获取当前显示的页面
        QWidget *currentPage = stackedWidget->currentWidget();

        // 比较并输出当前页面的信息
        if (currentPage == stackedWidget->widget(0)) {
            deleteModel(terrainSaveDirectory, current_fileName, "terrainList");
        } else if (currentPage == stackedWidget->widget(1)) {
            deleteModel(nodeSaveDirectory, current_fileName, "nodeList");
        }

    });

    // 创建中央部件及布局
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // 创建 QAction
    QAction *terrainAction = new QAction(QIcon("://UI_simulation/Resources/terrain.png"),"地形模型区", this);
    QAction *nodeAction = new QAction(QIcon("://UI_simulation/Resources/node.png"),"节点模型区", this);
    QAction *taskAction = new QAction(QIcon(":/UI_simulation/Resources/business.png"),"任务模型区", this);
    QAction *businessAction = new QAction(QIcon("://UI_simulation/Resources/task.png"),"业务模型区", this);
    // 将 QAction 添加到工具栏
    QToolBar *sidebar = new QToolBar(this);
    sidebar->setIconSize(QSize(40,50));
    // 设置工具栏方向为垂直
    sidebar->setOrientation(Qt::Vertical);
    // 设置工具栏的显示模式以包含文本
    sidebar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    // 添加指定高度的空白间隔
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setFixedHeight(25);  // 设置间隔高度为25像素
    sidebar->addWidget(spacerWidget);

    sidebar->addAction(terrainAction);
    sidebar->addAction(nodeAction);
    sidebar->addAction(taskAction);
    sidebar->addAction(businessAction);
    // 设置工具栏样式
    sidebar->setStyleSheet("QToolBar { border: 0px solid rgb(150, 150, 150); }");

    QString titlestyle = "font-size: 18px; font-weight: bold;";
    QString style = "font-size: 15px;";
    // 创建区域容器
    stackedWidget = new QStackedWidget(this);
    // 创建不同的区域
    terrainWidget = new QWidget(this);
    nodeWidget = new QWidget(this);
    taskWidget = new QWidget(this);
    businessWidget = new QWidget(this);

    QGridLayout *terrainLayout = new QGridLayout(terrainWidget);
    terrainLayout->setContentsMargins(0, 0, 0, 0);  // 设置左、上、右、下边距
    QLabel *terrainLabel = new QLabel("地形模型区", this);
    terrainLabel->setStyleSheet(titlestyle);
    QListWidget *terrainList = new QListWidget(this);
    terrainList->setStyleSheet(style);
    terrainList->setObjectName("terrainList");
    terrainLayout->addWidget(terrainLabel, 0, 0);
    terrainLayout->addWidget(terrainList, 1, 0);
    // 设置布局边缘的空白
    terrainLayout->setContentsMargins(0, 0, 0, 0); // 去掉布局边缘的空白
    terrainLayout->setSpacing(5); // 设置控件间距
    terrainWidget->setLayout(terrainLayout);
    populateList(terrainList, terrainSaveDirectory, "*.obj");

    QGridLayout *nodeLayout = new QGridLayout(nodeWidget);
    nodeLayout->setContentsMargins(0, 0, 0, 0);  // 设置左、上、右、下边距
    QLabel *nodeLabel = new QLabel("节点模型区", this);
    nodeLabel->setStyleSheet(titlestyle);
    QListWidget *nodeList = new QListWidget(this);
    nodeList->setStyleSheet(style);
    nodeList->setObjectName("nodeList");
    nodeLayout->addWidget(nodeLabel, 0, 0);
    nodeLayout->addWidget(nodeList, 1, 0);
    // 设置布局边缘的空白
    nodeLayout->setContentsMargins(0, 0, 0, 0); // 去掉布局边缘的空白
    nodeLayout->setSpacing(5); // 设置控件间距
    nodeWidget->setLayout(nodeLayout);
    populateList(nodeList, nodeSaveDirectory, "*.obj");

    QGridLayout *taskLayout = new QGridLayout(taskWidget);
    taskLayout->setContentsMargins(0, 0, 0, 0);  // 设置左、上、右、下边距
    QLabel *taskLabel = new QLabel("任务模型区", this);
    taskLabel->setStyleSheet(titlestyle);
    QListWidget *taskList = new QListWidget(this);
    taskList->setStyleSheet(style);
    taskLayout->addWidget(taskLabel, 0, 0);
    taskLayout->addWidget(taskList, 1, 0);
    // 设置布局边缘的空白
    taskLayout->setContentsMargins(0, 0, 0, 0); // 去掉布局边缘的空白
    taskLayout->setSpacing(5); // 设置控件间距
    taskWidget->setLayout(taskLayout);

    QGridLayout *businessLayout = new QGridLayout(businessWidget);
    businessLayout->setContentsMargins(0, 0, 0, 0);  // 设置左、上、右、下边距
    QLabel *businessLabel = new QLabel("业务模型区", this);
    businessLabel->setStyleSheet(titlestyle);
    QListWidget *businessList = new QListWidget(this);
    businessList->setStyleSheet(style);
    businessLayout->addWidget(businessLabel, 0, 0);
    businessLayout->addWidget(businessList, 1, 0);
    // 设置布局边缘的空白
    businessLayout->setContentsMargins(0, 0, 0, 0); // 去掉布局边缘的空白
    businessLayout->setSpacing(5); // 设置控件间距
    businessWidget->setLayout(businessLayout);

    stackedWidget->addWidget(terrainWidget);
    stackedWidget->addWidget(nodeWidget);
    stackedWidget->addWidget(taskWidget);
    stackedWidget->addWidget(businessWidget);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stackedWidget);
    centralWidget->setLayout(mainLayout);

    // 连接按钮与槽函数
    connect(terrainAction, &QAction::triggered, this, &AdminWindow::switchToTerrain);
    connect(nodeAction, &QAction::triggered, this, &AdminWindow::switchToNode);
    connect(taskAction, &QAction::triggered, this, &AdminWindow::switchToTask);
    connect(businessAction, &QAction::triggered, this, &AdminWindow::switchToBusiness);
    // 点击数据
    connect(terrainList, &QListWidget::itemClicked, this, &AdminWindow::handleItemClicked);
    connect(terrainList, &QListWidget::itemDoubleClicked, this, &AdminWindow::handleItemDoubleClicked);
    connect(nodeList, &QListWidget::itemClicked, this, &AdminWindow::handleItemClicked);
    connect(nodeList, &QListWidget::itemDoubleClicked, this, &AdminWindow::handleItemDoubleClicked);
}
//边栏切换
void AdminWindow::switchToTerrain() {
    stackedWidget->setCurrentWidget(terrainWidget);
}
void AdminWindow::switchToNode() {
    stackedWidget->setCurrentWidget(nodeWidget);
}
void AdminWindow::switchToTask() {
    stackedWidget->setCurrentWidget(taskWidget);
}
void AdminWindow::switchToBusiness() {
    stackedWidget->setCurrentWidget(businessWidget);
}

//单击显示模型名称
void AdminWindow::handleItemClicked(QListWidgetItem *item) {
    if (item) {
        QString text = item->text();

        // 查找第一个点的位置，点前面是序号和点，后面是文件名
        int dotIndex = text.indexOf('.');

        if (dotIndex != -1) {
            // 从点的位置开始提取文件名
            current_fileName = text.mid(dotIndex + 2); // 点后面加上空格的长度
            // 处理文件名，比如进行进一步操作
            qDebug() << "File Name:" << current_fileName;
        } else {
            // 如果没有找到点，则认为文件名格式有误
            qDebug() << "Invalid item format:" << text;
        }
    }
}
//双击打开模型
void AdminWindow::handleItemDoubleClicked(QListWidgetItem *item) {
    // 获取点击的项的文本
    QString text = item->text();

    // 查找第一个点的位置，点前面是序号和点，后面是文件名
    int dotIndex = text.indexOf('.');

    if (dotIndex != -1) {
        // 从点的位置开始提取文件名
        current_fileName = text.mid(dotIndex + 2);
        //确定数据区域
        QWidget *currentPage = stackedWidget->currentWidget();
        if (currentPage == stackedWidget->widget(0)) {
            openModel(terrainSaveDirectory, current_fileName, "terrainList");
        } else if (currentPage == stackedWidget->widget(1)) {
            openModel(nodeSaveDirectory, current_fileName, "nodeList");
        }
    } else {
        qDebug() << "Invalid item format:" << text;
    }
}
//显示模型列表
void AdminWindow::populateList(QListWidget *listWidget, const QString &directory, const QString &filter) {
    listWidget->clear();  // 清空现有项
    QDir dir(directory);  // 创建目录对象
    QStringList files = dir.entryList(QStringList() << filter, QDir::Files);  // 获取符合过滤条件的文件列表

    int index = 1;  // 初始化索引
    for (const QString &file : files) {
        // 使用 QString::arg 格式化文件名和索引
        QString formattedItem = QString("%1. %2").arg(index++).arg(file);
        listWidget->addItem(formattedItem);  // 将格式化后的项添加到 QListWidget 中
    }
}
//上传模型
void AdminWindow::uploadModel(const QString &destinationDir, const QString &dialogTitle,const QString &areaTitle) {
    // 打开文件对话框，只允许选择 .obj 文件
    QString fileName = QFileDialog::getOpenFileName(this,  dialogTitle, "", "OBJ Files (*.obj)");

    if (!fileName.isEmpty()) {
        // 创建保存目录，如果不存在的话
        QDir dir(destinationDir);
        if (!dir.exists()) {
            dir.mkpath(destinationDir);
        }

        // 构建保存路径
        QString destinationFilePath = destinationDir + "/" + QFileInfo(fileName).fileName();

        // 复制文件到目标目录
        if (QFile::copy(fileName, destinationFilePath)) {
            qDebug() << "文件已成功保存到:" << destinationFilePath;
            // 假设上传成功后重新加载地形模型列表
            updateModelList(destinationDir,areaTitle);
        } else {
            qDebug() << "文件保存失败";
        }
    }
}
//更新模型区
void AdminWindow::updateModelList(const QString &destinationDir,const QString &areaTitle) {
    QListWidget *List = findChild<QListWidget *>(areaTitle); // 获取地形模型区的列表
    if(List){
        populateList(List, destinationDir, "*.obj");
    }
}
//删除模型
void AdminWindow::deleteModel(const QString &destinationDir, const QString &filename,const QString &areaTitle){
    // 使用 QDir 处理路径拼接
    QDir dir(destinationDir);
    QString filePath = dir.filePath(filename);

    if (QFile::remove(filePath)) {
        qDebug() << "文件删除成功。";
        // 假设删除成功后重新加载地形模型列表
        updateModelList(destinationDir,areaTitle);
    } else {
        qDebug() << "文件删除失败。";
    }
}
//打开模型
void AdminWindow::openModel(const QString &destinationDir, const QString &filename,const QString &areaTitle){
    // 使用 QDir 处理路径拼接
    QDir dir(destinationDir);
    QString filePath = dir.filePath(filename);

    // 确保文件存在
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        // 使用默认程序打开文件
        QUrl fileUrl = QUrl::fromLocalFile(filePath);
        QDesktopServices::openUrl(fileUrl);
    } else {
        qWarning("File does not exist: %s", qPrintable(filePath));
    }
}
