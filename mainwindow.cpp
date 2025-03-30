#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDebug>
#include <QFont>

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
        if (i!= 3) {
            ui->tableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }

        QString headerText = model->headerData(i, Qt::Horizontal).toString();
        model->setHeaderData(i, Qt::Horizontal, headerText.toUpper());
    }
}
