#include "bookListView.h"

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

BookListView::BookListView(QWidget *parent) :
    QWidget(parent)
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
    removeButton = new QPushButton("Remove Book", this);
    buttonLayout->addWidget(removeButton);
    mainLayout->addLayout(buttonLayout);

    connect(removeButton, &QPushButton::clicked, this, &BookListView::onRemoveClicked);
    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &BookListView::onBookDoubleClicked);
}

void BookListView::setupTable()
{
    tableWidget->setColumnCount(6);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "ID" << "Title" << "Author" << "Genre" << "Year" << "Image");
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void BookListView::loadBooks()
{
    tableWidget->setRowCount(0);

    QSqlQuery query(R"(
    SELECT f.id, b.title, b.author, b.genre, b.year, b.image
    FROM tbFavorites f
    JOIN tbBooks b ON f.bookId = b.id
    ORDER BY b.title
    )");

    tableWidget->setRowCount(0);
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));      // ID
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value("title").toString()));   // Title
        tableWidget->setItem(row, 2, new QTableWidgetItem(query.value("author").toString()));  // Author
        tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("genre").toString()));   // Genre
        tableWidget->setItem(row, 4, new QTableWidgetItem(query.value("year").toString()));    // Year
        tableWidget->setItem(row, 5, new QTableWidgetItem(query.value("image").toString()));   // Image
        ++row;
    }
}

void BookListView::onRemoveClicked()
{
    int row = tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Remove Book", "No book selected.");
        return;
    }

    QString favoriteId = tableWidget->item(row, 0)->text();
    QSqlQuery query;
    query.prepare("DELETE FROM tbFavorites WHERE id = :id");
    query.bindValue(":id", favoriteId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    loadBooks();
}

void BookListView::onBookDoubleClicked(int row, int)
{
    QString title = tableWidget->item(row, 1)->text();
    QString author = tableWidget->item(row, 2)->text();
    QMessageBox::information(this, "Book Info", "You double-clicked: " + title + "\n by " + author);
}
