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

#include <QCryptographicHash>
#include <QDesktopServices>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QEventLoop>
#include <QSettings>
#include <QProcess>
#include <QThread>
#include <QUrl>

#include "shortcutsdialog.h"
#include "downloaddialog.h"
#include "solutiondialog.h"
#include "controldialog.h"
#include "outputdialog.h"
#include "aboutdialog.h"
#include "waitdialog.h"
#include "filesystem.h"
#include "executor.h"
#include "mainmenu.h"
#include "dialogs.h"
#include "wizard.h"

#define FINISHED static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished)

Wizard::Wizard(QObject *parent) :
    QObject(parent)
{
    QFile f(FS::config().absoluteFilePath("style.qss"));
    if (f.exists())
    {
        f.open(QFile::ReadOnly);
        qApp->setStyleSheet(f.readAll());
    }
    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        auto tray = new QSystemTrayIcon(QIcon::fromTheme("winewizard"), this);
        connect(tray, &QSystemTrayIcon::activated, this, [this](){ start(); });
        tray->show();
    }
}

void Wizard::start(const QString &exe)
{
    if (SingletonWidget::activate())
        return;
    else if (exe.isEmpty())
        showMenu();
    else if (mRunList.isEmpty() && mBusyList.isEmpty())
        install(exe);
    else
        Dialogs::error(tr("Another process is already running!"));
}

void Wizard::showMenu()
{
    MainMenu mm(mRunList);
    auto act = mm.exec();
    if (act)
    {
        switch (act->data().toInt())
        {
        case MainMenu::Shortcut:
            {
                auto solution = act->property("Solution").toString();
                auto exe = FS::devices(solution).absoluteFilePath(act->property("Exe").toString());
                mRunList.append(solution);
                Ex::execute(FS::readFile(":/release").arg(exe), solution);
                mRunList.removeOne(solution);
            }
            break;
        case MainMenu::Terminate:
            {
                auto solution = act->property("Solution").toString();
                QSettings s(FS::solution(solution).absoluteFilePath(".settings"), QSettings::IniFormat);
                s.setIniCodec("UTF-8");
                auto solutionName = s.value("Name").toString();
                if (Dialogs::confirm(tr(R"(Are you sure you want to terminate "%1"?)").arg(solutionName)))
                {
                    mRunList.append(solution);
                    Ex::execute(FS::readFile(":/terminate"), solution);
                    mRunList.removeOne(solution);
                }
            }
            break;
        case MainMenu::Control:
            ControlDialog(mBusyList, mRunList).exec();
            break;
        case MainMenu::Help:
            QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
            break;
        case MainMenu::About:
            AboutDialog().exec();
            break;
        }
    }
}

void Wizard::install(const QString &exe)
{
    if (DownloadDialog(QString()).exec() != QDialog::Accepted)
        return;
    auto exePath = exe;
    if (!QFile::exists(exePath))
    {
        Dialogs::error(tr(R"(File "%1" not found!)").arg(exePath));
        return;
    }
    if (!testSuffix(exePath))
    {
        Dialogs::error(tr(R"(File "%1" is not a valid Windows application!)").arg(exePath));
        return;
    }
    SolutionDialog sd;
    if (sd.exec() == QDialog::Accepted)
    {
        if (DownloadDialog(sd.url()).exec() != QDialog::Accepted)
            return;
        QSettings s(FS::temp().absoluteFilePath("solution"), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        auto solutionName = s.value("Name").toString();
        auto solution = FS::hash(solutionName);
        if (mBusyList.contains(solution) || mRunList.contains(solution))
        {
            Dialogs::error(tr("The application \"%1\" is already running!").arg(solutionName));
            return;
        }
        else
            mBusyList.append(solution);
        if (FS::solution(solution).exists())
        {
            if (Dialogs::confirm(tr(R"(Application "%1" is already installed! Are you sure you want to reinstall this application?)").arg(solutionName)))
                FS::removeSolution(solution);
            else
            {
                mBusyList.removeOne(solution);
                return;
            }
        }
        auto bw = s.value("BWine").toString();
        auto bp = s.value("BPackages").toStringList();
        auto bpr = required(bp);
        auto aw = s.value("AWine").toString();
        auto ap = s.value("APackages").toStringList();
        auto apr = required(ap);
        auto varSH = FS::readFile(FS::mainPart("variables-sh")).arg("win32");
        auto mainSH = FS::readFile(FS::mainPart("functions-sh"));
        mainSH += varSH;
        Ex::wait(mainSH + FS::readFile(":/prepare").arg(bw, bpr), solution);
        QSettings(FS::solution(solution).absoluteFilePath(".settings"), QSettings::IniFormat).setValue("Name", solutionName);
        s.setIniCodec("UTF-8");
        Ex::execute(FS::readFile(":/create"), solution);
        auto wmbPath = FS::sys32(solution).absoluteFilePath("winemenubuilder.exe");
        QFile::remove(wmbPath);
        QFile::copy(":/winemenubuilder.exe", wmbPath);
        mainSH += FS::readFile(FS::mainPart("packages-sh"));
        Ex::terminal(mainSH + FS::readFile(":/before").arg(bp.join(' ')), solution);
        auto debugSH = FS::readFile(":/debug");
        mRunList.append(solution);
        Ex::execute(debugSH.arg(exePath), solution);
        mRunList.removeOne(solution);
        Ex::wait(mainSH + FS::readFile(":/prepare-after").arg(aw, apr), solution);
        Ex::terminal(mainSH + FS::readFile(":/after").arg(ap.join(' ')), solution);
        auto clearSH = varSH + FS::readFile(":/clear");
        while (true)
        {
            auto res = Dialogs::finish(solution);
            if (res == Dialogs::Yes)
            {
                ShortcutsDialog sd(solution);
                if (sd.exec() == QDialog::Accepted)
                {
                    QSettings s(FS::shortcuts(solution).absoluteFilePath(sd.shortcut()), QSettings::IniFormat);
                    s.setIniCodec("UTF-8");
                    exePath = FS::devices(solution).absoluteFilePath(s.value("Exe").toString());
                }
                else
                {
                    killSolution(solution);
                    return;
                }
            }
            else if (res == Dialogs::No)
            {
                OutputDialog().exec();
                killSolution(solution);
                return;
            }
            else
            {
                Ex::execute(clearSH, solution);
                return;
            }
            mRunList.append(solution);
            Ex::execute(debugSH.arg(exePath), solution);
            mRunList.removeOne(solution);
        }
    }
}

QString Wizard::required(const QStringList &packages) const
{
    QStringList res;
    QSettings r(FS::mainPart("required"), QSettings::IniFormat);
    for (auto package : packages)
        res.append(r.value(package).toStringList());
    return QStringList(res.toSet().toList()).join(' ');
}

void Wizard::killSolution(const QString &solution)
{
    FS::removeSolution(solution);
    mBusyList.removeOne(solution);
}

bool Wizard::testSuffix(const QFileInfo &path) const
{
    auto suffix = path.suffix().toUpper();
    return suffix == "EXE" || suffix == "MSI";
}
