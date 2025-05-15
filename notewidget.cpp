#include "notewidget.h"
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

    query.prepare("INSERT INTO tbReviews (bookId, dateCreated, dateModified, title, text) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(bookId);
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    query.addBindValue(title);
    query.addBindValue(text);

    if (!query.exec()) {
        qDebug() << "Failed to insert note:" << query.lastError().text();
    } else {
        qDebug() << "Note saved successfully!";
    }
}

void loadNoteFromDatabase(int id){

    QSqlDatabase db = QSqlDatabase::database("ShelfSpaceConnection");

    if (!db.isOpen()) {
        qDebug() << "Database not open for noteLoadOperation!";
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT text, dateCreated FROM tbReviews WHERE id = ?");
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

NoteWidget::NoteWidget(QWidget *parent)
    : QWidget(parent)
{
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

    connect(saveButton, &QPushButton::clicked, this, &NoteWidget::handleSave);
    //connect(loadButton, &QPushButton::clicked, this, &NoteWidget::loadNote);
}

void NoteWidget::setContent(QString newContent)
{
    contentEdit->setText(newContent);
}
void NoteWidget::setTitle(QString newTitle)
{
    contentEdit->setText(newTitle);
}

QString NoteWidget::title() const
{
    return titleEdit->text();
}

QString NoteWidget::content() const
{
    return contentEdit->toPlainText();
}

QString NoteWidget::bookId() const
{
    return "replace_later";
}

QDateTime NoteWidget::dateCreated() const
{
    return QDateTime::currentDateTime();
}

int NoteWidget::id() const
{
    return testid;
}

void NoteWidget::handleSave()
{
    emit noteSaved(title(), content());
    saveNoteToDatabase(bookId(), dateCreated(), title(), content());
}

void NoteWidget::loadNote(){
    loadNoteFromDatabase(id());
}

