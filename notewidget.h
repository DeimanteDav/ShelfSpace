#ifndef NOTEWIDGET_H
#define NOTEWIDGET_H

#include <QWidget>

class QLineEdit;
class QTextEdit;
class QPushButton;

class NoteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoteWidget(QWidget *parent = nullptr);
    QString bookId() const;

    QDateTime dateCreated() const;

    QString title() const;
    QString content() const;

    void setTitle(QString newTitle);
    void setContent(QString newContent);


signals:
    void noteSaved(const QString &title, const QString &content);

private slots:
    void handleSave();
    void loadNote();


private:
    QLineEdit *titleEdit;
    QTextEdit *contentEdit;
    QPushButton *saveButton;
    QPushButton *loadButton;
    int testid = 1;

    int id() const;

};

#endif // NOTEWIDGET_H
