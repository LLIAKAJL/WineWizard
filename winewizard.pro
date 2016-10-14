#-------------------------------------------------
#
# Project created by QtCreator 2016-09-07T03:44:05
#
#-------------------------------------------------

QT       += core gui network

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

isEmpty(OS) {
 OS = linux
}

DEFINES += VERSION=\\\"3.0.1\\\" \
           OS=\\\"$$OS\\\"

TARGET = winewizard

isEmpty(PREFIX) {
    PREFIX = /usr/local
}
isEmpty(BINDIR) {
    BINDIR = $$PREFIX/bin
}
isEmpty(DATADIR) {
    DATADIR = $$PREFIX/share
}

target.path = $$BINDIR

TARGET = winewizard
TEMPLATE = app

icon.files = winewizard.png
icon.path = $$DATADIR/icons/hicolor/128x128/apps/

desktop.files = winewizard.desktop
desktop.path = $$DATADIR/applications/

INSTALLS += target \
            icon \
            desktop

SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/mainmodel.cpp \
    src/settingsdialog.cpp \
    src/intropage.cpp \
    src/solutionpage.cpp \
    src/installpage.cpp \
    src/debugpage.cpp \
    src/finalpage.cpp \
    src/editprefixdialog.cpp \
    src/editshortcutdialog.cpp \
    src/qtsingleapplication/qtlocalpeer.cpp \
    src/qtsingleapplication/qtlockedfile.cpp \
    src/qtsingleapplication/qtlockedfile_unix.cpp \
    src/qtsingleapplication/qtlockedfile_win.cpp \
    src/qtsingleapplication/qtsingleapplication.cpp \
    src/qtsingleapplication/qtsinglecoreapplication.cpp \
    src/newappdialog.cpp \
    src/packageview.cpp \
    src/editsolutiondialog.cpp \
    src/utils.cpp \
    src/winedialog.cpp \
    src/winesortmodel.cpp \
    src/appmodel.cpp \
    src/netmanager.cpp \
    src/waitform.cpp \
    src/solutionmodel.cpp \
    src/packagemodel.cpp \
    src/terminatedialog.cpp \
    src/terminatemodel.cpp \
    src/updatepage.cpp \
    src/delegates.cpp \
    src/categorymodel.cpp \
    src/packagesortmodel.cpp \
    src/centercombobox.cpp \
    src/setupwizard.cpp \
    src/adslabel.cpp \
    src/aboutdialog.cpp \
    src/highlightedlistview.cpp \
    src/highlightedtableview.cpp \
    src/ticker.cpp

HEADERS  += src/mainwindow.h \
    src/mainmodel.h \
    src/settingsdialog.h \
    src/intropage.h \
    src/solutionpage.h \
    src/installpage.h \
    src/debugpage.h \
    src/finalpage.h \
    src/editprefixdialog.h \
    src/editshortcutdialog.h \
    src/qtsingleapplication/qtlocalpeer.h \
    src/qtsingleapplication/QtLockedFile \
    src/qtsingleapplication/qtlockedfile.h \
    src/qtsingleapplication/QtSingleApplication \
    src/qtsingleapplication/qtsingleapplication.h \
    src/qtsingleapplication/qtsinglecoreapplication.h \
    src/wintypes.h \
    src/newappdialog.h \
    src/packageview.h \
    src/editsolutiondialog.h \
    src/utils.h \
    src/winedialog.h \
    src/winesortmodel.h \
    src/appmodel.h \
    src/netmanager.h \
    src/waitform.h \
    src/solutionmodel.h \
    src/packagemodel.h \
    src/terminatedialog.h \
    src/terminatemodel.h \
    src/updatepage.h \
    src/delegates.h \
    src/categorymodel.h \
    src/packagesortmodel.h \
    src/centercombobox.h \
    src/setupwizard.h \
    src/adslabel.h \
    src/aboutdialog.h \
    src/highlightedlistview.h \
    src/highlightedtableview.h \
    src/ticker.h

FORMS    += src/mainwindow.ui \
    src/settingsdialog.ui \
    src/intropage.ui \
    src/solutionpage.ui \
    src/installpage.ui \
    src/debugpage.ui \
    src/finalpage.ui \
    src/editshortcutdialog.ui \
    src/editprefixdialog.ui \
    src/newappdialog.ui \
    src/editsolutiondialog.ui \
    src/winedialog.ui \
    src/waitform.ui \
    src/terminatedialog.ui \
    src/setupwizard.ui \
    src/aboutdialog.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    translations/ru.ts
