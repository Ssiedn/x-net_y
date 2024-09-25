#include <QApplication>
#include "userwindow.h"
#include "loginwindow.h"
#include "adminwindow.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    while (true) {
        LoginWindow login;
        login.show();
        QEventLoop loop;
        QWidget *mainWindow = nullptr;

        QObject::connect(&login, &LoginWindow::loginSuccessful, [&]() {
            QString role = login.getRole();
            if (role == "Normal") {
                mainWindow = new UserWindow();
            } else if (role == "Admin") {
                mainWindow = new AdminWindow();
            }

            if (mainWindow) {
                login.hide();
                mainWindow->show();

                // Connect AdminWindow's close event
                if (AdminWindow *adminWin = qobject_cast<AdminWindow*>(mainWindow)) {
                    QObject::connect(adminWin, &AdminWindow::adminWindowClosed, [&]() {
                        login.show(); // Show login window again if AdminWindow was closed
                        loop.quit(); // Quit the event loop
                    });
                }
            }
        });

        loop.exec(); // Start event loop and wait for login result

        // Exit if all windows are closed
        if (app.allWindows().isEmpty()) {
            break;
        }
    }

    return 0;
}
