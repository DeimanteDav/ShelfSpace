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
#include <QPointer>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stackedWidget(new QStackedWidget(this)),  // Used to switch between different views (pages)
    addBookWidget(new QWidget(this)),           // Widget for adding new books (will be implemented by someone else)
    viewBooksWidget(new QWidget(this))
{
    ui->setupUi(this);

    setupDatabase();

    networkManager = new QNetworkAccessManager(this); // Manages network requests, in this case for downloading images
    setupCentralViews();             // Sets up the main layout and widgets in the center of the window

    setupMenu();

    setCentralWidget(stackedWidget); // Makes the stacked widget the main area to display content

    setWindowTitle("ShelfSpace");
    resize(800, 600);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMenu()
{
    // Home Button - Loads all books and shows the main book grid
    QAction *actionHome = new QAction("Home", this);
    connect(actionHome, &QAction::triggered, [this]() {
        loadAllBooks();
        stackedWidget->setCurrentWidget(mainPageWidget);
    });
    menuBar()->addAction(actionHome);

    // View Books (Collection) Button - Shows a view of all books (currently a placeholder)
    actionViewBooks = new QAction("&View Collection", this);
    connect(actionViewBooks, &QAction::triggered, this, &MainWindow::showViewBooksView);
    menuBar()->addAction(actionViewBooks);

    // Notes Button - Shows the notes window (will be implemented by someone)
    actionShowNotes = new QAction("Open Notes", this);
    connect(actionShowNotes, &QAction::triggered, this, &MainWindow::showNotesView);
    menuBar()->addAction(actionShowNotes);

    // Exit Button - Closes the app
    actionExit = new QAction("&Exit", this);
    connect(actionExit, &QAction::triggered, this, &MainWindow::exitApplication);
    menuBar()->addAction(actionExit);
}


void MainWindow::setupCentralViews() {

    mainPageWidget = new QWidget(this); // The main page displaying book covers
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QLabel *titleLabel = new QLabel("Welcome to ShelfSpace");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Arial", 20, QFont::Bold));
    mainLayout->addWidget(titleLabel);

    // Scroll area to contain the dynamically loaded book widgets
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
    stackedWidget->setCurrentWidget(viewBooksWidget); // Switch to the "View Collection" page
}

void MainWindow::showNotesView() {
    stackedWidget->setCurrentWidget(notesWidget); // Switch to the "Notes" page
}

void MainWindow::exitApplication() {
    QApplication::quit();
}

void MainWindow::setupDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("ShelfSpace.db");

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
    } else {
        qDebug() << "Database opened successfully at: ShelfSpace.db";
    }

    // Enable Write-Ahead Logging (WAL) for better database performance
    QSqlQuery query(db);
    query.exec("PRAGMA journal_mode=WAL;");
    if (query.lastError().isValid()) {
        qDebug() << "Failed to enable WAL mode:" << query.lastError().text();
    }
    qDebug() << "WAL mode status:" << query.lastError().text();
}

// Loads all book information from the database and displays them
void MainWindow::loadAllBooks() {
    // Abort any ongoing image downloads from the previous view to prevent issues
    qDebug() << "Number of active replies before abort:" << activeReplies.size();
    for (QNetworkReply *reply : activeReplies) {
        if (!reply) {
            qDebug() << "Warning: activeReplies contains a null pointer!";
            continue;
        }
        if (reply && reply->isRunning()) {
            reply->abort(); // Stop download
            qDebug() << "Aborted network reply:" << reply->url().toString();
        }
        reply->deleteLater(); // Clean up the reply object later
    }
    activeReplies.clear(); // Clear the list of active replies (downloads)

    // Clear old book widgets
    QLayoutItem *child;
    while ((child = scrollLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    // Query the database to retrieve all book details
    QSqlQuery query("SELECT id, title, author, image FROM tbBooks");

    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return;
    }

    // Loop through each book record from the database
    while (query.next()) {
        QString bookId = query.value("id").toString();
        QString title = query.value("title").toString();
        QString author = query.value("author").toString();
        QString imageUrl = query.value("image").toString();

        // Create a container widget for each book
        QWidget *bookWidget = new QWidget();
        QHBoxLayout *bookLayout = new QHBoxLayout(bookWidget);

        // Label to display the book cover (initially shows "Loading...")
        QPointer coverLabel = new QLabel();
        coverLabel->setFixedSize(100, 150);
        coverLabel->setAlignment(Qt::AlignCenter);
        coverLabel->setText("Loading...");
        bookLayout->addWidget(coverLabel); // Add the label to the book's layout

        // Start download of the book cover image
        QUrl imageUrlWithId(imageUrl);
        QNetworkRequest request(imageUrlWithId);
        QNetworkReply *reply = networkManager->get(request);
        activeReplies.append(reply); // Keep track of the download

        // Connect the 'finished' signal of the download to a lambda function to handle the image display
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
            activeReplies.removeOne(reply); // Remove the finished download from the active list
            reply->deleteLater(); // Schedule the network reply object for deletion
        });

        // Container for the book title and author, vertically
        QWidget *infoContainer = new QWidget();
        QVBoxLayout *infoLayout = new QVBoxLayout(infoContainer);
        infoLayout->setSpacing(2);
        infoLayout->setContentsMargins(0, 0, 0, 0);
        infoContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // Label to display the book title and author using Rich Text for formatting
        QLabel *titleAuthorLabel = new QLabel();
        QString richText = QString("<div style='line-height: 1.0;'><b style='font-size: 16px;'>%1</b><br/><span style='font-style: italic; font-size: 14px; color: gray;'>by %2</span></div>")
                               .arg(title).arg(author);
        titleAuthorLabel->setText(richText);
        titleAuthorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        infoLayout->addWidget(titleAuthorLabel);

        bookLayout->addWidget(infoContainer); // Add the title/author container to the horizontal layout

        // Button to add or remove the book from the favorites list
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

        // Button to view more details about the book - CONNECTION POINT FOR SINGLE-BOOK-WINDOW
        QPushButton *infoButton = new QPushButton("Info");
        connect(infoButton, &QPushButton::clicked, this, [this, bookId, title]() {
            qDebug() << "Info button clicked for book ID:" << bookId << " Title:" << title;
            // TODO: Here you will likely create and show the single-book-window
            // Example (assuming BookDetailsWindow takes a bookId):
            // BookDetailsWindow *detailsWindow = new BookDetailsWindow(bookId, this);
            // detailsWindow->show();
            QMessageBox::information(this, "Book Info", "Open single-book window for: " + title + "\n(Not yet implemented)");
        });

        bookLayout->addWidget(favoriteButton);
        bookLayout->addWidget(infoButton);

        bookWidget->setLayout(bookLayout);
        scrollLayout->addWidget(bookWidget); // Add the complete book widget to the scroll layout
    }

    scrollLayout->addStretch();
    query.finish(); // Close the database query
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
