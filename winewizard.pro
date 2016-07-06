#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T02:31:29
#
#-------------------------------------------------

QT       += core gui network

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 2.0.1

isEmpty(OS) {
 OS = linux
}

DEFINES += APP_VERSION=\\\"$$VERSION\\\" \
           OS=\\\"$$OS\\\"

TARGET = winewizard
isEmpty(PREFIX) {
 PREFIX = /usr/bin
}
target.path = $$PREFIX/

TEMPLATE = app

icon.path = /usr/share/icons/hicolor/128x128/apps/
icon.files = winewizard.png
desktop.path = /usr/share/applications/
desktop.files = winewizard.desktop
INSTALLS += target \
            icon \
            desktop

TRANSLATIONS += ru.ts

SOURCES += src/qtsingleapplication/qtlocalpeer.cpp \
    src/qtsingleapplication/qtlockedfile.cpp \
    src/qtsingleapplication/qtlockedfile_unix.cpp \
    src/qtsingleapplication/qtlockedfile_win.cpp \
    src/qtsingleapplication/qtsingleapplication.cpp \
    src/qtsingleapplication/qtsinglecoreapplication.cpp \
    src/aboutdialog.cpp \
    src/dialogs.cpp \
    src/executor.cpp \
    src/filesystem.cpp \
    src/main.cpp \
    src/wizard.cpp \
    src/searchmodel.cpp \
    src/packagemodel.cpp \
    src/packagesortmodel.cpp \
    src/mainmenu.cpp \
    src/settingsdialog.cpp \
    src/installwizard.cpp \
    src/intropage.cpp \
    src/winpage.cpp \
    src/installpage.cpp \
    src/solutionmodel.cpp \
    src/mainwindow.cpp \
    src/shortcutmodel.cpp \
    src/repository.cpp \
    src/categorymodel.cpp \
    src/centercombobox.cpp \
    src/winemodel.cpp \
    src/winesortmodel.cpp \
    src/categorysortmodel.cpp \
    src/debugpage.cpp \
    src/solutionpage.cpp \
    src/solutiondialog.cpp \
    src/prefixmodel.cpp

HEADERS  += src/qtsingleapplication/qtlocalpeer.h \
    src/qtsingleapplication/qtlockedfile.h \
    src/qtsingleapplication/qtsingleapplication.h \
    src/qtsingleapplication/qtsinglecoreapplication.h \
    src/aboutdialog.h \
    src/dialogs.h \
    src/executor.h \
    src/filesystem.h \
    src/wizard.h \
    src/searchmodel.h \
    src/packagemodel.h \
    src/packagesortmodel.h \
    src/mainmenu.h \
    src/settingsdialog.h \
    src/installwizard.h \
    src/intropage.h \
    src/winpage.h \
    src/installpage.h \
    src/solutionmodel.h \
    src/mainwindow.h \
    src/shortcutmodel.h \
    src/repository.h \
    src/categorymodel.h \
    src/centercombobox.h \
    src/winemodel.h \
    src/winesortmodel.h \
    src/categorysortmodel.h \
    src/debugpage.h \
    src/solutionpage.h \
    src/solutiondialog.h \
    src/prefixmodel.h

FORMS    += \
    src/aboutdialog.ui \
    src/settingsdialog.ui \
    src/installwizard.ui \
    src/intropage.ui \
    src/winpage.ui \
    src/installpage.ui \
    src/mainwindow.ui \
    src/debugpage.ui \
    src/solutionpage.ui \
    src/solutiondialog.ui

RESOURCES += \
    src/resources.qrc

OTHER_FILES +=
