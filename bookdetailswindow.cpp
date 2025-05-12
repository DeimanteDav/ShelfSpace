#include "bookdetailswindow.h"
#include "ui_bookdetailswindow.h"  // Ensure this is included

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

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("ShelfSpace.db");  // Path to your database file

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
    } else {
        qDebug() << "Database connected!";
    }

    ui->setupUi(this);  // Initialize UI
    loadBookDetails(bookId);  // Load book details
}

BookDetailsWindow::~BookDetailsWindow()
{
    delete ui;  // Clean up UI resources
}

void BookDetailsWindow::loadBookDetails(const QString &bookId)
{
    QSqlQuery query;
    query.prepare("SELECT title, year, image, genre, description, reviewsId FROM tbBooks WHERE id = ?");
    query.addBindValue(bookId);

    if (query.exec() && query.next()) {
        QString title = query.value(0).toString();
        int year = query.value(1).toInt();
        QString imageUrl = query.value(2).toString();
        QString genre = query.value(3).toString();
        QString description = query.value(4).toString();
        QString reviewsId = query.value(5).toString();

        ui->lblTitleValue->setText("<h2>" + title + "</h2>");
        ui->lblYearValue->setText(QString::number(year));
        ui->lblGenreValue->setText(genre);
        ui->lblDescriptionValue->setText(description);
        ui->lblReviewsIdValue->setText(reviewsId);

        // Load image from URL or local path
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
            if (reply->error() == QNetworkReply::NoError) {
                QPixmap pixmap;
                pixmap.loadFromData(reply->readAll());
                ui->lblImage->setPixmap(pixmap.scaledToWidth(200, Qt::SmoothTransformation));
            } else {
                qDebug() << "Image load error:" << reply->errorString();
            }
            reply->deleteLater();
        });
        manager->get(QNetworkRequest(QUrl(imageUrl)));
    } else {
        qDebug() << "Failed to load book:" << query.lastError().text();
    }

    // Fetch and display reviews
    QSqlQuery reviewsQuery;
    reviewsQuery.prepare("SELECT dateCreated, text FROM tbReviews WHERE bookId = ?");
    reviewsQuery.addBindValue(bookId);

    if (reviewsQuery.exec()) {
        while (reviewsQuery.next()) {
            QString date = reviewsQuery.value(0).toString();
            QString text = reviewsQuery.value(1).toString();

            QLabel *reviewLabel = new QLabel(this);
            reviewLabel->setWordWrap(true);
            reviewLabel->setText("<b>Posted on:</b> " + date + "<br>" + text);
            ui->reviewsLayout->addWidget(reviewLabel);  // This layout should be inside a scroll area
        }
    } else {
        qDebug() << "Failed to load reviews:" << reviewsQuery.lastError().text();
    }

}
