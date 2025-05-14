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

    networkManager = new QNetworkAccessManager(this);

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
        coverLabel->setFixedSize(100, 150);
        coverLabel->setAlignment(Qt::AlignCenter);
        coverLabel->setText("Loading...");
        bookLayout->addWidget(coverLabel); // placeholder

        QUrl imageUrlWithId(imageUrl);
        QNetworkRequest request(imageUrlWithId);
        QNetworkReply *reply = networkManager->get(request);

        connect(reply, &QNetworkReply::finished, this, [reply, coverLabel]() {
            if (reply->error() == QNetworkReply::NoError) {
                QPixmap pixmap;
                pixmap.loadFromData(reply->readAll());
                if (!pixmap.isNull()) {
                    coverLabel->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
                    coverLabel->setText("");
                }
            } else {
                coverLabel->setText("Image\nfailed");
                qDebug() << "Failed to load image:" << reply->url().toString();
            }
            reply->deleteLater();
        });
        networkManager->get(QNetworkRequest(QUrl(imageUrl)));
        //coverLabel->setPixmap(pixmap.scaled(80, 100, Qt::KeepAspectRatio));

        QLabel *titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


        QPushButton *favoriteButton = new QPushButton();
        favoriteButton->setCheckable(true);

        bool isFavorite = isBookFavorite(bookId);
        favoriteButton->setChecked(isFavorite);
        favoriteButton->setText(isFavorite ? "★" : "☆");
        //favoriteButton->setCheckable(true);
        favoriteButton->setToolTip("Add/Remove from Favorites");
        favoriteButton->setFixedWidth(30);

        connect(favoriteButton, &QPushButton::clicked, this, [this, bookId, favoriteButton]() {
            QSqlDatabase db = QSqlDatabase::database();

            if (favoriteButton->isChecked()) {
                addToFavorites(bookId);
                favoriteButton->setText("★");
            } else {
                removeFromFavorites(bookId);
                favoriteButton->setText("☆");
            }
        });


        QPushButton *infoButton = new QPushButton("Info");
        connect(infoButton, &QPushButton::clicked, this, [this, bookId, title]() {
            QMessageBox::information(this, "Book Info", "Open single-book window for: " + title);
            // Later: emit signal or switch to single-book widget
        });

        //bookLayout->addWidget(coverLabel);
        bookLayout->addWidget(titleLabel);
        bookLayout->addWidget(favoriteButton);
        bookLayout->addWidget(infoButton);
        bookWidget->setLayout(bookLayout);

        scrollLayout->addWidget(bookWidget);
    }

    scrollLayout->addStretch();
}

bool MainWindow::isBookFavorite(int bookId) {

    QSqlQuery check(QSqlDatabase::database());
    check.prepare("SELECT COUNT(*) FROM tbFavorites WHERE bookId = :id");
    check.bindValue(":id", bookId);
    if (!check.exec() || !check.next())
        return false;
    return check.value(0).toInt() > 0;

    /*
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM tbFavorites WHERE bookId = ?");
    checkQuery.addBindValue(bookId);
    if (!checkQuery.exec()) {
        qDebug() << "Failed to check favorite:" << checkQuery.lastError().text();
        return false;
    }
    if (checkQuery.next()) {
        return checkQuery.value(0).toInt() > 0;
    }
    return false;
*/
}

void MainWindow::addToFavorites(int bookId) {
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO tbFavorites (bookId) VALUES (:id)");
    insertQuery.addBindValue(bookId);
    if (!insertQuery.exec()) {
        qDebug() << "Failed to add to favorites:" << insertQuery.lastError().text();
    }
}

void MainWindow::removeFromFavorites(int bookId) {
    QSqlQuery deleteQuery;
    deleteQuery.prepare("DELETE FROM tbFavorites WHERE bookId = ?");
    deleteQuery.addBindValue(bookId);
    if (!deleteQuery.exec()) {
        qDebug() << "Failed to remove from favorites:" << deleteQuery.lastError().text();
    }
}

void MainWindow::handleBookClicked(QListWidgetItem *item) {
    QString bookTitle = item->text();
    qDebug() << "Selected book:" << bookTitle;

    // TODO: Replace this with logic to show the real single-book page later
    QMessageBox::information(this, "Book Selected", "You selected: " + bookTitle);

    // In the future:
    // stackedWidget->setCurrentWidget(singleBookWidget);
    // singleBookWidget->loadBook(bookTitle); // or pass book ID, etc.
}

