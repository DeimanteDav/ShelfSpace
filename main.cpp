#include "mainwindow.h"
#include "databasemanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool ok = DatabaseManager::instance().openDatabase("ShelfSpace.db");
    if (!ok) {
        qDebug() << "Failed to open database, exiting...";
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
