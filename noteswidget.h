#ifndef NOTESWIDGET_H
#define NOTESWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "dragscrollarea.h"
#include "labeledbutton.h"
#include "noteeditwidget.h"


#include <QSqlDatabase>
#include <QSqlError>

struct Book;

class notesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit notesWidget(QWidget *parent = nullptr);

signals:
    void openNewNote(const QString &bookId);

private:
    void setUpScrollArea(DragScrollArea *scrollArea);

    void setUpNotes();
    QList<Book> loadAllBooks(QSqlDatabase &db);
    LabeledButton *bookButton;
    QPushButton *addNoteButton;

};

#endif // NOTESWIDGET_H
