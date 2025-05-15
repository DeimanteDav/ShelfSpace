#include "bookdetailswindow.h"
#include "databasemanager.h"
#include "ui_bookdetailswindow.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QSqlDatabase>
#include <QSqlError>


BookDetailsWindow::BookDetailsWindow(QString bookId, QWidget *parent)
    : QDialog(parent), ui(new Ui::BookDetailsWindow)
{
    QSqlDatabase db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
    } else {
        qDebug() << "Database connection reused successfully";
    }

    this->currentBookId = bookId;

    ui->setupUi(this);

    QVBoxLayout *reviewsLayout = new QVBoxLayout(ui->scrollContentReviews);
    ui->scrollContentReviews->setLayout(reviewsLayout);
    reviewsLayout->setSpacing(15);
    reviewsLayout->setContentsMargins(10, 10, 10, 10);

    this->reviewsLayout = reviewsLayout;

    connect(ui->btnFavorite, &QPushButton::clicked, this, &BookDetailsWindow::toggleFavorite);
    connect(ui->btnSubmitReview, &QPushButton::clicked, this, &BookDetailsWindow::submitReview);

    updateFavoriteButton();
    loadBookDetails();
}

BookDetailsWindow::~BookDetailsWindow()
{
    delete ui;
}

void BookDetailsWindow::loadBookDetails()
{
    QSqlQuery query;
    query.prepare("SELECT title, author, year, image, genre, description FROM tbBooks WHERE id = ?");
    query.addBindValue(currentBookId);

    if (query.exec() && query.next()) {
        QString title = query.value(0).toString();
        QString author = query.value(1).toString();
        int year = query.value(2).toInt();
        QString imageUrl = query.value(3).toString();
        QString genre = query.value(4).toString();
        QString description = query.value(5).toString();

        ui->lblTitleValue->setText("<h2>" + title + "</h2>");
        ui->lblAuthorValue->setText(author);
        ui->lblYearValue->setText(QString::number(year));
        ui->lblGenreValue->setText(genre);
        ui->lblDescriptionValue->setText(description);

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
            if (reply->error() == QNetworkReply::NoError) {
                QPixmap pixmap;
                pixmap.loadFromData(reply->readAll());
                QPixmap scaledPixmap = pixmap.scaled(ui->lblImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                ui->lblImage->setPixmap(scaledPixmap);
            } else {
                qDebug() << "Image load error:" << reply->errorString();
            }
            reply->deleteLater();
        });
        manager->get(QNetworkRequest(QUrl(imageUrl)));
    } else {
        qDebug() << "Failed to load book:" << query.lastError().text();
    }

    QSqlQuery reviewsQuery;
    reviewsQuery.prepare("SELECT dateCreated, text FROM tbReviews WHERE bookId = ? ORDER BY datetime(dateCreated) DESC");
    reviewsQuery.addBindValue(currentBookId);

    if (reviewsQuery.exec()) {
        while (reviewsQuery.next()) {
            QString rawDate = reviewsQuery.value(0).toString();
            QDateTime dateTime = QDateTime::fromString(rawDate, Qt::ISODateWithMs);
            if (!dateTime.isValid())
                dateTime = QDateTime::fromString(rawDate, Qt::ISODate);

            QString formattedDate = dateTime.toString("dd MMM yyyy, HH:mm");
            QString text = reviewsQuery.value(1).toString();

            QLabel *reviewLabel = new QLabel(this);
            reviewLabel->setWordWrap(true);
            reviewLabel->setText("<b>" + formattedDate + "</b><br>" + text);
            this->reviewsLayout->addWidget(reviewLabel);
        }
    } else {
        qDebug() << "Failed to load reviews:" << reviewsQuery.lastError().text();
    }
}


void BookDetailsWindow::updateFavoriteButton()
{
    QSqlQuery query;
    query.prepare("SELECT id FROM tbFavorites WHERE bookId = ?");
    query.addBindValue(currentBookId);

    if (query.exec() && query.next()) {
        ui->btnFavorite->setText("★");
    } else {
        ui->btnFavorite->setText("☆");
    }
}

void BookDetailsWindow::toggleFavorite()
{
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT id FROM tbFavorites WHERE bookId = ?");
    checkQuery.addBindValue(currentBookId);

    if (checkQuery.exec() && checkQuery.next()) {
        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM tbFavorites WHERE bookId = ?");
        deleteQuery.addBindValue(currentBookId);

        if (!deleteQuery.exec()) {
            qDebug() << "Failed to remove from favorites:" << deleteQuery.lastError().text();
        }
    } else {
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO tbFavorites (bookId) VALUES (?)");
        insertQuery.addBindValue(currentBookId);

        if (!insertQuery.exec()) {
            qDebug() << "Failed to add to favorites:" << insertQuery.lastError().text();
        }
    }

    updateFavoriteButton();
}

void BookDetailsWindow::submitReview()
{
    QString reviewText = ui->txtReview->toPlainText().trimmed();
    if (reviewText.isEmpty())
        return;

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO tbReviews (bookId, text, dateCreated) VALUES (?, ?, datetime('now'))");
    insertQuery.addBindValue(currentBookId);
    insertQuery.addBindValue(reviewText);

    if (!insertQuery.exec()) {
        qDebug() << "Failed to insert review:" << insertQuery.lastError().text();
        return;
    }

    QLabel *reviewLabel = new QLabel(this);
    reviewLabel->setWordWrap(true);
    QString formattedDate = QDateTime::currentDateTime().toString("dd MMM yyyy, HH:mm");
    reviewLabel->setText("<b>" + formattedDate + "</b><br>" + reviewText);
    this->reviewsLayout->insertWidget(0, reviewLabel);

    ui->txtReview->clear();
}
