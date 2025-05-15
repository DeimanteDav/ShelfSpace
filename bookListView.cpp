#include "bookListView.h"
#include "ui_bookListView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QIcon>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

BookListView::BookListView(QWidget *parent) :
    QWidget(parent)
{
    networkManager = new QNetworkAccessManager(this);
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
    tableWidget->setIconSize(QSize(64, 64));
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

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));      // ID
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value("title").toString()));   // Title
        tableWidget->setItem(row, 2, new QTableWidgetItem(query.value("author").toString()));  // Author
        tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("genre").toString()));   // Genre
        tableWidget->setItem(row, 4, new QTableWidgetItem(query.value("year").toString()));    // Year

        QString imageUrl = query.value("image").toString();
        QTableWidgetItem *imageItem = new QTableWidgetItem("Loading...");
        tableWidget->setItem(row, 5, imageItem);

        // Capture row index with lambda
        QNetworkRequest request(imageUrl);
        QNetworkReply *reply = networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [=]() {
            QPixmap pixmap;
            if (pixmap.loadFromData(reply->readAll())) {
                imageItem->setIcon(QIcon(pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
                imageItem->setText("");
                tableWidget->resizeRowToContents(row);
            } else {
                imageItem->setText("Image load failed");
            }
            reply->deleteLater();
        });

        ++row;
    }

    tableWidget->setIconSize(QSize(64, 64));
}

void BookListView::onRemoveClicked()
{
    int row = tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Remove Book", "No book selected.");
        return;
    }

    QString bookTitle = tableWidget->item(row, 1)->text();
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Removal",
        "Are you sure you want to remove \"" + bookTitle + "\" from your favorites?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply != QMessageBox::Yes) {
        return; // User cancelled
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
