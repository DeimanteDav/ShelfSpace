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
/*
#include <QApplication>
#include <QMainWindow>
#include "bookListView.h"
#include "dbManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString dbPath = "ShelfSpace.db";
    DatabaseManager::connect(dbPath);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle("My Personal Book List");

    // Create and add BookListView
    BookListView *bookListView = new BookListView();
    mainWindow.setCentralWidget(bookListView);

    mainWindow.resize(800, 600);
    mainWindow.show();

    return app.exec();
*/
}
