#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QVBoxLayout;
QT_END_NAMESPACE

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    QString getRole() const;

private slots:
    void handleLogin();
    void handleRegister();
    void togglePasswordVisibility(bool checked);
    QString accountGenerate(QString file_path_name)
    {
        // 获取持久数据路径
        QString persistentPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        // 创建目标文件路径
        QString filePath = QDir(persistentPath).filePath(file_path_name);
        // 创建上层目录
        QDir dir(persistentPath);
        if (!dir.exists()) {
            dir.mkpath("."); // 创建所有必要的上层目录
        }
        // 检查文件是否存在
        QFile file(filePath);
        if (!file.exists()) {
            if (file.open(QIODevice::WriteOnly)) {
                QTextStream out(&file);
                out << "{}"; // 写入一个空的 JSON 对象
                file.close();
                qDebug() << "文件创建成功：" << filePath;
            } else {
                qDebug() << "文件打开失败：" << filePath;
            }
        } else {
            qDebug() << "文件已存在：" << filePath;
        }
        return filePath;
    }

signals:
    void loginSuccessful(); // 自定义信号，用于登录成功时通知其他部分

private:
    QString accountdirectory = accountGenerate("users.dat");
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QCheckBox *showPasswordCheckBox;
    QComboBox *roleComboBox;
    QHBoxLayout *layout;
    QLabel *imageLabel;
    void loadAccountData();
    void saveAccountData(const QString &username, const QString &password, const QString &role);
};

#endif // LOGINWINDOW_H

