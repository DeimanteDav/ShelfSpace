#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "bookdetailswindow.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDebug>
#include <QFont>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QEvent>
#include <QListWidget>
#include <QPushButton>
#include <QMenuBar>
#include <QApplication>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
    , stackedWidget(new QStackedWidget(this)),
    addBookWidget(new QWidget(this)),
    viewBooksWidget(new QWidget(this))
{
    //ui->setupUi(this);

    //setupDatabase();
    //setupModel();


   // auto *mb = new QMenuBar(this);
   // setMenuBar(mb);

    //setupPages();
    setupMenu(); //ar svarbi eiga?

    // setupToolbar(); //no toolbar
    setupCentralViews(); //new

    setCentralWidget(stackedWidget);

    setWindowTitle("ShelfSpace");
    resize(800, 600); //new

    //stackedWidget->setCurrentWidget(mainPage);
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::setupMenu()
{
    QMenu *bookMenu = menuBar()->addMenu("&Book");

    /*
    actionAddBook = new QAction("&Add Book", this);
    connect(actionAddBook, &QAction::triggered, this, &MainWindow::showAddBookView);
    bookMenu->addAction(actionAddBook);
    */

    actionViewBooks = new QAction("&View Collection", this);
    connect(actionViewBooks, &QAction::triggered, this, &MainWindow::showViewBooksView);
    bookMenu->addAction(actionViewBooks);

    QMenu *notesMenu = menuBar()->addMenu("Notes");
    actionShowNotes = new QAction("Open Notes", this);
    connect(actionShowNotes, &QAction::triggered, this, &MainWindow::showNotesView);
    notesMenu->addAction(actionShowNotes);

    QMenu *fileMenu = menuBar()->addMenu("&File"); //new
    actionExit = new QAction("&Exit", this);
    connect(actionExit, &QAction::triggered, this, &MainWindow::exitApplication);
    fileMenu->addAction(actionExit);


    /*
    QMenu *viewMenu = menuBar()->addMenu("View");


    QAction *mainAct = new QAction("Main", this);
    QAction *favAct = new QAction("Favorites", this);
    QAction *notesAct = new QAction("Notes", this);
    QAction *bookAct = new QAction("Book", this); // do we need this? Kaip zinos i kokia knyga eit?

    connect(mainAct, &QAction::triggered, this, &MainWindow::showMainPage);
    connect(favAct, &QAction::triggered, this, &MainWindow::showFavoritesPage);
    connect(notesAct, &QAction::triggered, this, &MainWindow::showNotesPage);
    connect(bookAct, &QAction::triggered, this, &MainWindow::showBookPage);

    viewMenu->addAction(mainAct);
    viewMenu->addAction(favAct);
    viewMenu->addAction(notesAct);
    viewMenu->addAction(bookAct);
*/
}

//don't need toolbar:
/*
void MainWindow::setupToolbar() {
    QToolBar *toolbar = addToolBar("Main Toolbar");
    toolbar->addAction(actionAddBook);
    toolbar->addAction(actionViewBooks);
}
*/

void MainWindow::setupCentralViews() {

    /*
    // Placeholder widget for Add Book
    QVBoxLayout *addLayout = new QVBoxLayout();
    addLayout->addWidget(new QLabel("Add Book Form (placeholder)", this));
    addBookWidget->setLayout(addLayout);
    */

    // Placeholder widget for View Books
    QVBoxLayout *viewLayout = new QVBoxLayout();
    viewLayout->addWidget(new QLabel("View Books Table (placeholder)", this));
    viewBooksWidget->setLayout(viewLayout);

    stackedWidget->addWidget(viewBooksWidget); // index 0
    stackedWidget->addWidget(addBookWidget);   // index 1

    notesWidget = new QLabel("Notes page will be implemented here.");
    stackedWidget->addWidget(notesWidget);

    stackedWidget->setCurrentWidget(viewBooksWidget); // default view
}

/*
void MainWindow::showAddBookView() {
    stackedWidget->setCurrentWidget(addBookWidget);
}
*/

void MainWindow::showViewBooksView() {
    stackedWidget->setCurrentWidget(viewBooksWidget);
}

void MainWindow::showNotesView() {
    stackedWidget->setCurrentWidget(notesWidget);
}

void MainWindow::exitApplication() {
    QApplication::quit();
}


/*
void MainWindow::setupPages()
{
    setupMainPage();

  //  QWidget *bookListViewPage = new QWidget(this);
   // QVBoxLayout *bookListLayout = new QVBoxLayout(bookListViewPage);
   // bookListLayout->addWidget(ui->tableView);

    // Placeholder pages for now
    favoritesPage = new QLabel("Favorites Page");
    notesPage = new QLabel("Notes Page");
    bookPage = new QLabel("Book page"); //bookListViewPage;

    stackedWidget->addWidget(mainPage);
    stackedWidget->addWidget(favoritesPage);
    stackedWidget->addWidget(notesPage);
    stackedWidget->addWidget(bookPage);

    stackedWidget->setCurrentWidget(mainPage);
}

void MainWindow::addPage(QWidget* page, const QString& name)
{
    stackedWidget->addWidget(page);
    // for named lookup: map<QString, QWidget*>
}

void MainWindow::showMainPage()
{
    stackedWidget->setCurrentWidget(mainPage);
}
void MainWindow::showFavoritesPage()
{
    stackedWidget->setCurrentWidget(favoritesPage);
}
void MainWindow::showNotesPage()
{
    stackedWidget->setCurrentWidget(notesPage);
}
void MainWindow::showBookPage()
{
    stackedWidget->setCurrentWidget(bookPage);
}

void MainWindow::setupDatabase()
{
    QString dbPath = QDir::currentPath() + "/books.db";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS books ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "title TEXT NOT NULL,"
               "author TEXT NOT NULL,"
               "year INTEGER)");

    query.exec("SELECT COUNT(*) FROM books");
    query.first();

    int bookCount = query.value(0).toInt();

    if (bookCount == 0) {
        fetchAndInsertBooks();
    }
}

void MainWindow::setupModel()
{
    model = new QSqlTableModel(this, db);
    model->setTable("books");
    model->select();
    ui->tableView->setModel(model);

    ui->tableView->resizeColumnsToContents();

    ui->tableView->hideColumn(0);
    ui->tableView->setColumnWidth(3, 80);

    ui->tableView->horizontalHeader()->setSectionsMovable(true);

    QFont headerFont = ui->tableView->horizontalHeader()->font();
    headerFont.setBold(true);
    ui->tableView->horizontalHeader()->setFont(headerFont);

    for (int i = 1; i < model->columnCount(); ++i) {
        if (i != 3) {
            ui->tableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }

        QString headerText = model->headerData(i, Qt::Horizontal).toString();
        model->setHeaderData(i, Qt::Horizontal, headerText.toUpper());
    }
}

void MainWindow::fetchAndInsertBooks()
{
    manager = new QNetworkAccessManager(this);

    QUrl url("https://www.googleapis.com/books/v1/volumes?q=fiction+OR+non-fiction&maxResults=40");
    QNetworkRequest request(url);

    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::onFetchFinished);

    manager->get(request);
}

void MainWindow::onFetchFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error fetching data:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray items = jsonObject["items"].toArray();

    parseAndInsertBooks(items);

    reply->deleteLater();
}

void MainWindow::parseAndInsertBooks(const QJsonArray &items)
{
    QSqlQuery query;
    for (const QJsonValue &value : items) {
        QJsonObject book = value.toObject();
        QJsonObject volumeInfo = book["volumeInfo"].toObject();

        QString title = volumeInfo["title"].toString();
        QString author = volumeInfo["authors"].toArray().first().toString();
        int year = volumeInfo["publishedDate"].toString().left(4).toInt();

        query.prepare("INSERT INTO books (title, author, year) VALUES (?, ?, ?)");
        query.addBindValue(title);
        query.addBindValue(author);
        query.addBindValue(year);

        if (!query.exec()) {
            qDebug() << "Error inserting book:" << query.lastError().text();
        }
    }

    model->select();
}

void MainWindow::setupMainPage()
{
    mainPage = new QWidget(this);
    auto *mainLay = new QVBoxLayout(mainPage);

    //header
    auto *hdr = new QLabel("<h1>Welcome to ShelfSpace</h1>", this);
    hdr->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(hdr);

    QListWidget *randomBookList = new QListWidget(this);
    randomBookList->setStyleSheet("border: none;");

    //hardcoded:
    auto *randomBooksLayout = new QVBoxLayout();
    randomBooksLayout->setContentsMargins(0, 10, 0, 0); // Add some top margin

    QLabel *book1 = new QLabel("The Lord of the Rings", this);
    book1->setAlignment(Qt::AlignCenter);
    randomBooksLayout->addWidget(book1);

    QLabel *book2 = new QLabel("1984", this);
    book2->setAlignment(Qt::AlignCenter);
    randomBooksLayout->addWidget(book2);

    QLabel *book3 = new QLabel("The Hitchhiker's Guide to the Galaxy", this);
    book3->setAlignment(Qt::AlignCenter);
    randomBooksLayout->addWidget(book3);

    mainLay->addLayout(randomBooksLayout);
    mainLay->addStretch();

    mainPage->setLayout(mainLay);

    //database:
    /*
    QSqlDatabase db = QSqlDatabase::database("MAINPAGE"); // Get the existing connection
    if (!db.isValid())
    {
        db = QSqlDatabase::addDatabase("QSQLITE", "MAINPAGE");
        db.setDatabaseName("ShelfSpace.db");
        if (!db.open()) qDebug() << db.lastError().text();
    }

    QSqlQuery q(db);
    if (q.exec("SELECT title FROM books ORDER BY RANDOM() LIMIT 3")) {
        while (q.next()) {
            randomBookList->addItem(q.value(0).toString());
        }
    } else {
        qDebug() << "Error fetching random books:" << q.lastError().text();
    }
    db.close(); // Close the specific connection


    mainLay->addWidget(randomBookList);
    mainLay->addStretch();

    mainPage->setLayout(mainLay);

*/

/*
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) //override (?)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        if (auto *lbl = qobject_cast<QLabel*>(watched)) {
            // lbl->objectName() is the imageUrl; look up corresponding bookId
            // For simplicity: re-query the DB for this URL
            QSqlQuery q;
            q.prepare("SELECT id FROM tbBooks WHERE image = ?");
            q.addBindValue(lbl->objectName());
            if (q.exec() && q.next()) {
                QString bookId = q.value(0).toString();
                //BookDetailsWindow dlg(bookId, this);
                //dlg.exec();
            }
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

*/
