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

#include <QSortFilterProxyModel>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QWidgetAction>
#include <QSettings>
#include <QLabel>
#include <QMenu>

#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include "shortcutmodel.h"
#include "installwizard.h"
#include "prefixmodel.h"
#include "searchmodel.h"
#include "mainwindow.h"
#include "filesystem.h"
#include "executor.h"
#include "dialogs.h"
#include "wizard.h"

MainWindow *MainWindow::mInstance = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mInstance = this;
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->actionInstall->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));
    ui->actionRun->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->actionTerminate->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->actionBrowse->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    ui->actionTerminateAll->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/quit")));

    QSettings s("winewizard", "settings");
    s.beginGroup("MainWindow");
    resize(s.value("Size", size()).toSize());
    ui->splitter->restoreState(s.value("Splitter").toByteArray());
    s.endGroup();

    QSize center = (QApplication::desktop()->size() - size()) * 0.5;
    move(center.width(), center.height());

    ui->actionQuit->setVisible(!Wizard::autoquit());

    ShortcutModel *shortcutModel = new ShortcutModel(this);
    QSortFilterProxyModel *shortcutSortModel = new QSortFilterProxyModel(this);
    shortcutSortModel->setSourceModel(shortcutModel);
    connect(shortcutSortModel, &QSortFilterProxyModel::dataChanged, shortcutSortModel,
            [shortcutSortModel]{ shortcutSortModel->sort(0); });
    ui->shortcuts->setModel(shortcutSortModel);

    PrefixModel *prefixModel = new PrefixModel(this);
    QSortFilterProxyModel *prefixSortModel = new QSortFilterProxyModel(this);
    prefixSortModel->setSourceModel(prefixModel);
    prefixSortModel->sort(0);
    connect(prefixSortModel, &QSortFilterProxyModel::dataChanged, prefixSortModel,
            [prefixSortModel]{ prefixSortModel->sort(0); });
    ui->prefixes->setModel(prefixSortModel);
    connect(ui->prefixes->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &MainWindow::prefixChanged);

    connect(ui->shortcuts->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &MainWindow::shortcutChanged);

    if (prefixSortModel->rowCount() > 0)
        ui->prefixes->setCurrentIndex(prefixSortModel->index(0, 0));
    update();
}

MainWindow::~MainWindow()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("MainWindow");
    s.setValue("Size", size());
    s.setValue("Splitter", ui->splitter->saveState());
    s.endGroup();
    delete ui;
    mInstance = nullptr;
    Wizard::update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        close();
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);
    else
        QMainWindow::changeEvent(e);
}

MainWindow *MainWindow::instance()
{
    return mInstance;
}

void MainWindow::update()
{
    if (mInstance)
    {
        bool empty = Executor::instances().isEmpty();
        mInstance->ui->actionTerminateAll->setDisabled(empty);
        mInstance->ui->actionInstall->setDisabled(InstallWizard::instance());
        mInstance->ui->actionSettings->setDisabled(InstallWizard::instance() || !empty);
        QModelIndex index = mInstance->ui->prefixes->currentIndex();
        if (index.isValid())
        {
            QString prefix = index.data(PrefixModel::PrefixRole).toString();
            bool contains = Executor::contains(prefix);
            mInstance->ui->actionTerminate->setEnabled(contains);
        }
        else
            mInstance->ui->actionTerminate->setEnabled(false);
        if (InstallWizard::instance())
        {
            QVariant field = InstallWizard::instance()->field("app");
            if (field.isValid())
            {
                QModelIndex ai = field.toModelIndex();
                if (ai.isValid())
                {
                    QString busy = ai.data(SearchModel::PrefixRole).toString();
                    QAbstractItemModel *model = mInstance->ui->prefixes->model();
                    QModelIndexList l = model->match(model->index(0, 0), PrefixModel::PrefixRole,
                                                     busy, -1, Qt::MatchFixedString);
                    if (!l.isEmpty())
                        model->removeRow(l.first().row());
                }
            }
        }
    }
}

void MainWindow::prefixChanged(const QModelIndex &index)
{
    QAbstractItemModel *model = ui->shortcuts->model();
    if (index.isValid())
    {
        QString prefix = index.data(PrefixModel::PrefixRole).toString();
        ui->actionTerminate->setEnabled(Executor::contains(prefix));
        model->setData(QModelIndex(), prefix, ShortcutModel::ResetRole);
    }
    else
    {
        ui->actionTerminate->setEnabled(false);
        model->setData(QModelIndex(), QString(), ShortcutModel::ResetRole);
    }
    model->sort(0);
    if (model->rowCount() > 0)
        ui->shortcuts->setCurrentIndex(model->index(0, 0));
    ui->actionRun->setEnabled(index.isValid() && ui->shortcuts->currentIndex().isValid());
    ui->actionBrowse->setEnabled(index.isValid());
}

void MainWindow::shortcutChanged(const QModelIndex &index)
{
    ui->actionRun->setEnabled(index.isValid());
}

void MainWindow::on_actionInstall_triggered()
{
    QString exe = Dialogs::open(tr("Select Installer"),
                                tr("Executable files (*.exe *.msi)"), this);
    if (!exe.isEmpty())
    {
        close();
        (new InstallWizard(exe))->show();
    }
}

void MainWindow::on_prefixes_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->prefixes->currentIndex();
    if (!index.isValid())
        return;

    QString prefix = index.data(PrefixModel::PrefixRole).toString();
    bool running = Executor::contains(prefix);

    QMenu menu(ui->prefixes);

    QMenu *iMenu = menu.addMenu(QIcon(":/icons/set-icon"), tr("Set icon..."));

    QAction *act = iMenu->addAction(tr("from file"));
    connect(act, &QAction::triggered, this, &MainWindow::setIconFromFile);

    act = iMenu->addAction(tr("from shortcut"));
    QModelIndex sIndex = ui->shortcuts->currentIndex();
    if (sIndex.isValid())
    {
        QString shortcut = sIndex.data(ShortcutModel::ShortcutRole).toString();
        act->setEnabled(FS::icons(prefix).exists(shortcut));
        connect(act, &QAction::triggered, this, &MainWindow::setIconFromShortcut);
    }
    else
        act->setEnabled(false);

    menu.addSeparator();

    act = menu.addAction(QIcon(":/icons/winecfg"), tr("Winecfg"));
    connect(act, &QAction::triggered, this, &MainWindow::winecfg);
    act->setDisabled(running);

    act = menu.addAction(QIcon(":/icons/regedit"), tr("Regedit"));
    connect(act, &QAction::triggered, this, &MainWindow::regedit);
    act->setDisabled(running);

    menu.addSeparator();

    act = menu.addAction(QIcon(":/icons/rename"), tr("Rename"));
    connect(act, &QAction::triggered, this, &MainWindow::renameApp);
    act->setDisabled(running);

    menu.addSeparator();

    act = menu.addAction(style()->standardIcon(QStyle::SP_TrashIcon), tr("Delete"));
    act->setDisabled(running);
    connect(act, &QAction::triggered, this, &MainWindow::deleteApp);

    menu.move(ui->prefixes->mapToGlobal(pos));
    menu.exec();
}

void MainWindow::deleteApp()
{
    QModelIndex index = ui->prefixes->currentIndex();
    QString name = index.data().toString();
    if (Dialogs::confirm(tr("Are you sure you want to delete \"%1\"?").arg(name), this))
    {
        QString prefix = index.data(PrefixModel::PrefixRole).toString();
        FS::prefix(prefix).removeRecursively();
        ui->prefixes->model()->removeRow(index.row());
    }
}

void MainWindow::renameApp()
{
    ui->prefixes->edit(ui->prefixes->currentIndex());
}

void MainWindow::winecfg()
{
    QString prefix = ui->prefixes->currentIndex().data(PrefixModel::PrefixRole).toString();
    QString path = FS::toUnixPath(prefix, "C:\\windows\\system32\\winecfg.exe");
    QString script = QString(FS::readFile(":/run")).arg(path).arg(QString()).arg(QString());
    Executor *e = new Executor(prefix);
    e->start(script, false, QProcess::ForwardedChannels);
    close();
}

void MainWindow::regedit()
{
    QString prefix = ui->prefixes->currentIndex().data(PrefixModel::PrefixRole).toString();
    QString path = FS::toUnixPath(prefix, "C:\\windows\\regedit.exe");
    QString script = QString(FS::readFile(":/run")).arg(path).arg(QString()).arg(QString());
    Executor *e = new Executor(prefix);
    e->start(script, false, QProcess::ForwardedChannels);
    close();
}

void MainWindow::on_shortcuts_customContextMenuRequested(const QPoint &pos)
{
    if (!ui->prefixes->currentIndex().isValid())
        return;
    QMenu menu(ui->shortcuts);
    QAction *act = menu.addAction(QIcon(":/icons/plus"), tr("Add"));
    connect(act, &QAction::triggered, this, &MainWindow::addShortcut);
    menu.addSeparator();
    QModelIndex index = ui->shortcuts->currentIndex();
    act = menu.addAction(QIcon(":/icons/set-icon"), tr("Set icon"));
    act->setEnabled(index.isValid());
    connect(act, &QAction::triggered, this, &MainWindow::editShortcutIcon);
    act = menu.addAction(style()->standardIcon(QStyle::SP_DirIcon), tr("Set working Directory"));
    act->setEnabled(index.isValid());
    connect(act, &QAction::triggered, this, &MainWindow::editWorkDir);
    act = menu.addAction(QIcon(":/icons/terminal"), tr("Set command line arguments"));
    act->setEnabled(index.isValid());
    connect(act, &QAction::triggered, this, &MainWindow::editArgs);
    menu.addSeparator();
    act = menu.addAction(QIcon(":/icons/rename"), tr("Rename"));
    connect(act, &QAction::triggered, this, &MainWindow::renameShortcut);
    act->setEnabled(index.isValid());
    menu.addSeparator();
    act = menu.addAction(style()->standardIcon(QStyle::SP_TrashIcon), tr("Delete"));
    connect(act, &QAction::triggered, this, &MainWindow::deleteShortcut);
    act->setEnabled(index.isValid());
    menu.move(ui->shortcuts->mapToGlobal(pos));
    menu.exec();
}

void MainWindow::renameShortcut()
{
    ui->shortcuts->edit(ui->shortcuts->currentIndex());
}

void MainWindow::deleteShortcut()
{
    QModelIndex index = ui->shortcuts->currentIndex();
    QString name = index.data().toString();
    if (Dialogs::confirm(tr("Are you sure you want to delete \"%1\"?").arg(name), this))
        ui->shortcuts->model()->removeRow(index.row());
}

void MainWindow::addShortcut()
{
    QString prefix = ui->prefixes->currentIndex().data(PrefixModel::PrefixRole).toString();
    QString dir = FS::drive(prefix).absolutePath();
    QString exe = Dialogs::open(tr("Select Executable File"),
                                tr("Executable files (*.exe *.msi)"), this, dir);
    if (!exe.isEmpty())
    {
        QSortFilterProxyModel *sm = static_cast<QSortFilterProxyModel *>(ui->shortcuts->model());
        QFileInfo info(exe);
        QString name = info.completeBaseName();
        while (!sm->match(sm->index(0, 0), Qt::DisplayRole, name, -1, Qt::MatchFixedString).isEmpty())
            name.append('0');
        QAbstractItemModel *m = sm->sourceModel();
        m->insertRow(0);
        QModelIndex index = m->index(0, 0);
        m->setData(index, name);
        m->setData(index, FS::toWinPath(prefix, exe), ShortcutModel::ExeRole);
        m->setData(index, FS::toWinPath(prefix, info.absolutePath()), ShortcutModel::WorkDirRole);
        ui->shortcuts->setCurrentIndex(sm->mapFromSource(index));
        sm->sort(0);
    }
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog sd(this);
    if (sd.exec() == SettingsDialog::Accepted)
        ui->actionQuit->setVisible(!Wizard::autoquit());
}

void MainWindow::setIconFromFile()
{
    QString path = Dialogs::open(tr("Select Icon"), tr("Icon files (*.png)"), this);
    if (!path.isEmpty())
        ui->prefixes->model()->setData(ui->prefixes->currentIndex(), path, Qt::DecorationRole);
}

void MainWindow::editShortcutIcon()
{
    QString path = Dialogs::open(tr("Select Icon"), tr("Icon files (*.png)"), this);
    if (!path.isEmpty())
        ui->shortcuts->model()->setData(ui->shortcuts->currentIndex(), path, Qt::DecorationRole);
}

void MainWindow::setIconFromShortcut()
{
    QModelIndex sIndex = ui->shortcuts->currentIndex();
    QModelIndex pIndex = ui->prefixes->currentIndex();
    QString sName = sIndex.data().toString();
    QString pName = pIndex.data().toString();
    if (Dialogs::confirm(tr("Are you sure you want to set icon for \"%1\" from \"%2\"?").
                         arg(pName).arg(sName), this))
    {
        QString shortcut = sIndex.data(ShortcutModel::ShortcutRole).toString();
        QString prefix = pIndex.data(PrefixModel::PrefixRole).toString();
        QString path = FS::icons(prefix).absoluteFilePath(shortcut);
        ui->prefixes->model()->setData(ui->prefixes->currentIndex(), path, Qt::DecorationRole);
    }
}

void MainWindow::runShortcut()
{
    QModelIndex index = ui->shortcuts->currentIndex();
    if (index.isValid())
    {
        QString exe = index.data(ShortcutModel::ExeRole).toString();
        QString workDir = index.data(ShortcutModel::WorkDirRole).toString();
        QString args = index.data(ShortcutModel::ArgsRole).toString();
        QString prefix = ui->prefixes->currentIndex().data(PrefixModel::PrefixRole).toString();
        QString script = QString(FS::readFile(":/run")).arg(exe).arg(workDir).arg(args);
        Executor *e = new Executor(prefix);
        e->start(script, true, QProcess::ForwardedChannels);
        close();
    }
}

void MainWindow::editArgs()
{
    QModelIndex index = ui->shortcuts->currentIndex();
    QString args = index.data(ShortcutModel::ArgsRole).toString();
    if (Dialogs::getText(tr("Command Line Arguments"), tr("Enter command line arguments:"), args, this))
        ui->shortcuts->model()->setData(index, args, ShortcutModel::ArgsRole);
}

void MainWindow::editWorkDir()
{
    QModelIndex index = ui->shortcuts->currentIndex();
    QString workDir = index.data(ShortcutModel::WorkDirRole).toString();
    if (workDir.isEmpty())
    {
        QString prefix = ui->prefixes->currentIndex().data(PrefixModel::PrefixRole).toString();
        workDir = FS::prefix(prefix).absolutePath();
    }
    QString newDir = Dialogs::dir(workDir, this);
    if (!newDir.isEmpty())
        ui->shortcuts->model()->setData(index, newDir, ShortcutModel::WorkDirRole);
}

void MainWindow::on_actionBrowse_triggered()
{
    QString prefix = ui->prefixes->currentIndex().data(PrefixModel::PrefixRole).toString();
    FS::browse(FS::data().absoluteFilePath(prefix));
}

void MainWindow::on_actionTerminate_triggered()
{
    QModelIndex index = ui->prefixes->currentIndex();
    QString name = index.data().toString();
    if (Dialogs::confirm(tr("Are you sure you want to terminate \"%1\"?").arg(name), this))
    {
        QString prefix = index.data(PrefixModel::PrefixRole).toString();
        for (Executor *e : Executor::instances())
            if (e->prefix() == prefix)
                e->deleteLater();
    }
}

void MainWindow::on_actionTerminateAll_triggered()
{
    if (Dialogs::confirm(tr("Are you sure you want to terminate all applications?"), this))
        for (Executor *e : Executor::instances())
            e->deleteLater();
}

void MainWindow::on_actionQuit_triggered()
{
    if (Dialogs::confirm(tr("Are you sure you want to quit from Wine Wizard?"), this))
    {
        for (Executor *e : Executor::instances())
            e->deleteLater();
        QApplication::quit();
    }
}
