QT       += core gui widgets sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bookdetailswindow.cpp \
    databasemanager.cpp \
    main.cpp \
    mainwindow.cpp \
    bookListView.cpp \
    noteswidget.cpp \
    noteeditwidget.cpp \
    labeledbutton.cpp \
    dragscrollarea.cpp

HEADERS += \
    bookdetailswindow.h \
    databasemanager.h \
    mainwindow.h \
    bookListView.h \
    noteswidget.h \
    noteeditwidget.h \
    labeledbutton.h \
    dragscrollarea.h

FORMS += \
    bookdetailswindow.ui \
    mainwindow.ui \
    bookListView.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
