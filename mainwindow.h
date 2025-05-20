#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bookListView.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStackedWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QVector>
#include <QListWidget>
#include <QScrollArea>
#include <QVBoxLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;

    void setupDatabase();

    QStackedWidget *stackedWidget;

    BookListView *bookListView;

    void setupMenu();
    void setupCentralViews();

    QWidget *mainPageWidget;
    QListWidget *recommendedBooksList;

    QWidget *addBookWidget;
    QWidget *viewBooksWidget;
    QWidget *notesWidget;
    QScrollArea* notesScrollArea; //for notesWdget

    QAction *actionAddBook;
    QAction *actionViewBooks;
    QAction *actionShowNotes;
    QAction *actionExit;

    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVBoxLayout *scrollLayout;

    bool isBookFavorite(const QString& bookId);
    void addToFavorites(const QString& bookId);
    void removeFromFavorites(const QString& bookId);
    void addBookToLayout(const QString& bookId, const QString& title, const QString& author, const QString& imageUrl);
    void clearBookDisplay();

    QNetworkAccessManager *networkManager;
    QList<QNetworkReply*> activeReplies;

    QLineEdit *searchLineEdit;

private slots:

    void showBookListView();
    void showNotesView();
    void exitApplication();
    void loadAllBooks();
    void filterBooks(const QString &searchText);
};

#endif // MAINWINDOW_H
