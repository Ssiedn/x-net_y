#include "configwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDir>
#include <QFileInfoList>
#include <QProcess>
#include <QMessageBox>
ConfigWindow::ConfigWindow(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("网络配置");

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // 地图模型部分
    QHBoxLayout *mapModelLayout = new QHBoxLayout;
    QLabel *mapModelLabel = new QLabel("地图模型:", this);
    mapModelComboBox = new QComboBox(this);
    // 设置字体大小
    setFontSize(mapModelLabel, 13);
    setFontSize(mapModelComboBox, 13);
    // 读取文件夹中的.obj文件
    QDir dir(terrainSaveDirectory);
    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.obj", QDir::Files);

    for (const QFileInfo &fileInfo : fileList) {
        mapModelComboBox->addItem(fileInfo.baseName()); // 添加文件名，不包括扩展名
    }

    mapModelLayout->addWidget(mapModelLabel);
    QWidget *spacer = new QWidget();
    spacer->setFixedSize(5, 0); // 设置固定宽度
    mapModelLayout->addWidget(spacer); // 添加空白控件
    mapModelLayout->addWidget(mapModelComboBox,1,Qt::AlignLeft);
    mapModelLayout->setContentsMargins(0, 0, 0, 0); // 去掉布局边缘的空白
    mapModelLayout->setSpacing(0); // 去掉控件间距

    // 网络类型部分
    QHBoxLayout *networkTypeLayout = new QHBoxLayout;
    QLabel *networkTypeLabel = new QLabel("网络类型:", this);
    networkTypeComboBox = new QComboBox(this);
    // 设置字体大小
    setFontSize(networkTypeLabel, 13);
    setFontSize(networkTypeComboBox, 13);
    networkTypeComboBox->addItem("随机网络");
    networkTypeComboBox->addItem("幂律网络");
    networkTypeComboBox->addItem("星形网络");
    networkTypeComboBox->addItem("规则网络");
    networkTypeComboBox->addItem("网格网络");
    networkTypeComboBox->addItem("线性网络");
    // 创建水平布局
    networkTypeLayout->addWidget(networkTypeLabel);
    networkTypeLayout->addWidget(spacer); // 添加空白控件
    networkTypeLayout->addWidget(networkTypeComboBox,1,Qt::AlignLeft);
    networkTypeLayout->setContentsMargins(0, 0, 0, 0); // 去掉布局边缘的空白
    networkTypeLayout->setSpacing(0); // 去掉控件间距

    // 节点数量部分
    QHBoxLayout *nodeCountLayout = new QHBoxLayout;
    QLabel *nodeCountLabel = new QLabel("节点数量:", this);
    nodeCountLineEdit = new QLineEdit(this);
    // 设置字体大小
    setFontSize(nodeCountLabel, 13);
    setFontSize(nodeCountLineEdit, 13);
    nodeCountLineEdit->setPlaceholderText("默认为10个");
    nodeCountLayout->addWidget(nodeCountLabel);
    nodeCountLayout->addWidget(nodeCountLineEdit);

    // 地图比例部分
    QHBoxLayout *mapScaleLayout = new QHBoxLayout;
    QLabel *mapScaleLabel = new QLabel("地图比例:", this);
    mapScaleLineEdit = new QLineEdit(this);
    // 设置输入框内的提示文字
    mapScaleLineEdit->setPlaceholderText("可选填, 默认为1:1000");
    // 设置字体大小
    setFontSize(mapScaleLabel, 13);
    setFontSize(mapScaleLineEdit, 13);
    // 将组件添加到布局中
    mapScaleLayout->addWidget(mapScaleLabel);
    mapScaleLayout->addWidget(mapScaleLineEdit);

    // 信道模型部分
    QHBoxLayout *channelModelLayout = new QHBoxLayout;
    channelModelCheckBox = new QCheckBox("信道模型", this);
    channelModelLayout->addWidget(channelModelCheckBox);
    // 设置字体大小
    setFontSize(channelModelCheckBox, 13);

    // 确认按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    confirmButton = new QPushButton("确认", this);
    setFontSize(confirmButton, 13);
    buttonLayout->addStretch();
    buttonLayout->addWidget(confirmButton);
    // 连接信号和槽
    connect(confirmButton, &QPushButton::clicked, this, &ConfigWindow::onConfirmButtonClicked);


    // 添加到主布局
    mainLayout->addLayout(mapModelLayout);
    mainLayout->addLayout(networkTypeLayout);
    mainLayout->addLayout(nodeCountLayout);
    mainLayout->addLayout(mapScaleLayout);
    mainLayout->addLayout(channelModelLayout);
    mainLayout->addLayout(buttonLayout);
    // // 创建一个水平弹簧
    // QSpacerItem* spaceritem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    // // 添加弹簧到主布局的左侧
    // mainLayout->addItem(spaceritem);
    setLayout(mainLayout);
    // 设置固定大小
    QSize fixedSize(500, 400); // 你可以根据需要调整宽度和高度
    setFixedSize(fixedSize);
}



void ConfigWindow::onConfirmButtonClicked() {
    QProcess process;

    // 构建基本命令
    QString command = "python3 topo.py";

    // 网络类型参数
    int networkTypeIndex = networkTypeComboBox->currentIndex();
    command += QString(" -t %1").arg(networkTypeIndex);

    // 节点数量参数
    QString nodeCountText = nodeCountLineEdit->text();
    if (!nodeCountText.isEmpty()) {
        command += QString(" -n %1").arg(nodeCountText);
    }

    // 地图比例参数
    QString mapScaleText = mapScaleLineEdit->text();
    if (!mapScaleText.isEmpty()) {
        command += QString(" -r %1").arg(mapScaleText);
    }

    // 信道模型参数
    if (channelModelCheckBox->isChecked()) {
        command += " -model";
    }

    // 执行命令
    process.start(command);
    process.waitForFinished();

    // 获取输出
    QByteArray output = process.readAll();
    QString outputString(output);

    // 处理输出或错误
    if (process.exitStatus() == QProcess::CrashExit) {
        QMessageBox::critical(this, "错误", "配置错误，命令执行失败。");
    } else {
        QMessageBox::information(this, "执行结果", outputString);
    }
}
