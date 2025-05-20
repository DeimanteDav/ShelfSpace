#include <QApplication>
#include <QMainWindow>
#include "noteswidget.h"
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //Temporary database solution
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "ShelfSpaceConnection");
    db.setDatabaseName("/home/augustinas/code/ShelfSpace/ShelfSpace.db");

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return -1;
    }

    QWidget *mainWidget = new notesWidget;

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidget(mainWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Show it in a main window or layout
    scrollArea->resize(800, 600);
    scrollArea->show();

    return app.exec();
}
