#include "userwindow.h"
#include "configwindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QAction>
#include <QApplication>
#include <QPushButton>
#include <QDockWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QRegularExpression>
#include <QFormLayout>
#include <QLineEdit>
#include <QFileInfo>
#include <QDateTime>
#include <QTimer>
#include <QStackedWidget>
#include <QProcess>
UserWindow::UserWindow()
{
    //标题
    setWindowTitle("仿真主界面");
    setWindowIcon(QPixmap("://UI_simulation/Resources/system.png"));
    // 创建菜单栏
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    // 设置样式表
    QString styleSheet =
        "QMenuBar {"
        "    background-color: #0B6897;" // 背景颜色
        "    color: white;"           // 字体颜色
        "    font-size: 15px;"        // 字体大小
        "    font-family: Arial;"     // 字体类型
        "}"
        "QMenuBar::item {"
        "    background-color: #0B6897;" // 菜单项背景颜色
        "    padding: 5px 10px;"      // 菜单项内边距
        "}"
        "QMenuBar::item:selected {"
        "    background-color: #86BAE5;" // 选中项背景颜色
        "}";
    QString dockstyleSheet =
        "QDockWidget::title {"
        "   background-color: #E8F2FF;"
        "   padding: 5px;"
        "   border: 1px solid #A0C4FF;"  // 添加边框
        "}";

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
    QString wordstyle = "font-size: 14px;";
    menuBar->setStyleSheet(styleSheet);
    // 创建菜单
    QMenu *configMenu = menuBar->addMenu("配置");
    QMenu *dataMenu = menuBar->addMenu("数据");
    QMenu *statusMenu = menuBar->addMenu("监控");
    QMenu *windowMenu = menuBar->addMenu("窗口");
    // 添加动作到菜单
    QAction *networkConfigAction = new QAction("网络配置", this);
    configMenu->addAction(networkConfigAction);
    configMenu->addAction(new QAction("链路配置", this));
    configMenu->addAction(new QAction("业务配置", this));

    dataMenu->addAction(new QAction("数据存储", this));
    dataMenu->addAction(new QAction("数据读取", this));

    QAction *nodestatusAction = new QAction("运行状态", this);
    statusMenu->addAction(nodestatusAction);
    QAction *linkstatusAction = new QAction("链接状态", this);
    statusMenu->addAction(linkstatusAction);

    QAction *infowindowAction = new QAction("信息窗口", this);
    windowMenu->addAction(infowindowAction);
    QMenu *propertywindowMenu = new QMenu("属性窗口", this);
    windowMenu->addMenu(propertywindowMenu);
    QAction *nodeswindowAction = new QAction("节点窗口", this);
    propertywindowMenu->addAction(nodeswindowAction);
    QAction *linkswindowAction = new QAction("链路窗口", this);
    propertywindowMenu->addAction(linkswindowAction);
    QAction *statuswindowAction = new QAction("状态窗口", this);
    windowMenu->addAction(statuswindowAction);
    QAction *terminalwindowAction = new QAction("终端窗口", this);
    windowMenu->addAction(terminalwindowAction);
    // 连接信号和槽
    connect(networkConfigAction, &QAction::triggered, this, &UserWindow::showNetworkConfigWindow);

    // 创建工具栏
    toolBar = addToolBar("仿真工具栏");
    toolBar->setIconSize(QSize(20,20));
    toolBar->setStyleSheet(toolstyleSheet);
    QIcon activeStartIcon("://UI_simulation/Resources/run.png");
    QIcon inactiveStartIcon("://UI_simulation/Resources/run_h.png");
    QIcon activeStopIcon("://UI_simulation/Resources/complete.png");
    QIcon inactiveStopIcon("://UI_simulation/Resources/complete_h.png");
    QIcon terminalIcon("://UI_simulation/Resources/terminal.png");
    //创建动作
    QAction *startsimulation = new QAction(activeStartIcon, "", toolBar);
    QAction *stopsimulation = new QAction(inactiveStopIcon, "", toolBar);
    QAction *terminalAction = new QAction(terminalIcon, "", toolBar); // 新的终端动作
    toolBar->addAction(startsimulation);
    toolBar->addAction(stopsimulation);
    toolBar->addSeparator(); // 添加分隔符
    toolBar->addAction(terminalAction); // 添加终端动作
    // 设置工具提示
    startsimulation->setToolTip("开始仿真");
    stopsimulation->setToolTip("停止仿真");
    terminalAction->setToolTip("终端窗口");
    // 连接信号和槽
    QObject::connect(startsimulation, &QAction::triggered, [=]() {
        startsimulation->setEnabled(false);
        startsimulation->setIcon(inactiveStartIcon);
        stopsimulation->setEnabled(true);
        stopsimulation->setIcon(activeStopIcon);
    });

    QObject::connect(stopsimulation, &QAction::triggered, [=]() {
        stopsimulation->setEnabled(false);
        stopsimulation->setIcon(inactiveStopIcon);
        startsimulation->setEnabled(true);
        startsimulation->setIcon(activeStartIcon);
    });
    QObject::connect(terminalAction, &QAction::triggered, [=]() {
        toggleDockWindow(terminalDock);
    });

    // 创建中央内容区域
    //QTextEdit *pageContent = new QTextEdit("场景显示", this);
    // 创建DockWidget作为中央内容区域
    QDockWidget *centralDock = new QDockWidget("仿真场景", this);
    centralDock->setStyleSheet(dockstyleSheet);
    //centralDock->setWidget(pageContent);
    // 将DockWidget添加到主窗口中
    setCentralWidget(centralDock);

    // 创建数据边栏作为DockWidget
    dataDock = new QDockWidget("仿真网络数据", this);
    dataDock->setStyleSheet(dockstyleSheet);
    // 创建主容器Widget
    auto *container = new QWidget();
    auto *layout = new QVBoxLayout(container);
    // 调整主容器的内边距和边距
    layout->setContentsMargins(1, 0, 0, 0);  // 左、上、右、下
    // 创建树状列表
    auto *tree = new QTreeWidget();
    tree->setStyleSheet(wordstyle);
    tree->setHeaderLabel("网络信息");
    //创建节点、链路信息
    populateTree(tree);
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, [this, tree]() { checkForUpdates(tree); });
    updateTimer->start(5000);  // 每5秒检查一次
    // 连接树状列表的点击事件
    connect(tree, &QTreeWidget::itemClicked, this, &UserWindow::onItemClicked);
    // 将树状列表添加到布局中
    layout->addWidget(tree);
    // 设置主容器为DockWidget的内容
    container->setLayout(layout);
    dataDock->setWidget(container);

    // 将DockWidget添加到主窗口
    addDockWidget(Qt::LeftDockWidgetArea, dataDock);

    // 创建节点属性显示的DockWidget
    nodeAttributesDock = new QDockWidget("节点属性", this);
    nodeAttributesDock->setStyleSheet(dockstyleSheet);
    nodeAttributesDock->hide();
    nodeAttributesContent = new QWidget(this);
    nodeAttributesContent->setStyleSheet("background-color: #EEEEEE;");//设置背景色
    nodeAttributesLayout = new QFormLayout(nodeAttributesContent);
    nodeAttributesLayout->setSpacing(5); // 设置行间隔为0像素
    nodeAttributesLayout->setContentsMargins(0, 0, 5, 0); // 设置边距为0像素
    nodeAttributesContent->setLayout(nodeAttributesLayout); // 设置布局
    nodeAttributesDock->setWidget(nodeAttributesContent);
    addDockWidget(Qt::RightDockWidgetArea, nodeAttributesDock);
    // 创建链路属性显示的DockWidget
    linkAttributesDock = new QDockWidget("链路属性", this);
    linkAttributesDock->setStyleSheet(dockstyleSheet);
    linkAttributesDock->hide();
    linkAttributesContent = new QWidget(this);
    linkAttributesContent->setStyleSheet("background-color: #EEEEEE;");
    linkAttributesLayout = new QFormLayout(linkAttributesContent);
    linkAttributesLayout->setSpacing(5); // 设置行间隔为0像素
    linkAttributesLayout->setContentsMargins(0, 0, 5, 0); // 设置边距为0像素
    linkAttributesContent->setLayout(linkAttributesLayout); // 设置布局
    linkAttributesDock->setWidget(linkAttributesContent);
    addDockWidget(Qt::RightDockWidgetArea, linkAttributesDock);
    // 初始化节点和链路属性内容
    updateNodeAttributesWidget(QJsonObject()); // Empty object initially
    updateLinkAttributesWidget(QJsonObject()); // Empty object initially

    // 创建状态栏作为DockWidget
    statusDock = new QDockWidget("状态栏", this);
    statusDock->setStyleSheet(dockstyleSheet);
    QWidget *statusContent = new QWidget(this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusContent);
    statusDisplay = new QTextEdit(this);
    statusLayout->setContentsMargins(1, 0, 5, 5);  // 设置左、上、右、下边距
    statusDisplay->setReadOnly(true);
    statusDisplay->setText("状态信息");
    statusLayout->addWidget(statusDisplay);
    statusContent->setLayout(statusLayout);
    statusDock->setWidget(statusContent);
    // 设置 QDockWidget 的最大高度（示例: 最大高度为200）
    statusDock->setMaximumHeight(300);

    // 创建终端栏作为DockWidget
    terminalDock = new QDockWidget("终端栏", this);
    terminalDock->setStyleSheet(dockstyleSheet);
    terminalDock->hide();
    // 创建内容区域
    QWidget *terminalContent = new QWidget(this);
    // 创建垂直布局
    QVBoxLayout *terminalLayout = new QVBoxLayout(terminalContent);
    terminalLayout->setContentsMargins(1, 0, 5, 0);  // 设置左、上、右、下边距
    // 创建大框用于显示提示信息
    infoDisplay = new QTextEdit(this);
    infoDisplay->setReadOnly(true); // 使其只读，防止用户修改
    // 设置初始提示信息
    infoDisplay->setText("命令行");
    terminalLayout->addWidget(infoDisplay);
    // 创建一个水平布局用于输入框和按钮
    QHBoxLayout *inputLayout = new QHBoxLayout();
    QLineEdit *terminalinputField = new QLineEdit(this);
    inputLayout->addWidget(terminalinputField);
    QPushButton *sendButton = new QPushButton("Send", this);
    inputLayout->addWidget(sendButton);
    QPushButton *clearButton = new QPushButton("Clear", this);
    inputLayout->addWidget(clearButton);
    // 将水平布局添加到内容区域的垂直布局中
    terminalLayout->addLayout(inputLayout);
    // 添加一个空的间距项到垂直布局底部，以保持输入框和按钮距离底部5像素
    terminalLayout->addItem(new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Fixed));
    // 设置内容区域的布局
    terminalContent->setLayout(terminalLayout);
    // 设置内容区域到 QDockWidget
    terminalDock->setWidget(terminalContent);
    terminalDock->setMaximumHeight(300);
    // 设置底部区域
    setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::AllowNestedDocks);
    addDockWidget(Qt::BottomDockWidgetArea, statusDock);
    addDockWidget(Qt::BottomDockWidgetArea, terminalDock);

    // 将两个DockWidget合并成标签页
    tabifyDockWidget(statusDock, terminalDock);

    // 连接信号和槽
    connect(infowindowAction, &QAction::triggered, [this]() { toggleDockWindow(dataDock); });
    connect(nodeswindowAction, &QAction::triggered, [this]() { toggleDockWindow(nodeAttributesDock); });
    connect(linkswindowAction, &QAction::triggered, [this]() { toggleDockWindow(linkAttributesDock); });
    connect(statuswindowAction, &QAction::triggered, [this]() { toggleDockWindow(statusDock); });
    connect(terminalwindowAction, &QAction::triggered, [this]() { toggleDockWindow(terminalDock); });
    // 连接清除按钮的点击信号到槽函数
    connect(clearButton, &QPushButton::clicked, [this, terminalinputField]() { clearInputFields(terminalinputField);});
}
//读取文件
// 读取 JSON 文件并返回 QJsonObject
QJsonObject loadJsonFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file.");
        return QJsonObject();
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    return doc.object();
}
//检查数据文件更新
void UserWindow::checkForUpdates(QTreeWidget *tree) {
    QFileInfo currentNodesFileInfo(nodesFilePath);
    QFileInfo currentLinksFileInfo(linksFilePath);

    if (currentNodesFileInfo.lastModified() != nodesFileInfo.lastModified() ||
        currentLinksFileInfo.lastModified() != linksFileInfo.lastModified())
    {
        nodesFileInfo = currentNodesFileInfo;
        linksFileInfo = currentLinksFileInfo;
        // Update the tree widget
        if (tree) {
            populateTree(tree);
            nodeAttributesDock->hide();
            linkAttributesDock->hide();
        }
    }
}
//更新信息窗口
void UserWindow::populateTree(QTreeWidget *tree) {
    // 读取数据
    QJsonObject nodes = loadJsonFromFile(nodesFilePath);
    QJsonObject links = loadJsonFromFile(linksFilePath);

    // 清空现有项
    tree->clear();
    // 设置标题样式
    QFont boldFont;
    boldFont.setBold(true);
    // 创建节点信息树状项
    auto *nodeItem = new QTreeWidgetItem({"节点"});
    nodeItem->setFont(0, boldFont);  // 设置“节点信息”标题加粗
    QJsonArray nodeArray = nodes["nodes"].toArray();
    if(!nodeArray.isEmpty()){
        for (const QJsonValue &nodeValue : nodeArray) {
            QJsonObject nodeObj = nodeValue.toObject();
            QString nodeName = nodeObj["name"].toString();
            auto *node = new QTreeWidgetItem({nodeName});
            nodeItem->addChild(node);
        }
    }

    // 创建链路信息树状项
    auto *linkItem = new QTreeWidgetItem({"链路"});
    linkItem->setFont(0, boldFont);  // 设置“链路信息”标题加粗
    QJsonArray linkArray = links["links"].toArray();
    if(!linkArray.isEmpty()){
        for (const QJsonValue &linkValue : linkArray) {
            QJsonObject linkObj = linkValue.toObject();
            int source = linkObj["source"].toInt();
            int target = linkObj["target"].toInt();
            QString linkName = QString("n%1 to n%2").arg(source).arg(target);
            auto *link = new QTreeWidgetItem({linkName});
            linkItem->addChild(link);
        }
    }

    // 将节点信息和链路信息添加到树中
    tree->addTopLevelItem(nodeItem);
    tree->addTopLevelItem(linkItem);
    // 展开所有顶级项及其子项
    expandAllItems(tree);
}
//展开所有项
void UserWindow::expandAllItems(QTreeWidget *tree)
{
    // 递归展开所有项
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        item->setExpanded(true);
    }
}
//点击事件
void UserWindow::onItemClicked(QTreeWidgetItem *item, int column) {
    QString itemName = item->text(column);

    // 读取数据
    QJsonObject nodes = loadJsonFromFile(nodesFilePath);
    QJsonObject links = loadJsonFromFile(linksFilePath);

    if (item->parent() && item->parent()->text(0) == "节点") {
        nodeAttributesDock->show();
        handleNodeClick(itemName, nodes);
    } else if (item->parent() && item->parent()->text(0) == "链路") {
        linkAttributesDock->show();
        handleLinkClick(itemName, links);
    }
}
//读取属性
void UserWindow::handleNodeClick(const QString& itemName, const QJsonObject& nodes) {
    QJsonArray nodeArray = nodes["nodes"].toArray();
    for (const QJsonValue &nodeValue : nodeArray) {
        QJsonObject nodeObj = nodeValue.toObject();
        if (nodeObj["name"].toString() == itemName) {
            updateNodeAttributesWidget(nodeObj);
            return; // 找到匹配的节点后可以直接返回
        }
    }
    qWarning() << "节点信息未找到：" << itemName;
}
void UserWindow::handleLinkClick(const QString& itemName, const QJsonObject& links) {
    // 使用 QRegularExpression 解析 itemName
    QRegularExpression regex("(n(\\d+)) to (n(\\d+))");
    QRegularExpressionMatch match = regex.match(itemName);

    if (!match.hasMatch()) {
        qWarning() << "链路信息名称格式错误：" << itemName;
        return;
    }

    bool okSource, okTarget;
    int desiredSource = match.captured(2).toInt(&okSource);
    int desiredTarget = match.captured(4).toInt(&okTarget);

    if (!okSource || !okTarget) {
        qWarning() << "链路信息名称格式错误或无法解析 source/target：" << itemName;
        return;
    }

    QJsonArray linkArray = links["links"].toArray();
    for (const QJsonValue &linkValue : linkArray) {
        QJsonObject linkObj = linkValue.toObject();
        int source = linkObj["source"].toInt();
        int target = linkObj["target"].toInt();

        if (source == desiredSource && target == desiredTarget) {
            updateLinkAttributesWidget(linkObj);
            return; // 找到匹配的链路后可以直接返回
        }
    }
    qWarning() << "链路信息未找到：" << itemName;
}
//更新属性表
void UserWindow::updateNodeAttributesWidget(const QJsonObject &attributes) {
    // 清空之前的内容
    QLayoutItem *item;
    while ((item = nodeAttributesLayout->takeAt(0))) {
        delete item->widget();
        delete item;
    }
    // 设置字体样式
    QFont font("Arial", 11); // 设定字体和大小
    QFont labelFont("Arial", 11, QFont::Bold); // 标签字体为加粗
    // 更新内容
    for (const QString &key : attributes.keys()) {
        QJsonValue jsonValue = attributes.value(key);

        QString value;
        if (jsonValue.isArray()) { // 判断是否为数组类型
            QJsonArray jsonArray = jsonValue.toArray();
            QStringList valueList;
            for (const QJsonValue &arrayValue : jsonArray) {
                valueList.append(arrayValue.toString());
            }
            value = valueList.join(", ");
        } else if (jsonValue.isDouble()) { // 判断是否为数值类型
            double doubleValue = jsonValue.toDouble();
            if (doubleValue == static_cast<int>(doubleValue)) {
                // 值是整数
                value = QString::number(static_cast<int>(doubleValue));
            } else {
                // 值是浮点数
                value = QString::number(doubleValue, 'f', 6); // 'f' 表示浮点数，6 表示小数点后保留6位
            }
        } else {
            value = jsonValue.toString();
        }
        QLabel *label = new QLabel(QString("<b>%1</b>").arg(key), nodeAttributesContent);
        label->setFont(labelFont); // 应用标签字体样式
        QLineEdit *valueField = new QLineEdit(value, nodeAttributesContent);
        valueField->setFont(font); // 应用输入框字体样式
        valueField->setReadOnly(true); // 设置为只读
        nodeAttributesLayout->addRow(label, valueField); // 使用 addRow 添加行

    }
}

void UserWindow::updateLinkAttributesWidget(const QJsonObject &attributes) {
    // 清空之前的内容
    QLayoutItem *item;
    while ((item = linkAttributesLayout->takeAt(0))) {
        delete item->widget();
        delete item;
    }
    // 设置字体样式
    QFont font("Arial", 11); // 设定字体和大小
    QFont labelFont("Arial", 11, QFont::Bold); // 标签字体为加粗
    // 更新内容
    for (auto key : attributes.keys()) {
        QString value;
        QJsonValue jsonValue = attributes.value(key);

        if (jsonValue.isDouble()) {
            double doubleValue = jsonValue.toDouble();
            if (doubleValue == static_cast<int>(doubleValue)) {
                // 如果值是整数，转换为整数字符串
                value = QString::number(static_cast<int>(doubleValue));
            } else {
                // 如果值是浮点数，转换为字符串
                value = QString::number(doubleValue, 'f', 6);
            }
        } else {
            // 处理其他类型的值（例如字符串）
            value = jsonValue.toString();
        }

        QLabel *label = new QLabel(QString("<b>%1</b>").arg(key), linkAttributesContent);
        label->setFont(labelFont); // 应用标签字体样式
        QLineEdit *valueField = new QLineEdit(value, linkAttributesContent);
        valueField->setFont(font); // 应用输入框字体样式
        valueField->setReadOnly(true); // 设置为只读
        linkAttributesLayout->addRow(label, valueField); // 使用 addRow 添加行
    }
}
//配置按键
void UserWindow::showNetworkConfigWindow() {
    ConfigWindow *networkConfigWindow = new ConfigWindow(this);
    networkConfigWindow->exec(); // 使用 exec() 显示为模态对话框
}
//打开窗口
void UserWindow::toggleDockWindow(QDockWidget *dockWidget) {
    if (dockWidget->isVisible()) {
        dockWidget->hide();
    } else {
        dockWidget->show();
    }
}

//清理命令输入框
void UserWindow::clearInputFields(QLineEdit *inputField) {
    // 假设 inputField 是你的输入框对象
    inputField->clear();
}
