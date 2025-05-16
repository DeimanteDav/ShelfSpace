#include <QApplication>
#include <QMainWindow>
#include "noteeditwidget.h"
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
    mainWidget->resize(800, 600);
    mainWidget->show();

    return app.exec();
    /*QApplication app(argc, argv);




    QMainWindow window;
    NoteEditWidget *note = new NoteEditWidget;

    window.setCentralWidget(note);
    window.resize(400, 300);
    window.show();

    return app.exec();*/
}
