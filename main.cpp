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
}
