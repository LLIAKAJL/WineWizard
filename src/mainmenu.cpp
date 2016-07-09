/***************************************************************************
 *   Copyright (C) 2016 by Vitalii Kachemtsev <LLIAKAJI@wwizard.net>         *
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

#include <QSortFilterProxyModel>
#include <QDesktopServices>
#include <QWidgetAction>
#include <QApplication>
#include <QSettings>
#include <QStyle>
#include <QUrl>

#include "installwizard.h"
#include "shortcutmodel.h"
#include "aboutdialog.h"
#include "prefixmodel.h"
#include "mainwindow.h"
#include "filesystem.h"
#include "executor.h"
#include "mainmenu.h"
#include "dialogs.h"
#include "wizard.h"

MainMenu *MainMenu::mInstance = nullptr;

MainMenu::MainMenu(QWidget *parent) :
    QMenu(parent)
{
    mInstance = this;
    setAttribute(Qt::WA_DeleteOnClose);
    QAction *act = addAction(style()->standardIcon(QStyle::SP_ComputerIcon), tr("&Control Center"));
    connect(act, &QAction::triggered, this, &MainMenu::controlCenter);
    addSeparator();
    PrefixModel model;
    if (model.rowCount() == 0)
        addEmpty(this);
    else
    {
        QSortFilterProxyModel sortModel;
        sortModel.setSourceModel(&model);
        sortModel.sort(0);

        for (int i = 0, count = sortModel.rowCount(); i < count; ++i)
        {
            QModelIndex pi = sortModel.index(i, 0);
            QString prefix = pi.data(PrefixModel::PrefixRole).toString();
            QString name = pi.data().toString();
            QIcon icon = pi.data(Qt::DecorationRole).value<QIcon>();
            QMenu *pMenu = addMenu(icon, QString(name).replace('&', "&&"));
            ShortcutModel sm;
            sm.setData(QModelIndex(), prefix, ShortcutModel::ResetRole);
            QSortFilterProxyModel ssm;
            ssm.setSourceModel(&sm);
            ssm.sort(0);
            if (ssm.rowCount() == 0)
                addEmpty(pMenu);
            else
            {
                for (int i = 0, count = ssm.rowCount(); i < count; ++i)
                {
                    QModelIndex si = ssm.index(i, 0);
                    QString name = si.data().toString().replace('&', "&&");
                    QIcon icon = si.data(Qt::DecorationRole).value<QIcon>();
                    QAction *act = pMenu->addAction(icon, name);
                    QString exe = si.data(ShortcutModel::ExeRole).toString();
                    QString workDir = si.data(ShortcutModel::WorkDirRole).toString();
                    QString args = si.data(ShortcutModel::ArgsRole).toString();
                    act->setProperty("prefix", prefix);
                    act->setProperty("exe", exe);
                    act->setProperty("dir", workDir);
                    act->setProperty("args", args);
                    connect(act, &QAction::triggered, this, &MainMenu::run);
                }
                pMenu->addSeparator();
                QAction *act = pMenu->addAction(style()->standardIcon(QStyle::SP_MediaStop),
                                                tr("&Terminate"));
                act->setShortcut(QKeySequence(QString("Ctrl+T")));
                act->setProperty("name", name);
                act->setProperty("prefix", prefix);
                connect(act, &QAction::triggered, this, &MainMenu::terminate);
                act->setEnabled(Executor::contains(prefix));
            }
        }
    }
    addSeparator();
    act = addAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Terminate &All"));
    act->setShortcut(QKeySequence(QString("Ctrl+Shift+T")));
    connect(act, &QAction::triggered, this, &MainMenu::terminateAll);
    act->setDisabled(Executor::instances().isEmpty());
    addSeparator();
    act = addAction(style()->standardIcon(QStyle::SP_DialogHelpButton), tr("&Help"));
    act->setShortcut(QKeySequence::HelpContents);
    connect(act, &QAction::triggered, this, &MainMenu::help);
    act = addAction(style()->standardIcon(QStyle::SP_MessageBoxInformation), tr("About"));
    connect(act, &QAction::triggered, this, &MainMenu::about);
    if (!Wizard::autoquit())
    {
        addSeparator();
        act = addAction(QIcon(":/icons/quit"), tr("&Quit"));
        act->setShortcut(QKeySequence::Quit);
        connect(act, &QAction::triggered, this, &MainMenu::quit);
    }
}

MainMenu::~MainMenu()
{
    mInstance = nullptr;
    Wizard::update();
}

MainMenu *MainMenu::instance()
{
    return mInstance;
}

void MainMenu::controlCenter()
{
    (new MainWindow)->showNormal();
}

void MainMenu::terminateAll()
{
    if (Dialogs::confirm(tr("Are you sure you want to terminate all applications?")))
        for (Executor *e : Executor::instances())
            e->deleteLater();
}

void MainMenu::terminate()
{
    QString name = sender()->property("name").toString();
    if (Dialogs::confirm(tr("Are you sure you want to terminate \"%1\"?").arg(name)))
    {
        QString prefix = sender()->property("prefix").toString();
        for (Executor *e : Executor::instances())
            if (e->prefix() == prefix)
                e->deleteLater();
    }
}

void MainMenu::about()
{
    AboutDialog().exec();
}

void MainMenu::quit()
{
    if (Executor::instances().isEmpty())
        QApplication::quit();
    else if (Dialogs::confirm(tr("Are you sure you want to terminate all " \
                                 "applications and quit from Wine Wizard?")))
    {
        for (Executor *e : Executor::instances())
            e->deleteLater();
        QApplication::quit();
    }
}

void MainMenu::help()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help/"));
}

void MainMenu::addEmpty(QMenu *menu)
{
    QWidgetAction *act = new QWidgetAction(menu);
    act->setText(tr("*** Empty ***"));
    menu->addAction(act);
}

void MainMenu::run()
{
    QString prefix = sender()->property("prefix").toString();
    QString exe = sender()->property("exe").toString();
    QString workDir = sender()->property("dir").toString();
    QString args = sender()->property("args").toString();
    QString script = QString(FS::readFile(":/run")).arg(exe).arg(workDir).arg(args);
    Executor *e = new Executor(prefix);
    e->start(script, false, QProcess::ForwardedChannels);
}
