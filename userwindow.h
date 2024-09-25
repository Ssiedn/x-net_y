#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QFormLayout>
#include <QIcon>
#include <QFileInfo>
class QAction;
class QMenuBar;
class QTextEdit;

class UserWindow : public QMainWindow {
    Q_OBJECT

public:
    UserWindow();

private slots:
    void onItemClicked(QTreeWidgetItem *item, int column);
    void updateLinkAttributesWidget(const QJsonObject &attributes);
    void updateNodeAttributesWidget(const QJsonObject &attributes);
    void populateTree(QTreeWidget *tree);
    void handleNodeClick(const QString& itemName, const QJsonObject& nodes);
    void handleLinkClick(const QString& itemName, const QJsonObject& links);
    void showNetworkConfigWindow();
    void toggleDockWindow(QDockWidget *dockWidget);
    void checkForUpdates(QTreeWidget *tree);
    void expandAllItems(QTreeWidget *tree);
    void clearInputFields(QLineEdit *inputField);
protected:
    void showEvent(QShowEvent *event) override {
        QWidget::showEvent(event);
        showMaximized(); // Maximize the window when it is shown
    }
private:
    QString nodesFilePath = "G:/UI test/nodes.json";
    QString linksFilePath = "G:/UI test/links.json";
    QFileInfo nodesFileInfo = QFileInfo(nodesFilePath);;
    QFileInfo linksFileInfo = QFileInfo(linksFilePath);;
    QTimer *updateTimer;

    QMenuBar *menuBar;
    QToolBar *toolBar;

    QDockWidget *dataDock;
    QDockWidget *statusDock;
    QDockWidget *terminalDock;
    QTextEdit *statusDisplay;
    QTextEdit *infoDisplay;

    QDockWidget *nodeAttributesDock;
    QFormLayout *nodeAttributesLayout;
    QWidget *nodeAttributesContent;

    QDockWidget *linkAttributesDock;
    QWidget *linkAttributesContent;
    QFormLayout *linkAttributesLayout;
};

#endif // USERWINDOW_H

