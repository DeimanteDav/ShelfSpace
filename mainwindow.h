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

    //void addPage(QWidget* page, const QString& name);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;

    void setupDatabase();

    /*
    QSqlTableModel *model;
    QNetworkAccessManager *manager;

    void setupModel();
    void fetchAndInsertBooks();
    void parseAndInsertBooks(const QJsonArray &items);

    QStackedWidget *stackedWidget;

    QWidget *mainPage;
    QWidget *favoritesPage;
    QWidget *notesPage;
    QWidget *bookPage;

    void setupMainPage();
    void setupPages();
*/

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

    QScrollArea *scrollArea; //should they be in .h?
    QWidget *scrollWidget;
    QVBoxLayout *scrollLayout;

    void loadAllBooks();

//protected:
  //  bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    /*
    void onFetchFinished(QNetworkReply *reply);
    void showMainPage();
    void showFavoritesPage();
    void showNotesPage();
    void showBookPage();
    */

    void showViewBooksView();
    void showNotesView();
    void exitApplication();

    void handleBookClicked(QListWidgetItem *item);
};

#endif // MAINWINDOW_H
