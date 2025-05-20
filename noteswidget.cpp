#include "noteswidget.h"
#include <QRandomGenerator>


struct Book {
    QString id;
    QString title;
    QString author;
    int year;
    QString imageUrl;
};

struct Note {
    QString id;
    QString title;
    QString dateCreated;
};

notesWidget::notesWidget(QWidget *parent)
    : QWidget(parent)
{
    /*QSqlDatabase db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
    } else {
        qDebug() << "Database connection reused successfully";
    }*/

    QSqlDatabase db = QSqlDatabase::database("ShelfSpaceConnection");

    if (!db.isOpen()) {
        qCritical() << "Database not open for main NotesWidget!";
        return;
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

//For each book:
    QSqlQuery query(db);
    QList books = loadAllBooks(db);
    for(int i = 0; i < books.size(); i++){

        QWidget *contentWidget = new QWidget;
        QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
        contentLayout->setSpacing(10);
        contentLayout->setContentsMargins(10, 10, 10, 10);
        contentLayout->setAlignment(Qt::AlignLeft);

        // First button with icon and label /1 per book
        bookButton = new LabeledButton;
        bookButton->setIconFromUrl(books[i].imageUrl, QSize(120, 120));
        bookButton->setLabelText(books[i].title +" by " + books[i].author);
        bookButton->setTotalSize(QSize(160, 160));
        contentLayout->addWidget(bookButton);


        QList notes = loadAllNotes(db, books[i].id);
        for (int i = 0; i < notes.size(); ++i) {
            QPushButton *button = new QPushButton(notes[i].title);
            button->setStyleSheet("text-align: left;");
            button->setFixedSize(120, 100);
            connect(button, &QPushButton::clicked, this, [=]() {
                NoteEditWidget *note = new NoteEditWidget(nullptr, notes[i].id, notes[i].dateCreated, notes[i].title);
                qDebug() << "Trying to edit note of this book: " << notes[i].id;
                qDebug() << "Title:" << notes[i].title;
                note->show();
            });
            contentLayout->addWidget(button);
        }



        // Add note button. / 1 per book
        addNoteButton = new QPushButton("Add note");
        addNoteButton->setFixedSize(120, 60);
        contentLayout->addWidget(addNoteButton);
        connect(addNoteButton, &QPushButton::clicked, this, [=]() {
            qDebug() << "Trying to add note to this book: " << books[i].id;
            NoteEditWidget *note = new NoteEditWidget(nullptr, books[i].id);
            note->show();
        });

        // Scroll Area.
        DragScrollArea *scrollArea = new DragScrollArea;
        setUpScrollArea(scrollArea);

        scrollArea->setWidget(contentWidget);
        mainLayout->addWidget(scrollArea);
    }

//end for
    mainLayout->addStretch();
}

QList<Book> notesWidget::loadAllBooks(QSqlDatabase &db) {
    QList<Book> books;
    QSqlQuery query(db);

    query.prepare("SELECT id, title, author, year, image FROM tbFavorites");
    //query.addBindValue(4); //for limit at testing
    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError().text();
        return books;
    }

    while (query.next()) {
        Book temp;
        temp.id = query.value("id").toString();
        temp.title = query.value("title").toString();
        temp.author = query.value("author").toString();
        temp.year = query.value("year").toInt();
        temp.imageUrl = query.value("image").toString();

        books.append(temp);
    }

    return books;
}

QList<Note> notesWidget::loadAllNotes(QSqlDatabase &db, QString bookId){
    QList<Note> notes;
    QSqlQuery query(db);

    query.prepare("SELECT title, dateCreated FROM tbNotesLocal WHERE bookId = ? ORDER BY dateModified DESC");
    query.addBindValue(bookId);

    if(!query.exec()){
        qDebug() << "Query failed:" << query.lastError().text();
        return notes;
    }

    while (query.next()){
        Note temp;
        temp.id = bookId;
        temp.dateCreated = query.value("dateCreated").toString();
        temp.title = query.value("title").toString();
        notes.append(temp);
    }

    return notes;
}

void notesWidget::setUpScrollArea(DragScrollArea *scrollArea){
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFixedHeight(190);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}
