#include "loginwindow.h"
#include <QMainWindow>
#include <QToolBar>
#include <QDockWidget>
#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <Qlabel>
#include <QFormLayout>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent),
    usernameEdit(new QLineEdit(this)),
    passwordEdit(new QLineEdit(this)),
    loginButton(new QPushButton("Login", this)),
    registerButton(new QPushButton("Register", this)),
    showPasswordCheckBox(new QCheckBox("Show Password", this)),
    roleComboBox(new QComboBox(this)),
    layout(new QHBoxLayout(this)),  // 修改为 QHBoxLayout
    imageLabel(new QLabel(this))    // 新增 QLabel
{
    setWindowTitle("登录界面");
    setWindowIcon(QPixmap("://UI_simulation/Resources/system.png"));
    setFixedSize(750, 550); // 固定窗口大小
    QString style = "background-color: #ECE9E9;font-size: 14px;";
    QString titlestyle = "background-color: #ECE9E9; font-size: 17px; font-weight: bold;";
    //QString style = "background-color: #C9E3F9;color:white;font-size: 14px;";
    //QString titlestyle = "background-color: #C9E3F9; color:white;font-size: 17px; font-weight: bold;";
    // Create name label with icon
    QLabel *iconLabel = new QLabel(this);
    iconLabel->setPixmap(QPixmap("://UI_simulation/Resources/system.png"));  // Replace with your icon path


    QLabel *nameLabel = new QLabel("X-Net Simulation System", this);
    nameLabel->setStyleSheet(titlestyle);
    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->addWidget(iconLabel);
    nameLayout->addWidget(nameLabel);
    nameLayout->setAlignment(Qt::AlignCenter);
    // 设置密码模式
    passwordEdit->setEchoMode(QLineEdit::Password);
    // 设置提示文字
    usernameEdit->setPlaceholderText("Username");
    passwordEdit->setPlaceholderText("Password");
    // 设置角色选择下拉框
    roleComboBox->addItems({"Admin", "Normal"});
    roleComboBox->setFixedWidth(100);  // 设置固定宽度
    // 使用 QFormLayout 对齐标签和输入框
    QFormLayout *formLayout = new QFormLayout;
    QLabel *usernameLabel = new QLabel("用户名：");
    QLabel *passwordLabel = new QLabel("密码：");
    usernameLabel->setStyleSheet(style);
    passwordLabel->setStyleSheet(style);
    showPasswordCheckBox->setStyleSheet(style);
    formLayout->addRow(usernameLabel, usernameEdit);
    formLayout->addRow(passwordLabel, passwordEdit);
    formLayout->addRow(roleComboBox);
    formLayout->addRow(showPasswordCheckBox);
    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    // 使用 QSpacerItem 使整体居中
    QSpacerItem *topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QSpacerItem *titleSpacer = new QSpacerItem(20, 25, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *buttonSpacer = new QSpacerItem(20, 15, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    // 添加布局到主布局
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addItem(topSpacer);
    //leftLayout->addWidget(nameLabel, 0, Qt::AlignCenter);  // 对齐中心
    leftLayout->addLayout(nameLayout);  // Use nameLayout here
    leftLayout->addItem(titleSpacer);
    leftLayout->addLayout(formLayout);
    leftLayout->addItem(buttonSpacer);
    leftLayout->addLayout(buttonLayout);
    leftLayout->addItem(bottomSpacer);
    // 设置左侧组件背景色
    QWidget *leftWidget = new QWidget(this);
    leftWidget->setLayout(leftLayout);
    leftWidget->setStyleSheet(style);  // 灰色透明背景
    layout->addWidget(leftWidget,1);

    // 设置图片的 QLabel
    imageLabel->setPixmap(QPixmap("://UI_simulation/Resources/jm.jpg")); // 设置图片路径
    imageLabel->setScaledContents(true); // 使图片适应 QLabel
    layout->addWidget(imageLabel,4);

    // 设置主布局
    setLayout(layout);

    // 连接信号和槽
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::handleRegister);
    connect(showPasswordCheckBox, &QCheckBox::toggled, this, &LoginWindow::togglePasswordVisibility);

    // 加载账户数据
    loadAccountData();

    // 清空输入框内容
    usernameEdit->clear();
    passwordEdit->clear();
}
//获取账号类型
QString LoginWindow::getRole() const
{
    return roleComboBox->currentText();
}
//登录
void LoginWindow::handleLogin()
{
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    QString role = getRole(); // 获取选择的角色

    QFile file(accountdirectory);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc(QJsonDocument::fromJson(data));
        QJsonObject json = doc.object();

        if (json.contains(role) && json[role].toObject().contains(username) && json[role][username].toString() == password) {
            emit loginSuccessful(); // 自定义信号
        } else {
            QMessageBox::warning(this, "Login Failed", "Username or password is incorrect.");
        }
    } else {
        QMessageBox::warning(this, "Login Failed", "Unable to open account data file.");
    }
    // 清除输入框
    usernameEdit->clear();
    passwordEdit->clear();
}
//注册
void LoginWindow::handleRegister()
{
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    QString role = getRole(); // 获取选择的角色
    // 检查用户名和密码是否为空
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Register Failed", "Username and password cannot be empty.");
        return;
    }
    QFile file(accountdirectory);
    QJsonObject json;
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc(QJsonDocument::fromJson(data));
        json = doc.object();
    }
    // 检查用户名在所有角色下是否已存在
    bool usernameExists = false;
    for (auto key : json.keys()) {
        QJsonObject roleObject = json[key].toObject();
        if (roleObject.contains(username)) {
            usernameExists = true;
            break;
        }
    }
    if (usernameExists) {
        QMessageBox::warning(this, "Register Failed", "Username already exists.");
        return;
    }
    // 用户名不存在，进行注册
    if (!json.contains(role)) {
        json[role] = QJsonObject();
    }
    QJsonObject roleObject = json[role].toObject();
    roleObject[username] = password;
    json[role] = roleObject;
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(json);
        file.write(doc.toJson());
        file.close();
        QMessageBox::information(this, "Registration Successful", "Account created successfully.");
    } else {
        QMessageBox::warning(this, "Register Failed", "Unable to open account data file.");
    }
    // 清除输入框
    usernameEdit->clear();
    passwordEdit->clear();
}

//密码可见
void LoginWindow::togglePasswordVisibility(bool checked)
{
    passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}
//加载账号数据
void LoginWindow::loadAccountData()
{
    QFile file(accountdirectory);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc(QJsonDocument::fromJson(data));
        QJsonObject json = doc.object();

        // 加载第一个角色的第一个用户的用户名和密码（如果存在）
        if (!json.isEmpty()) {
            QString role = json.keys().first(); // 获取第一个角色
            QJsonObject roleObject = json[role].toObject();
            if (!roleObject.isEmpty()) {
                QString username = roleObject.keys().first(); // 获取第一个用户
                QString password = roleObject[username].toString();
                usernameEdit->setText(username);
                passwordEdit->setText(password);
            }
        }
    }
}
//保存账号数据
void LoginWindow::saveAccountData(const QString &username, const QString &password, const QString &role)
{
    // 尝试打开文件以进行写入
    QFile file(accountdirectory);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件进行写入:" << accountdirectory;
        return;
    }

    // 读取现有的数据（如果文件存在并且非空）
    QJsonObject json;
    if (file.size() > 0) {
        file.seek(0); // 将文件指针移动到开始位置
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc(QJsonDocument::fromJson(data));
        json = doc.object();
    }
    else {
        file.close();
    }

    // 创建或更新角色对象
    QJsonObject roleObject;
    if (json.contains(role)) {
        roleObject = json[role].toObject();
    }

    // 更新用户数据
    roleObject[username] = password;

    // 更新角色数据
    json[role] = roleObject;

    // 写入更新后的数据
    QJsonDocument doc(json);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.write(doc.toJson());
    file.close();
}
