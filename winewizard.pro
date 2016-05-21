#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T02:31:29
#
#-------------------------------------------------

QT       += core gui network

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 1.0

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

TARGET = winewizard
isEmpty(PREFIX) {
 PREFIX = /usr/bin
}
target.path = $$PREFIX/

TEMPLATE = app

icon.path = /usr/share/icons/hicolor/48x48/apps/
icon.files = winewizard.png
desktop.path = /usr/share/applications/
desktop.files = winewizard.desktop
INSTALLS += target \
            icon \
            desktop

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
    src/outputdialog.cpp \
    src/solutiondialog.cpp \
    src/waitdialog.cpp \
    src/wizard.cpp \
    src/singletonwidget.cpp \
    src/singletondialog.cpp \
    src/terminaldialog.cpp \
    src/searchmodel.cpp \
    src/packagemodel.cpp \
    src/packagesortmodel.cpp \
    src/selectdialog.cpp \
    src/selectmodel.cpp \
    src/downloaddialog.cpp \
    src/netdialog.cpp \
    src/selectarchdialog.cpp \
    src/postdialog.cpp \
    src/mainmenu.cpp \
    src/editshortcutdialog.cpp \
    src/editprefixdialog.cpp \
    src/editsolutiondialog.cpp \
    src/settingsdialog.cpp

HEADERS  += src/qtsingleapplication/qtlocalpeer.h \
    src/qtsingleapplication/qtlockedfile.h \
    src/qtsingleapplication/qtsingleapplication.h \
    src/qtsingleapplication/qtsinglecoreapplication.h \
    src/aboutdialog.h \
    src/dialogs.h \
    src/executor.h \
    src/filesystem.h \
    src/outputdialog.h \
    src/solutiondialog.h \
    src/waitdialog.h \
    src/wizard.h \
    src/singletonwidget.h \
    src/singletondialog.h \
    src/terminaldialog.h \
    src/searchmodel.h \
    src/packagemodel.h \
    src/packagesortmodel.h \
    src/selectdialog.h \
    src/selectmodel.h \
    src/downloaddialog.h \
    src/netdialog.h \
    src/selectarchdialog.h \
    src/postdialog.h \
    src/mainmenu.h \
    src/editshortcutdialog.h \
    src/editprefixdialog.h \
    src/editsolutiondialog.h \
    src/settingsdialog.h

FORMS    += src/solutiondialog.ui \
    src/aboutdialog.ui \
    src/waitdialog.ui \
    src/outputdialog.ui \
    src/terminaldialog.ui \
    src/selectdialog.ui \
    src/downloaddialog.ui \
    src/selectarchdialog.ui \
    src/postdialog.ui \
    src/editshortcutdialog.ui \
    src/editprefixdialog.ui \
    src/editsolutiondialog.ui \
    src/settingsdialog.ui

RESOURCES += \
    src/resources.qrc

OTHER_FILES +=
