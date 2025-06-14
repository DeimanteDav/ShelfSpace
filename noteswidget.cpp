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
    QSqlDatabase db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
    } else {
        qDebug() << "Database connection reused successfully";
    }


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();

    this->setLayout(mainLayout);

    refresh();
}

QList<Book> notesWidget::loadAllBooks(QSqlDatabase &db) {
    QList<Book> books;
    QList<QString> ids;
    QSqlQuery query(db);

    if(!db.tables().contains("tbFavorites") || !db.tables().contains("tbBooks")){
        qDebug() << "Load favorites/books table not found!";
        return books;
    }
    else{
        qDebug() << "Favorites and Books table found!";
    }

    query.prepare("SELECT bookId FROM tbFavorites");

    if (!query.exec()) {
        qDebug() << "Book load query failed:" << query.lastError().text();
        return books;
    }
    else{
        qDebug() << "Fetching bookIds from tbFavorites";
    }

    while (query.next()) { // get bookId list
        QString temp;
        temp = query.value("bookId").toString();
        ids.append(temp);
    }

    QStringList placeholders; // A placeholder for all bookIds
    for (int i = 0; i < ids.size(); ++i) {
        placeholders << QString(":id%1").arg(i);
    }

    QString queryStr = QString("SELECT id, title, author, year, image FROM tbBooks WHERE id IN (%1)")
                           .arg(placeholders.join(", "));
    qDebug() << "Query string:" << queryStr;

    query.prepare(queryStr);

    for (int i = 0; i < ids.size(); ++i) { // Giving placeholders value
        query.bindValue(QString(":id%1").arg(i), ids[i]);
        qDebug() << "bindValue: " << QString(":id%1").arg(i) << ids[i];
    }

    if (!query.exec()) {
        qWarning() << "Query execution failed:" << query.lastError();
        return books;
    }

    while(query.next()){
        Book temp;
        temp.title = query.value("title").toString();
        temp.author = query.value("author").toString();
        temp.year = query.value("year").toInt();
        temp.imageUrl = query.value("image").toString();
        temp.id = query.value("id").toString();
        qDebug() << query.value("id").toString();
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
        qDebug() << "Note load query failed:" << query.lastError().text();
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

void notesWidget::refresh() {
    QLayoutItem *child;
    while ((child = layout()->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget(); // Remove old widgets
        }
        delete child;
    }

    QSqlDatabase db = DatabaseManager::instance().database();
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
                connect(note, &NoteEditWidget::noteUpdated, this, &notesWidget::refresh);
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
            connect(note, &NoteEditWidget::noteUpdated, this, &notesWidget::refresh);
            note->show();
        });

        // Scroll Area.
        DragScrollArea *scrollArea = new DragScrollArea;
        setUpScrollArea(scrollArea);

        scrollArea->setWidget(contentWidget);
        layout()->addWidget(scrollArea);
    }
}

void notesWidget::handleUpdate(){
    refresh();
}
