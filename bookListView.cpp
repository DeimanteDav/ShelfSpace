#include "booklistview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

BookListView::BookListView(QWidget *parent) : QWidget(parent)
{
    setupUI();
    setupTable();
    loadBooks();
}

void BookListView::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    tableWidget = new QTableWidget(this);
    mainLayout->addWidget(tableWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("Add Book", this);
    removeButton = new QPushButton("Remove Book", this);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    mainLayout->addLayout(buttonLayout);

    connect(addButton, &QPushButton::clicked, this, &BookListView::onAddClicked);
    connect(removeButton, &QPushButton::clicked, this, &BookListView::onRemoveClicked);
    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &BookListView::onBookDoubleClicked);
}

void BookListView::setupTable()
{
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "ID" << "Title" << "Genre" << "Year");
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void BookListView::loadBooks()
{
    tableWidget->setRowCount(0);

    QSqlQuery query("SELECT id, title, genre, year FROM tbBooks ORDER BY title");
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value("title").toString()));
        tableWidget->setItem(row, 2, new QTableWidgetItem(query.value("genre").toString()));
        tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("year").toString()));
        ++row;
    }
}

void BookListView::onAddClicked()
{
    QString id = QInputDialog::getText(this, "Add Book", "Enter Book ID:");
    if (id.isEmpty()) return;

    QString title = QInputDialog::getText(this, "Add Book", "Enter Book Title:");
    if (title.isEmpty()) return;

    QString genre = QInputDialog::getText(this, "Add Book", "Enter Genre:");
    QString yearStr = QInputDialog::getText(this, "Add Book", "Enter Year:");
    int year = yearStr.toInt();

    QSqlQuery query;
    query.prepare("INSERT INTO tbBooks (id, title, genre, year) VALUES (:id, :title, :genre, :year)");
    query.bindValue(":id", id);
    query.bindValue(":title", title);
    query.bindValue(":genre", genre);
    query.bindValue(":year", year);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    loadBooks();
}

void BookListView::onRemoveClicked()
{
    int row = tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Remove Book", "No book selected.");
        return;
    }

    QString id = tableWidget->item(row, 0)->text();
    QSqlQuery query;
    query.prepare("DELETE FROM tbBooks WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    loadBooks();
}

void BookListView::onBookDoubleClicked(int row, int)
{
    QString title = tableWidget->item(row, 1)->text();
    QMessageBox::information(this, "Book Info", "You double-clicked: " + title);
}
