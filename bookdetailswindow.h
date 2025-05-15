#ifndef BOOKDETAILSWINDOW_H
#define BOOKDETAILSWINDOW_H

#include <QDialog>
#include <QString>
#include <QVBoxLayout>

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
    void loadBookDetails();

    QString currentBookId;
    QPushButton* btnFavorite;
    QVBoxLayout* reviewsLayout = nullptr;
    void updateFavoriteButton();
    void toggleFavorite();
    void submitReview();
};

#endif // BOOKDETAILSWINDOW_H
