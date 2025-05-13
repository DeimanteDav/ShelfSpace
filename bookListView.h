#ifndef BOOKLISTVIEW_H
#define BOOKLISTVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

class BookListView : public QWidget
{
    Q_OBJECT

public:
    explicit BookListView(QWidget *parent = nullptr);
    void loadBooks();  // reload the table

private slots:
    void onAddClicked();
    void onRemoveClicked();
    void onBookDoubleClicked(int row, int column);

private:
    QTableWidget *tableWidget;
    QPushButton *addButton;
    QPushButton *removeButton;

    void setupUI();
    void setupTable();
};

#endif // BOOKLISTVIEW_H
