#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bookdetailswindow.h"
#include "databasemanager.h"
#include "bookListView.h"
#include "noteswidget.h"

#include <QSqlQuery>
#include <QSqlError>
//#include <QDir>
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
//#include <QEvent>
//#include <QListWidget>
#include <QPushButton>
#include <QMenuBar>
//#include <QApplication>
//#include <qmessagebox.h>
#include <QScrollArea>
#include <QPointer>
#include <QLineEdit>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stackedWidget(new QStackedWidget(this))
    , addBookWidget(new QWidget(this))
    , viewBooksWidget(new QWidget(this))
{
    ui->setupUi(this);

    setupDatabase();

    networkManager = new QNetworkAccessManager(this);

    bookListView = new BookListView(this);
    bookListView->setDatabase(DatabaseManager::instance().database());

    setupCentralViews();

    setupMenu();

    setCentralWidget(stackedWidget);

    setWindowTitle("ShelfSpace");
    resize(800, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMenu()
{
    // Home Button
    QAction *actionHome = new QAction("Home", this);
    connect(actionHome, &QAction::triggered, [this]() {
        loadAllBooks();
        stackedWidget->setCurrentWidget(mainPageWidget);
    });
    menuBar()->addAction(actionHome);

    // View Books (Collection) Button
    actionViewBooks = new QAction("&View Collection", this);
    connect(actionViewBooks, &QAction::triggered, this, &MainWindow::showBookListView);
    menuBar()->addAction(actionViewBooks);

    // Notes Button
    actionShowNotes = new QAction("Open Notes", this);
    connect(actionShowNotes, &QAction::triggered, this, &MainWindow::showNotesView);
    menuBar()->addAction(actionShowNotes);

    // Exit Button
    actionExit = new QAction("&Exit", this);
    connect(actionExit, &QAction::triggered, this, &MainWindow::exitApplication);
    menuBar()->addAction(actionExit);
}


void MainWindow::setupCentralViews() {

    mainPageWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QLabel *titleLabel = new QLabel("Welcome to ShelfSpace");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Arial", 20, QFont::Bold));
    mainLayout->addWidget(titleLabel);

    // Search Bar
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("Search books (title, author, year or genre)...");
    connect(searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::filterBooks);
    mainLayout->addWidget(searchLineEdit);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    scrollWidget = new QWidget();
    scrollLayout = new QVBoxLayout(scrollWidget);

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);

    loadAllBooks();

    mainPageWidget->setLayout(mainLayout);

    //View Books (Collection)
    QVBoxLayout *viewLayout = new QVBoxLayout();
    viewLayout->addWidget(new QLabel("View Books Table (placeholder)", this));
    viewBooksWidget->setLayout(viewLayout);

    // for Notes
    notesWidget = new class notesWidget;

    notesScrollArea = new QScrollArea;
    notesScrollArea->setWidget(notesWidget);
    notesScrollArea->setWidgetResizable(true);
    notesScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    notesScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    stackedWidget->addWidget(viewBooksWidget);
    stackedWidget->addWidget(addBookWidget);
    stackedWidget->addWidget(mainPageWidget);
    stackedWidget->addWidget(notesScrollArea);
    stackedWidget->addWidget(bookListView);

    stackedWidget->setCurrentWidget(mainPageWidget); // default view
}

void MainWindow::showBookListView() {
    bookListView->loadBooks();
    stackedWidget->setCurrentWidget(bookListView);
}

void MainWindow::showNotesView() {
    //FIX ME: doesnt update data
    stackedWidget->setCurrentWidget(notesScrollArea); // Switch to the "Notes" page
}

void MainWindow::exitApplication() {
    QApplication::quit();
}

void MainWindow::setupDatabase() {
    QSqlDatabase db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
    } else {
        qDebug() << "Database connection reused successfully";
    }

    //Write-Ahead Logging (WAL)
    QSqlQuery query(db);
    query.exec("PRAGMA journal_mode=WAL;");
    if (query.lastError().isValid()) {
        qDebug() << "Failed to enable WAL mode:" << query.lastError().text();
    }
    qDebug() << "WAL mode status:" << query.lastError().text();
}

void MainWindow::loadAllBooks() {
    // Abort image downloads from previous
    qDebug() << "Number of active replies before abort:" << activeReplies.size();
    for (QNetworkReply *reply : activeReplies) {
        if (!reply) {
            qDebug() << "Warning: activeReplies contains a null pointer!";
            continue;
        }
        if (reply && reply->isRunning()) {
            reply->abort();
            qDebug() << "Aborted network reply:" << reply->url().toString();
        }
        reply->deleteLater();
    }
    activeReplies.clear();

    QLayoutItem *child;
    while ((child = scrollLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    QSqlQuery query("SELECT id, title, author, image FROM tbBooks");

    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString bookId = query.value("id").toString();
        QString title = query.value("title").toString();
        QString author = query.value("author").toString();
        QString imageUrl = query.value("image").toString();

        addBookToLayout(bookId, title, author, imageUrl);
    }

    scrollLayout->addStretch();
    query.finish();
}

void MainWindow::filterBooks(const QString &searchText) {
    for (QNetworkReply *reply : activeReplies) {
        if (reply && reply->isRunning()) {
            reply->abort();
            reply->deleteLater();
        }
    }
    activeReplies.clear();

    QLayoutItem *child;
    while ((child = scrollLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    QString searchTextLower = searchText.toLower();
    QSqlQuery query;
    query.prepare("SELECT id, title, author, image FROM tbBooks WHERE LOWER(title) LIKE :pattern OR LOWER(author) LIKE :pattern OR LOWER(genre) LIKE :pattern OR LOWER(year) LIKE :pattern");
    query.bindValue(":pattern", "%" + searchTextLower + "%");

    if (!query.exec()) {
        qDebug() << "Failed to execute filter query:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString bookId = query.value("id").toString();
        QString title = query.value("title").toString();
        QString author = query.value("author").toString();
        QString imageUrl = query.value("image").toString();

        addBookToLayout(bookId, title, author, imageUrl);
    }

    scrollLayout->addStretch();
    query.finish();
}

void MainWindow::addBookToLayout(const QString& bookId, const QString& title, const QString& author, const QString& imageUrl) {
    QWidget *bookWidget = new QWidget();
    QHBoxLayout *bookLayout = new QHBoxLayout(bookWidget);

    QPointer<QLabel> coverLabel = new QLabel();
    coverLabel->setFixedSize(100, 150);
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setText("Loading...");
    bookLayout->addWidget(coverLabel);

    QUrl imageUrlWithId(imageUrl);
    QNetworkRequest request(imageUrlWithId);
    QNetworkReply *reply = networkManager->get(request);
    activeReplies.append(reply);

    connect(reply, &QNetworkReply::finished, this, [this, reply, coverLabel]() {
        if(coverLabel)
        {
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
        } else {
            qDebug() << "Warning: coverLabel was deleted before image finished loading for:" << reply->url().toString();
        }
        activeReplies.removeOne(reply);
        reply->deleteLater();
    });

    QWidget *infoContainer = new QWidget();
    QVBoxLayout *infoLayout = new QVBoxLayout(infoContainer);
    infoLayout->setSpacing(2);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QLabel *titleAuthorLabel = new QLabel();
    QString richText = QString("<div style='line-height: 1.0;'><b style='font-size: 16px;'>%1</b><br/><span style='font-style: italic; font-size: 14px; color: gray;'>by %2</span></div>")
                           .arg(title).arg(author);
    titleAuthorLabel->setText(richText);
    titleAuthorLabel->setWordWrap(true);
    //titleAuthorLabel->setMaximumWidth(520);
    titleAuthorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    infoLayout->addWidget(titleAuthorLabel);

    bookLayout->addWidget(infoContainer);

    // Add/remove button
    QPushButton *favoriteButton = new QPushButton();
    favoriteButton->setCheckable(true);
    bool isFavorite = isBookFavorite(bookId);
    favoriteButton->setChecked(isFavorite);
    favoriteButton->setText(isFavorite ? "★" : "☆");
    favoriteButton->setToolTip("Add/Remove from Favorites");
    favoriteButton->setFixedWidth(30);

    connect(favoriteButton, &QPushButton::clicked, this, [this, bookId, favoriteButton]() {
        qDebug() << "Favorite button clicked for book ID:" << bookId;
        if (favoriteButton->isChecked()) {
            addToFavorites(bookId);
            favoriteButton->setText("★");
        } else {
            removeFromFavorites(bookId);
            favoriteButton->setText("☆");
        }
    });

    // Connection to single-book-window
    QPushButton *infoButton = new QPushButton("Info");
    connect(infoButton, &QPushButton::clicked, this, [this, bookId, title]() {
        qDebug() << "Info button clicked for book ID:" << bookId << " Title:" << title;
        BookDetailsWindow *detailsWindow = new BookDetailsWindow(bookId, this);
        detailsWindow->setAttribute(Qt::WA_DeleteOnClose);
        detailsWindow->show();
    });

    bookLayout->addWidget(favoriteButton);
    bookLayout->addWidget(infoButton);

    bookWidget->setLayout(bookLayout);
    scrollLayout->addWidget(bookWidget);
}

bool MainWindow::isBookFavorite(const QString& bookId) {

    QSqlQuery check(QSqlDatabase::database());
    check.prepare("SELECT COUNT(*) FROM tbFavorites WHERE bookId = :id");
    check.bindValue(":id", bookId);
    if (!check.exec() || !check.next())
        return false;
    return check.value(0).toInt() > 0;
}

void MainWindow::addToFavorites(const QString& bookId) {
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO tbFavorites (bookId) VALUES (:id)");
    insertQuery.bindValue(":id", bookId);
    if (!insertQuery.exec()) {
        qDebug() << "Failed to add to favorites:" << insertQuery.lastError().text();
    }
}

void MainWindow::removeFromFavorites(const QString& bookId) {
    QSqlQuery deleteQuery;
    deleteQuery.prepare("DELETE FROM tbFavorites WHERE bookId = (:id)");
    deleteQuery.bindValue(":id", bookId);
    if (!deleteQuery.exec()) {
        qDebug() << "Failed to remove from favorites:" << deleteQuery.lastError().text();
        qDebug() << "bookId was:" << bookId;
    } else {
        qDebug() << "Successfully removed from favorites for bookId:" << bookId;
    }
}
