#include "mainwindow.h"
#include "ui_mainwindow.h"

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("ShelfSpace");

    setupDatabase();
    setupModel();
}

MainWindow::~MainWindow()
{
    delete ui;
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
