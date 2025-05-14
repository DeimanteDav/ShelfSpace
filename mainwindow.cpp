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
#include <qmessagebox.h>
#include <QScrollArea>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stackedWidget(new QStackedWidget(this)),
    addBookWidget(new QWidget(this)),
    viewBooksWidget(new QWidget(this))
{
    ui->setupUi(this);



    setupDatabase();
    //setupModel();

    setupMenu();

    setupCentralViews();

    setCentralWidget(stackedWidget);

    setWindowTitle("ShelfSpace");
    resize(800, 600);

    //stackedWidget->setCurrentWidget(mainPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMenu()
{
    QMenu *homeMenu = menuBar()->addMenu("&Home");
    QAction *actionHome = new QAction("Home", this);
    connect(actionHome, &QAction::triggered, [this]() {
        loadAllBooks();
        stackedWidget->setCurrentWidget(mainPageWidget);
    });
    homeMenu->insertAction(actionExit, actionHome);

    QMenu *bookMenu = menuBar()->addMenu("&Book Collection");
    actionViewBooks = new QAction("&View Collection", this);
    connect(actionViewBooks, &QAction::triggered, this, &MainWindow::showViewBooksView);
    bookMenu->addAction(actionViewBooks);

    QMenu *notesMenu = menuBar()->addMenu("Notes");
    actionShowNotes = new QAction("Open Notes", this);
    connect(actionShowNotes, &QAction::triggered, this, &MainWindow::showNotesView);
    notesMenu->addAction(actionShowNotes);

    QMenu *exitMenu = menuBar()->addMenu("&Exit");
    actionExit = new QAction("&Exit", this);
    connect(actionExit, &QAction::triggered, this, &MainWindow::exitApplication);
    exitMenu->addAction(actionExit);
}

//cia vos ne main page
void MainWindow::setupCentralViews() {

    mainPageWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QLabel *titleLabel = new QLabel("Welcome to ShelfSpace");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Arial", 20, QFont::Bold));
    mainLayout->addWidget(titleLabel);

    //edit here:


    /*
    recommendedBooksList = new QListWidget(this);
    mainLayout->addWidget(recommendedBooksList);
    loadRecommendedBooks();

    connect(recommendedBooksList, &QListWidget::itemClicked, this, &MainWindow::handleBookClicked);
*/

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    scrollWidget = new QWidget();
    scrollLayout = new QVBoxLayout(scrollWidget);

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);

    loadAllBooks();

    mainPageWidget->setLayout(mainLayout);

    // Placeholder widget for View Books (Collection)
    QVBoxLayout *viewLayout = new QVBoxLayout();
    viewLayout->addWidget(new QLabel("View Books Table (placeholder)", this));
    viewBooksWidget->setLayout(viewLayout);

    // for Notes
    notesWidget = new QLabel("Notes page will be implemented here.");

    stackedWidget->addWidget(viewBooksWidget);
    stackedWidget->addWidget(addBookWidget);
    stackedWidget->addWidget(mainPageWidget);
    stackedWidget->addWidget(notesWidget);

    stackedWidget->setCurrentWidget(mainPageWidget); // default view
}

void MainWindow::showViewBooksView() {
    stackedWidget->setCurrentWidget(viewBooksWidget);
}

void MainWindow::showNotesView() {
    stackedWidget->setCurrentWidget(notesWidget);
}

void MainWindow::exitApplication() {
    QApplication::quit();
}

void MainWindow::setupDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //QString dbPath = QDir::currentPath() + "/ShelfSpace.db";  // Assumes .db is next to the executable
    //QString dbPath = QCoreApplication::applicationDirPath() + "/ShelfSpace.db"; // shouldn't matter where .db is
    db.setDatabaseName("ShelfSpace.db");

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
    } else {
        qDebug() << "Database opened successfully at: ShelfSpace.db";
    }
}

void MainWindow::loadAllBooks() {
    // Clear old book widgets -- ?
    QLayoutItem *child;
    while ((child = scrollLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    QSqlQuery query("SELECT id, title, image FROM tbBooks");

    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int bookId = query.value("id").toInt();
        QString title = query.value("title").toString();
        QString imageUrl = query.value("image").toString();

        QWidget *bookWidget = new QWidget();
        QHBoxLayout *bookLayout = new QHBoxLayout(bookWidget);

        QLabel *coverLabel = new QLabel();
        coverLabel->setFixedSize(80, 100);
        QPixmap pixmap;
        pixmap.loadFromData(QByteArray());  // Placeholder; we'll add real downloading later
        coverLabel->setPixmap(pixmap.scaled(80, 100, Qt::KeepAspectRatio));

        QLabel *titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QPushButton *favoriteButton = new QPushButton("☆");
        favoriteButton->setCheckable(true);
        favoriteButton->setToolTip("Add/Remove from Favorites");
        favoriteButton->setFixedWidth(30);

        QPushButton *infoButton = new QPushButton("Info");
        connect(infoButton, &QPushButton::clicked, this, [this, bookId, title]() {
            QMessageBox::information(this, "Book Info", "Open single-book window for: " + title);
            // Later: emit signal or switch to single-book widget
        });

        bookLayout->addWidget(coverLabel);
        bookLayout->addWidget(titleLabel);
        bookLayout->addWidget(favoriteButton);
        bookLayout->addWidget(infoButton);
        bookWidget->setLayout(bookLayout);

        scrollLayout->addWidget(bookWidget);
    }

    scrollLayout->addStretch();
}

/*
void MainWindow::loadRecommendedBooks() { //doesn't do what it has to
    recommendedBooksList->clear();

    QSqlQuery query("SELECT title FROM books ORDER BY RANDOM() LIMIT 5");

    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString title = query.value(0).toString();
        recommendedBooksList->addItem(title);
    }
}
*/

void MainWindow::handleBookClicked(QListWidgetItem *item) {
    QString bookTitle = item->text();
    qDebug() << "Selected book:" << bookTitle;

    // TODO: Replace this with logic to show the real single-book page later
    QMessageBox::information(this, "Book Selected", "You selected: " + bookTitle);

    // In the future:
    // stackedWidget->setCurrentWidget(singleBookWidget);
    // singleBookWidget->loadBook(bookTitle); // or pass book ID, etc.
}

/*
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
*/
