// #include "mainwindow.h"
#include "bookdetailswindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();

    QString bookId = "XdMBTKWSfeMC"; // Replace this with a valid book ID

    BookDetailsWindow *detailsWindow = new BookDetailsWindow(bookId, nullptr);
    detailsWindow->exec();

    return a.exec();
}
