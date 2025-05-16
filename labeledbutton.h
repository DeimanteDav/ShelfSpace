#ifndef LABELEDBUTTON_H
#define LABELEDBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSize>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class ClickableLabel;

class LabeledButton : public QWidget {
    Q_OBJECT

public:
    explicit LabeledButton(QWidget *parent = nullptr);
    void setIconFromFile(const QString &iconPath, const QSize &iconSize);
    void setLabelText(const QString &text);
    void setTotalSize(const QSize &size);

signals:
    void clicked();

private slots:
    void handleClick();

private:
    QPushButton *iconButton;
    ClickableLabel *textLabel;
};

#endif // LABELEDBUTTON_H
