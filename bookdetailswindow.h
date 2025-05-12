#ifndef BOOKDETAILSWINDOW_H
#define BOOKDETAILSWINDOW_H

#include <QDialog>
#include <QString>

namespace Ui {
class BookDetailsWindow;
}

class BookDetailsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BookDetailsWindow(QString bookId, QWidget *parent = nullptr);
    ~BookDetailsWindow();

private:
    Ui::BookDetailsWindow *ui;
    void loadBookDetails(const QString &bookId);
};

#endif // BOOKDETAILSWINDOW_H
