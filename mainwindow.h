#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    void setupMenu();
    void setupCentralViews();

    QWidget *mainPageWidget;
    QListWidget *recommendedBooksList;

    QWidget *addBookWidget;
    QWidget *viewBooksWidget;
    QWidget *notesWidget;

    QAction *actionAddBook;
    QAction *actionViewBooks;
    QAction *actionShowNotes;
    QAction *actionExit;

    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVBoxLayout *scrollLayout;

    void loadAllBooks();
    bool isBookFavorite(const QString& bookId);
    void addToFavorites(const QString& bookId);
    void removeFromFavorites(const QString& bookId);

    QNetworkAccessManager *networkManager;
    QList<QNetworkReply*> activeReplies;


private slots:

    void showViewBooksView();
    void showNotesView();
    void exitApplication();

    void handleBookClicked(QListWidgetItem *item);
};

#endif // MAINWINDOW_H
