#include "mainWindow.h"

#include <QApplication>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
