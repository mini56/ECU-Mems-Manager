```cpp
#include <QApplication>
#include <QCoreApplication>
#include <QIcon>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("ECU Mems Manager");
    QApplication::setApplicationVersion("0.9.0");
    QApplication::setOrganizationName("ECU Mems Manager");

    MainWindow window;
    window.show();

    return app.exec();
}
```
