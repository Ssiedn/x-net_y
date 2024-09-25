#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QDialog>

class QComboBox;
class QLineEdit;
class QCheckBox;
class QPushButton;

class ConfigWindow : public QDialog {
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget *parent = nullptr);

private slots:
    void onConfirmButtonClicked();
    void setFontSize(QWidget *widget, int size) {
        QFont font = widget->font();
        font.setPointSize(size);
        widget->setFont(font);
    }


private:
    QString terrainSaveDirectory = "G:/UI test/3D模型";
    QComboBox *networkTypeComboBox;
    QComboBox *mapModelComboBox;
    QLineEdit *nodeCountLineEdit;
    QLineEdit *mapScaleLineEdit;
    QCheckBox *channelModelCheckBox;
    QPushButton *confirmButton;
};

#endif // CONFIGWINDOW_H
