/***************************************************************************
 *   Copyright (C) 2016 by Vitalii Kachemtsev <LLIAKAJL@yandex.ru>         *
 *                                                                         *
 *   This file is part of Wine Wizard.                                     *
 *                                                                         *
 *   Wine Wizard is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Wine Wizard is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with Wine Wizard.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#include <QLibraryInfo>
#include <QSettings>
#include <QUrl>

#include "qtsingleapplication/QtSingleApplication"
#include "settingsdialog.h"
#include "installwizard.h"
#include "mainwindow.h"
#include "filesystem.h"
#include "mainmenu.h"
#include "executor.h"
#include "dialogs.h"
#include "wizard.h"

Wizard *Wizard::mInstance = nullptr;
bool Wizard::mAutoquit = false;
QTranslator *Wizard::mStdTrans = nullptr;
QTranslator *Wizard::mTrans = nullptr;

Wizard::Wizard(QObject *parent) :
    QObject(parent)
{
    mInstance = this;
    QSettings s("winewizard", "settings");
    mAutoquit = s.value("Autoquit").toBool();
    retranslate(s.value("Language").toInt());

    QSystemTrayIcon *tray = new QSystemTrayIcon(QIcon::fromTheme("winewizard"), this);
    connect(tray, &QSystemTrayIcon::activated, this, &Wizard::trayActivated);
    tray->show();
}

void Wizard::update()
{
    if (mAutoquit &&
        Executor::instances().isEmpty() &&
        !MainWindow::instance() &&
        !InstallWizard::instance() &&
        !MainMenu::instance())
    {
        QApplication::quit();
    }
    mInstance = nullptr;
}

bool Wizard::autoquit()
{
    return mAutoquit;
}

void Wizard::setAutoquit(bool autoquit)
{
    mAutoquit = autoquit;
}

void Wizard::retranslate(int lang)
{
    if (mStdTrans)
    {
        qApp->removeTranslator(mStdTrans);
        delete mStdTrans;
        mStdTrans = nullptr;
    }
    if (mTrans)
    {
        qApp->removeTranslator(mTrans);
        delete mTrans;
        mTrans = nullptr;
    }
    if (lang != LANG_EN)
    {
        QString suffix = langSuffix(lang);
        QString stdTransFile = QString("qt_") + suffix;
        mStdTrans = new QTranslator(qApp);
        if (mStdTrans->load(stdTransFile, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
            qApp->installTranslator(mStdTrans);
        else
        {
            delete mStdTrans;
            mStdTrans = nullptr;
        }
        mTrans = new QTranslator;
        if (mTrans->load(QString(":/lang/") + suffix))
            qApp->installTranslator(mTrans);
        else
        {
            delete mTrans;
            mTrans = nullptr;
        }
    }
}

Wizard *Wizard::instance()
{
    return mInstance;
}

void Wizard::start(const QString &cmdLine)
{
    if (cmdLine.isEmpty())
    {
        if (MainWindow::instance())
            MainWindow::instance()->showNormal();
        else
            (new MainWindow)->showNormal();
    }
    else
    {
        QStringList cmdList = cmdLine.split('\n', QString::SkipEmptyParts);
        if (cmdList.count() > 1)
            executeShortcut(cmdList.first(), cmdList.at(1));
        else if (!QFile::exists(cmdList.first()))
            Dialogs::error(tr("File \"%1\" not found!").arg(cmdList.first()));
        else if (!testSuffix(cmdList.first()))
            Dialogs::error(tr("File \"%1\" is not a valid Windows application!").arg(cmdList.first()));
        else if (InstallWizard::instance())
            InstallWizard::instance()->show();
        else
            (new InstallWizard(cmdList.first()))->show();
    }
}

void Wizard::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Context)
    {
        if (MainWindow::instance())
            MainWindow::instance()->close();
        (new MainMenu)->popup(QCursor::pos());
    }
    else
    {
        MainWindow *mw = MainWindow::instance();
        if (mw)
        {
            if (mw->windowState() == Qt::WindowMinimized && mw->isVisible())
                mw->showNormal();
            else
                mw->close();
        }
        else
            (new MainWindow)->showNormal();
    }
}

bool Wizard::testSuffix(const QFileInfo &path) const
{
    QString suffix = path.suffix().toUpper();
    return suffix == "EXE" || suffix == "MSI";
}

void Wizard::executeShortcut(const QString &prefix, const QString &shortcut)
{
    mAutoquit = true;
    if (!FS::data().exists(prefix))
    {
        Dialogs::error(tr("Application not found!"));
        update();
    }
    else if (!FS::shortcuts(prefix).exists(shortcut))
    {
        Dialogs::error(tr("Shortcut not found!"));
        update();
    }
    else
    {
        QSettings s(FS::shortcuts(prefix).absoluteFilePath(shortcut), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        QString exe = FS::toUnixPath(prefix, s.value("Exe").toString());
        QString workDir = FS::toUnixPath(prefix, s.value("WorkDir").toString());
        QString args = s.value("Args").toString();
        QString script = QString(FS::readFile(":/run")).arg(exe).arg(workDir).arg(args);
        Executor *e = new Executor(prefix);
        e->start(script, true, QProcess::ForwardedChannels);
    }
}

QString Wizard::langSuffix(int lang)
{
    switch (lang)
    {
    case LANG_RU:
        return "ru";
    default:
        return "en";
    }
}
