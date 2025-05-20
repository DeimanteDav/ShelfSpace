#include "labeledbutton.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPixmap>
#include <QIcon>
#include <QDebug>

class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr) : QLabel(parent) {
        setCursor(Qt::PointingHandCursor);
    }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override {
        emit clicked();
        QLabel::mousePressEvent(event);
    }
};

LabeledButton::LabeledButton(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft);

    iconButton = new QPushButton;
    iconButton->setFlat(true);
    iconButton->setStyleSheet("border: none;");

    textLabel = new ClickableLabel;

    layout->addWidget(iconButton);
    layout->addWidget(textLabel);

    connect(iconButton, &QPushButton::clicked, this, &LabeledButton::handleClick);
    connect(textLabel, &ClickableLabel::clicked, this, &LabeledButton::handleClick);
}


//Temporary since we need to load from database url
void LabeledButton::setIconFromFile(const QString &iconPath, const QSize &iconSize) {
    QPixmap pix(iconPath);
    iconButton->setIcon(QIcon(pix));
    iconButton->setIconSize(iconSize);
    iconButton->setFixedSize(iconSize);
}

void LabeledButton::setIconFromUrl(const QString& url, const QSize& iconSize) {
    // Create a temporary manager (will be auto-deleted with QObject parenting)
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);

    QUrl imageUrl(url);
    QNetworkRequest request(imageUrl);
    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        manager->deleteLater();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QPixmap pixmap;
            if (pixmap.loadFromData(data)) {
                iconButton->setIcon(QIcon(pixmap));
                iconButton->setIconSize(iconSize);
            }
        } else {
            qDebug() << "Failed to download icon:" << reply->errorString();
        }
    });
}


void LabeledButton::setLabelText(const QString &text) {
    textLabel->setAlignment(Qt::AlignLeft);
    textLabel->setText(text);
}

void LabeledButton::setTotalSize(const QSize &size) {
    setFixedSize(size);
}

void LabeledButton::handleClick() { // Should open single book page TODO
    emit clicked();
}

#include "labeledbutton.moc"
