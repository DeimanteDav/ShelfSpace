#include "noteeditwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

#include <QObject>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include <QDateTime>

//id, bookid, reviewsid, dateCreated, dateModified, title, text
void saveNoteToDatabase(const QString &bookId,
                        const QDateTime &dateCreated,
                        const QString &title, const QString &text)
{

    QSqlDatabase db = QSqlDatabase::database("ShelfSpaceConnection");

    if (!db.isOpen()) {
        qDebug() << "Database not open for noteSaveOperation!";
        return;
    }

    QSqlQuery query(db);
    if(!db.tables().contains("tbNotesLocal")){
        QString createTable = "CREATE TABLE IF NOT EXISTS tbNotesLocal ("
                              "bookId TEXT NOT NULL, "
                              "dateCreated TEXT NOT NULL, "
                              "dateModified TEXT NOT NULL, "
                              "title TEXT NOT NULL, "
                              "text TEXT"
                              ");";

        if (!query.exec(createTable)) {
            qDebug() << "Error: Failed to create table -" << query.lastError().text();
        } else {
            qDebug() << "Table created successfully!";
        }
    } else{
        qDebug() << "Table already exists";
    }

    query.prepare("INSERT INTO tbNotesLocal (bookId, dateCreated, dateModified, title, text) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(bookId);
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss"));
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss"));
    query.addBindValue(title);
    query.addBindValue(text);

    if (!query.exec()) {
        qDebug() << "Failed to insert note:" << query.lastError().text();
    } else {
        qDebug() << "Note saved successfully!";
    }
}

void loadNoteFromDatabase(QString id){

    QSqlDatabase db = QSqlDatabase::database("ShelfSpaceConnection");

    if (!db.isOpen()) {
        qDebug() << "Database not open for noteLoadOperation!";
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT text, dateCreated FROM tbReviews WHERE bookid = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qCritical() << "Query failed:" << query.lastError().text();
        return;
    }

    if(query.next()){ // match found
        int id = query.value("id").toInt();
        QString text = query.value("text").toString();
        QString date = query.value("dateCreated").toString();

        qDebug() << "ID: "<< id;
        qDebug() << "Content: " << text;
    }
}

NoteEditWidget::NoteEditWidget(QWidget *parent, QString id)
    : QWidget(parent), id(id)
{

    qDebug() << "Openning a note window";
    titleEdit = new QLineEdit(this);
    titleEdit->setPlaceholderText("Title");

    contentEdit = new QTextEdit(this);
    contentEdit->setPlaceholderText("Write your note here...");

    saveButton = new QPushButton("Save", this);

    //loadButton = new QPushButton("Load", this); No load functionality rn

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(titleEdit);
    layout->addWidget(contentEdit);
    layout->addWidget(saveButton);
    //layout->addWidget(loadButton);

    setLayout(layout);

    connect(saveButton, &QPushButton::clicked, this, &NoteEditWidget::handleSave);
    //connect(loadButton, &QPushButton::clicked, this, &NoteEditWidget::loadNote);
}

void NoteEditWidget::setContent(QString newContent)
{
    contentEdit->setText(newContent);
}
void NoteEditWidget::setTitle(QString newTitle)
{
    contentEdit->setText(newTitle);
}

QString NoteEditWidget::title() const
{
    return titleEdit->text();
}

QString NoteEditWidget::content() const
{
    return contentEdit->toPlainText();
}

QString NoteEditWidget::bookId() const
{
    return id;
}

QDateTime NoteEditWidget::dateCreated() const
{
    return QDateTime::currentDateTime();
}


void NoteEditWidget::handleSave()
{
    emit noteSaved(title(), content());
    saveNoteToDatabase(bookId(), dateCreated(), title(), content());
}

void NoteEditWidget::loadNote(){
    //loadNoteFromDatabase(id());
    return;
}

