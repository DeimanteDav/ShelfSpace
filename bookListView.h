#ifndef BOOKLISTVIEW_H
#define BOOKLISTVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class BookListView : public QWidget
{
    Q_OBJECT

public:
    explicit BookListView(QWidget *parent = nullptr);
    void loadBooks();  // reload the table

private slots:
    void onRemoveClicked();
    void onBookDoubleClicked(int row, int column);
    void onSearchTextChanged(const QString &text);

private:
    QLineEdit *searchEdit;
    QTableWidget *tableWidget;
    QPushButton *removeButton;
    QNetworkAccessManager *networkManager;

    void setupUI();
    void setupTable();
};

#endif // BOOKLISTVIEW_H
