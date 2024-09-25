#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
class QMenuBar;

class AdminWindow : public QMainWindow {
    Q_OBJECT

public:
    AdminWindow(QWidget *parent = nullptr);

private slots:
    void populateList(QListWidget *listWidget, const QString &path, const QString &filter);
    void uploadModel(const QString &destinationDir, const QString &dialogTitle,const QString &areaTitle);
    void switchToTerrain();
    void switchToNode();
    void switchToTask();
    void switchToBusiness();
    void updateModelList(const QString &destinationDir,const QString &areaTitle);
    void handleItemClicked(QListWidgetItem *item);
    void handleItemDoubleClicked(QListWidgetItem *item);
    void deleteModel(const QString &destinationDir, const QString &filename,const QString &areaTitle);
    void openModel(const QString &destinationDir, const QString &filename,const QString &areaTitle);
    QString directoryGenerate(QString directory_name)
    {
        // 获取持久数据路径
        QString persistentPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        // 创建目标文件夹路径
        QString dirPath = QDir(persistentPath).filePath(directory_name);

        // 创建上层目录
        QDir dir(persistentPath);
        if (!dir.exists()) {
            dir.mkpath("."); // 创建所有必要的上层目录
        }

        // 创建目标文件夹
        QDir targetDir(dirPath);
        if (!targetDir.exists()) {
            if (targetDir.mkpath(".")) {
                qDebug() << "文件夹创建成功：" << dirPath;
            } else {
                qDebug() << "文件夹创建失败：" << dirPath;
            }
        } else {
            qDebug() << "文件夹已存在：" << dirPath;
        }

        return dirPath;
    }

signals:
    void adminWindowClosed();

protected:
    void closeEvent(QCloseEvent *event) override {
        emit adminWindowClosed();  // Emit signal when the window is closing
        QWidget::closeEvent(event); // Call base class implementation
    }
    void showEvent(QShowEvent *event) override {
        QWidget::showEvent(event);
        resize(800, 600); // 设置窗口初始大小为800x600
    }
private:
    QMenuBar *menuBar;
    QToolBar *toolBar;
    QString current_fileName;  // 成员变量

    QString terrainSaveDirectory = directoryGenerate("MapModels");
        //"G:/UI test/3D模型"; // 默认保存目录
    QString nodeSaveDirectory = directoryGenerate("NodeModels");
        //"G:/UI test/节点模型"; // 默认保存目录

    QStackedWidget *stackedWidget;
    QWidget *terrainWidget;
    QWidget *nodeWidget;
    QWidget *taskWidget;
    QWidget *businessWidget;

    QListWidget *terrainList;
};

#endif // MAINWINDOW_H

