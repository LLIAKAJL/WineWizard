#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T02:31:29
#
#-------------------------------------------------

QT       += core gui network

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 0.4

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
    src/mainmenu.cpp \
    src/outputdialog.cpp \
    src/shortcutsdialog.cpp \
    src/shortcutsmodel.cpp \
    src/singletonwidget.cpp \
    src/solutiondialog.cpp \
    src/waitdialog.cpp \
    src/wizard.cpp \
    src/downloaddialog.cpp \
    src/singletondialog.cpp \
    src/terminaldialog.cpp \
    src/createshortcutdialog.cpp \
    src/controldialog.cpp \
    src/settingsdialog.cpp \
    src/solutionmodel.cpp \
    src/scriptsdialog.cpp

HEADERS  += src/qtsingleapplication/qtlocalpeer.h \
    src/qtsingleapplication/qtlockedfile.h \
    src/qtsingleapplication/qtsingleapplication.h \
    src/qtsingleapplication/qtsinglecoreapplication.h \
    src/aboutdialog.h \
    src/dialogs.h \
    src/executor.h \
    src/filesystem.h \
    src/mainmenu.h \
    src/outputdialog.h \
    src/shortcutsdialog.h \
    src/shortcutsmodel.h \
    src/singletonwidget.h \
    src/solutiondialog.h \
    src/waitdialog.h \
    src/wizard.h \
    src/downloaddialog.h \
    src/singletondialog.h \
    src/terminaldialog.h \
    src/createshortcutdialog.h \
    src/controldialog.h \
    src/settingsdialog.h \
    src/solutionmodel.h \
    src/scriptsdialog.h

FORMS    += src/solutiondialog.ui \
    src/aboutdialog.ui \
    src/waitdialog.ui \
    src/outputdialog.ui \
    src/shortcutsdialog.ui \
    src/downloaddialog.ui \
    src/terminaldialog.ui \
    src/createshortcutdialog.ui \
    src/controldialog.ui \
    src/settingsdialog.ui \
    src/scriptsdialog.ui

RESOURCES += \
    src/resources.qrc

OTHER_FILES +=
