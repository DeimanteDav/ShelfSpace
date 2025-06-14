#ifndef NOTEEDITWIDGET_H
#define NOTEEDITWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QSqlDatabase>
#include "databasemanager.h"

class QLineEdit;
class QTextEdit;
class QPushButton;

class NoteEditWidget : public QWidget
{
    Q_OBJECT

public:
    NoteEditWidget(QWidget *parent, QString bookId); //create note
    NoteEditWidget(QWidget *parent, QString bookId, QString dateCreated, QString title); //edit note

signals:
    void noteUpdated();

private slots:
    void handleSave();
    void handleEdit();
    void handleDelete();


private:
    QLineEdit *titleEdit;
    QTextEdit *contentEdit;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QPushButton *deleteButton;

    QString bookId; //bookId
    QString dateCreated;

    //Setters
    void setTitle(QString newTitle);
    void setContent(QString newContent);

    //Getters
    QString getBookId() const;
    QString getDateCreated() const;
    QString getTitle() const;
    QString getContent() const;

};

#endif // NOTEEDITWIDGET_H
