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

void LabeledButton::setLabelText(const QString &text) {
    textLabel->setAlignment(Qt::AlignLeft);
    textLabel->setText(text);
}

void LabeledButton::setTotalSize(const QSize &size) {
    setFixedSize(size);
}

void LabeledButton::handleClick() {
    emit clicked();
}


#include "labeledbutton.moc"
