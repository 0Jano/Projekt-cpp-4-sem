#include <QApplication>
#include <QDebug>

#include "ui/MainWindow.h"
#include "managers/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!DatabaseManager::instance().openDatabase())
    {
        qDebug() << "Failed to open database.";
        return -1;
    }

    MainWindow window;
    window.show();

    return app.exec();
}