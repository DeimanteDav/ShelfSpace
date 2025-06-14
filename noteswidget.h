#ifndef NOTESWIDGET_H
#define NOTESWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMap>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "dragscrollarea.h"
#include "labeledbutton.h"
#include "noteeditwidget.h"


#include <QSqlDatabase>
#include <QSqlError>

struct Book;
struct Note;

class notesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit notesWidget(QWidget *parent = nullptr);

signals:
    void openNewNote(const QString &bookId);

public slots:
    void handleUpdate();
    void refresh();

private:
    void setUpScrollArea(DragScrollArea *scrollArea);


    void setUpNotes();
    QList<Book> loadAllBooks(QSqlDatabase &db);
    QList<Note> loadAllNotes(QSqlDatabase &db, QString bookId);

    LabeledButton *bookButton;
    QPushButton *addNoteButton;
    QMap<QString, QPushButton*> noteButtons; // key = bookId + dateCreated

};

#endif // NOTESWIDGET_H
