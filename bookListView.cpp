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
#include <QLabel>

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

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search by title, author, genre or year...");
    mainLayout->addWidget(searchEdit);

    tableWidget = new QTableWidget(this);
    mainLayout->addWidget(tableWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    removeButton = new QPushButton("Remove Book", this);
    buttonLayout->addWidget(removeButton);
    mainLayout->addLayout(buttonLayout);

    connect(removeButton, &QPushButton::clicked, this, &BookListView::onRemoveClicked);
    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &BookListView::onBookDoubleClicked);
    connect(searchEdit, &QLineEdit::textChanged, this, &BookListView::onSearchTextChanged);
}

void BookListView::setupTable()
{
    tableWidget->setColumnCount(5);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Title" << "Author" << "Genre" << "Year" << "Image");

    QHeaderView *header = tableWidget->horizontalHeader();
    header->setStretchLastSection(false);

    header->setSectionResizeMode(QHeaderView::Interactive);

    tableWidget->setColumnWidth(0, 247); // Title
    tableWidget->setColumnWidth(1, 240); // Author
    tableWidget->setColumnWidth(2, 140); // Genre
    tableWidget->setColumnWidth(3, 60);  // Year
    tableWidget->setColumnWidth(4, 66);  // Image

    tableWidget->setIconSize(QSize(64, 64));
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void BookListView::loadBooks()
{
    tableWidget->setRowCount(0);

    QSqlQuery query(R"(
    SELECT b.title, b.author, b.genre, b.year, b.image
    FROM tbFavorites f
    JOIN tbBooks b ON f.bookId = b.id
    ORDER BY b.title
    )");

    int row = 0;
    while (query.next()) {
        QString title = query.value("title").toString();
        QString author = query.value("author").toString();
        QString genre = query.value("genre").toString();
        QString year = query.value("year").toString();
        QString imageUrl = query.value("image").toString();

        // Filter here if search text is active
        QString filterText = searchEdit->text().trimmed();
        if (!filterText.isEmpty()) {
            QString allText = title + " " + author + " " + genre + " " + year;
            if (!allText.contains(filterText, Qt::CaseInsensitive)) {
                continue;
            }
        }

        tableWidget->insertRow(row);
        tableWidget->setRowHeight(row, 70);

        QTableWidgetItem *titleItem = new QTableWidgetItem(query.value("title").toString());
        QFont titleFont = titleItem->font();
        titleFont.setBold(true);
        titleItem->setFont(titleFont);
        tableWidget->setItem(row, 0, titleItem);

        QTableWidgetItem *authorItem = new QTableWidgetItem(query.value("author").toString());
        QFont authorFont = authorItem->font();
        authorFont.setItalic(true);
        authorItem->setFont(authorFont);
        tableWidget->setItem(row, 1, authorItem);

        tableWidget->setItem(row, 2, new QTableWidgetItem(query.value("genre").toString()));
        tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("year").toString()));

        QLabel *imageLabel = new QLabel();
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setFixedSize(66, 66);
        tableWidget->setCellWidget(row, 4, imageLabel);

        // Capture row index with lambda
        QNetworkRequest request(imageUrl);
        QNetworkReply *reply = networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [=]() {
            QPixmap pixmap;
            if (pixmap.loadFromData(reply->readAll())) {
                QPixmap scaled = pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                imageLabel->setPixmap(scaled);
            } else {
                imageLabel->setText("X");
                imageLabel->setAlignment(Qt::AlignCenter);
            }
            reply->deleteLater();
        });

        ++row;
    }
}

void BookListView::onSearchTextChanged(const QString &text)
{
    Q_UNUSED(text);
    loadBooks();  // Reload with filter applied
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
