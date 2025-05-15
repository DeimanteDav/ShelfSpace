#include <QApplication>
#include <QMainWindow>
#include "notewidget.h"
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFileInfo dbFile("/home/augustinas/code/ShelfSpace/ShelfSpace.db");
    qDebug() << "File exists:" << dbFile.exists();
    qDebug() << "Readable:" << dbFile.isReadable();
    qDebug() << "Absolute file path:" << dbFile.absoluteFilePath();


    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "ShelfSpaceConnection");
    db.setDatabaseName("/home/augustinas/code/ShelfSpace/ShelfSpace.db"); // May change!

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return -1;
    }

    qDebug() << "DB file:" << db.databaseName();

    QStringList tables = db.tables();
    if(!tables.isEmpty()){
        for(const QString &table: tables){
            qDebug() << table;
        }
    } else{
        qDebug() << "EMPTYY";
    }

    QMainWindow window;
    NoteWidget *note = new NoteWidget;

    QObject::connect(note, &NoteWidget::noteSaved, [](const QString &title, const QString &content) {
        qDebug() << "Note Saved:";
        qDebug() << "Title:" << title;
        qDebug() << "Content:" << content;
    });

    window.setCentralWidget(note);
    window.resize(400, 300);
    window.show();

    return app.exec();
}
