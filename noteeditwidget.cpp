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
                        const QString &dateCreated,
                        const QString &title, const QString &text)
{

    if (!DatabaseManager::instance().openDatabase("ShelfSpace.db")) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    if(!DatabaseManager::instance().database().tables().contains("tbNotesLocal")){
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
    query.addBindValue(dateCreated);
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz"));
    if(title == ""){
        QString newTitle = "Note " + QDate::currentDate().toString("dd/MM");
        query.addBindValue(newTitle);
        qDebug() << "No provided title, making one automatically: " << newTitle;
    }else{
        query.addBindValue(title);
    }
    query.addBindValue(text);

    if (!query.exec()) {
        qDebug() << "Failed to insert note:" << query.lastError().text();
    } else {
        qDebug() << "Note saved successfully!";
    }
}

void editNoteFromDatabase(const QString &bookId,
                        const QString &dateCreated,
                        const QString &newTitle, const QString &newText)
{
    if (!DatabaseManager::instance().openDatabase("ShelfSpace.db")) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE tbNotesLocal SET title = ?, text = ?, dateModified = ? "
                  "WHERE bookId = ? AND dateCreated = ?");

    query.addBindValue(newTitle);
    query.addBindValue(newText);
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz"));
    query.addBindValue(bookId);
    query.addBindValue(dateCreated);

    if(!query.exec()){
        qDebug() << "Edit FAILED";
    }
    else{
        qDebug() << "Edit SUCCESS";
    }
}

void deleteNoteFromDatabase(const QString &bookId, const QString &dateCreated)
{
    if (!DatabaseManager::instance().openDatabase("ShelfSpace.db")) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM tbNotesLocal WHERE bookId = ? AND dateCreated = ?");
    query.addBindValue(bookId);
    query.addBindValue(dateCreated);

    if (!query.exec()) {
        qDebug() << "Failed to delete note:" << query.lastError().text();
    } else {
        qDebug() << "Note deleted successfully!";
    }
}

NoteEditWidget::NoteEditWidget(QWidget *parent, QString id) //New note
    : QWidget(parent), bookId(id), dateCreated(QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz"))
{

    qDebug() << "Opening a note window";
    titleEdit = new QLineEdit(this);
    titleEdit->setPlaceholderText("Title");

    contentEdit = new QTextEdit(this);
    contentEdit->setPlaceholderText("Write your note here...");

    saveButton = new QPushButton("Save", this);
    deleteButton = new QPushButton("Delete", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(titleEdit);
    layout->addWidget(contentEdit);
    layout->addWidget(saveButton);
    layout->addWidget(deleteButton);

    setLayout(layout);

    connect(saveButton, &QPushButton::clicked, this, &NoteEditWidget::handleSave);
    connect(deleteButton, &QPushButton::clicked, this, &NoteEditWidget::handleDelete);
}

NoteEditWidget::NoteEditWidget(QWidget *parent,QString id,QString dateCreated, QString title) //Load a note
    : QWidget(parent), bookId(id), dateCreated(dateCreated)
{

    if (!DatabaseManager::instance().openDatabase("ShelfSpace.db")) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT text FROM tbNotesLocal WHERE bookid = ? AND dateCreated = ? AND title = ?");
    query.addBindValue(id);
    query.addBindValue(dateCreated);
    query.addBindValue(title);


    QString text;
    if (!query.exec()) {
        qCritical() << "Query failed:" << query.lastError().text();
        return;
    } else{
        if(query.next()){
            text = query.value("text").toString();
            qDebug() << "Node content found";
        }
        else{
            qDebug() << "No matching note found";
            return;
        }
    }

    qDebug() << "Opening a note window";
    titleEdit = new QLineEdit(this);
    titleEdit->setText(title);

    contentEdit = new QTextEdit(this);
    contentEdit->setText(text);

    saveButton = new QPushButton("Save", this);
    deleteButton = new QPushButton("Delete", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(titleEdit);
    layout->addWidget(contentEdit);
    layout->addWidget(saveButton);
    layout->addWidget(deleteButton);

    setLayout(layout);

    connect(saveButton, &QPushButton::clicked, this, &NoteEditWidget::handleEdit);
    connect(deleteButton, &QPushButton::clicked, this, &NoteEditWidget::handleDelete);
}

void NoteEditWidget::setContent(QString newContent)
{
    contentEdit->setText(newContent);
}
void NoteEditWidget::setTitle(QString newTitle)
{
    contentEdit->setText(newTitle);
}

QString NoteEditWidget::getTitle() const
{
    return titleEdit->text();
}

QString NoteEditWidget::getContent() const
{
    return contentEdit->toPlainText();
}

QString NoteEditWidget::getBookId() const
{
    return bookId;
}

QString NoteEditWidget::getDateCreated() const
{
    return dateCreated;
}


void NoteEditWidget::handleSave()
{
    saveNoteToDatabase(getBookId(), getDateCreated(), getTitle(), getContent());
    emit noteUpdated();
}

void NoteEditWidget::handleEdit(){
    editNoteFromDatabase(getBookId(), getDateCreated(), getTitle(), getContent());
    emit noteUpdated();
}

void NoteEditWidget::handleDelete(){
    deleteNoteFromDatabase(getBookId(), getDateCreated());
    emit noteUpdated();
    this->close();
}

