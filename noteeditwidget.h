#ifndef NOTEEDITWIDGET_H
#define NOTEEDITWIDGET_H

#include <QWidget>

class QLineEdit;
class QTextEdit;
class QPushButton;

class NoteEditWidget : public QWidget
{
    Q_OBJECT

public:
    NoteEditWidget(QWidget *parent, QString bookId);
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

    QString id;

};

#endif // NOTEEDITWIDGET_H
