/***************************************************************************
 *   Copyright (C) 2016 by Vitalii Kachemtsev <LLIAKAJI@wwizard.net>       *
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
#include <QJsonDocument>
#include <QWidgetAction>
#include <QLibraryInfo>
#include <QJsonArray>
#include <QSettings>
#include <QWindow>
#include <QUrl>

#include "editshortcutdialog.h"
#include "editprefixdialog.h"
#include "terminatedialog.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include "setupwizard.h"
#include "aboutdialog.h"
#include "mainwindow.h"
#include "netmanager.h"
#include "mainmodel.h"
#include "adslabel.h"
#include "ticker.h"
#include "utils.h"

#include <QDebug>

const QString UPDATE_URL = "http://wwizard.net/api3/?c=update&l=%1";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ads->hide();
    ui->splitter->setStretchFactor(1, 1);
    QSystemTrayIcon *tray = new QSystemTrayIcon(qApp->windowIcon(), this);
    connect(tray, &QSystemTrayIcon::activated, this, &MainWindow::trayActivated);
    tray->show();
    qApp->installTranslator(&mStdTrans);
    qApp->installTranslator(&mTrans);
    QSettings s("winewizard", "settings");
    QLocale::Language lang = static_cast<QLocale::Language>(s.value("Language", QLocale::English).toInt());
    setLanguage(lang);
    getUpdate();
    s.beginGroup("MainWindow");
    QDesktopWidget *dw = QApplication::desktop();
    resize(s.value("Size", QSize(dw->width() * 0.7, dw->height() * 0.6)).toSize());
    ui->splitter->restoreState(s.value("Splitter").toByteArray());
    s.endGroup();
    setActionIcon(ui->actionInstall,   "drive-optical",        QStyle::SP_DriveCDIcon);
    setActionIcon(ui->actionExecute,   "media-playback-start", QStyle::SP_MediaPlay);
    setActionIcon(ui->actionTerminate, "media-playback-stop",  QStyle::SP_MediaStop);
    setActionIcon(ui->actionBrowse,    "folder-open",          QStyle::SP_DirOpenIcon);
    setActionIcon(ui->actionControl,   "computer",             QStyle::SP_ComputerIcon);
    setActionIcon(ui->actionQuit,      "application-exit",     QStyle::SP_DialogCloseButton);
    setActionIcon(ui->actionHelp,      "help-contents",        QStyle::SP_DialogHelpButton);
    ui->actionSettings->setIcon(QIcon::fromTheme("preferences-system", QIcon(":/images/settings")));
    setBtnIcon(ui->editPrefixBtn,     "document-properties", QStyle::SP_FileDialogDetailedView);
    setBtnIcon(ui->editShortcutBtn,   "document-properties", QStyle::SP_FileDialogDetailedView);
    setBtnIcon(ui->newShortcutBtn,    "list-add",            QStyle::SP_FileDialogNewFolder);
    setBtnIcon(ui->deletePrefixBtn,   "list-remove",         QStyle::SP_TrashIcon);
    setBtnIcon(ui->deleteShortcutBtn, "list-remove",         QStyle::SP_TrashIcon);
    ui->prefixesEditPanel->hide();
    ui->shortcutsEditPanel->hide();
    QSortFilterProxyModel *m = new QSortFilterProxyModel(this);
    m->setSourceModel(new MainModel(this));
    m->sort(0);
    m->setDynamicSortFilter(true);
    ui->prefixes->setModel(m);
    ui->shortcuts->setModel(m);
    QItemSelectionModel *psm = ui->prefixes->selectionModel();
    connect(psm, &QItemSelectionModel::currentRowChanged, this, &MainWindow::checkState);
    connect(m,   &QSortFilterProxyModel::dataChanged,     this, &MainWindow::checkState);
    connect(m,   &QSortFilterProxyModel::rowsRemoved,     this, &MainWindow::checkState);
    connect(m,   &QSortFilterProxyModel::rowsInserted,    this, &MainWindow::rowsInserted);
    QItemSelectionModel *ssm = ui->shortcuts->selectionModel();
    connect(ssm, &QItemSelectionModel::currentRowChanged, this, &MainWindow::checkState);
    ui->prefixes->setCurrentIndex(m->index(0, 0));
}

MainWindow::~MainWindow()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("MainWindow");
    s.setValue("Size", size());
    s.setValue("Splitter", ui->splitter->saveState());
    s.endGroup();
    delete ui;
}

void MainWindow::start(const QString &cmdLine)
{
    ui->actionControl->setChecked(false);
    if (cmdLine.isEmpty())
        showNormal();
    else if (!QFile::exists(cmdLine))
    {
        Utils::error(tr("The file \"%1\" was not found.").arg(cmdLine), this);
        showNormal();
    }
    else if (QFileInfo(cmdLine).suffix().toUpper() == "LNK")
    {
        QAbstractItemModel *m = ui->prefixes->model();
        bool exists = false;
        for (int i = m->rowCount(); i >= 0; --i)
        {
            QModelIndex pi = m->index(i, 0);
            for (int j = m->rowCount(pi); j >= 0; --j)
            {
                QModelIndex si = m->index(j, 0, pi);
                if (QFileInfo(si.data(MainModel::PathRole).toString()) == cmdLine)
                {
                    exists = true;
                    hide();
                    m->setData(si, true, MainModel::ExecuteRole);
                }
            }
        }
        if (!exists)
        {
            Utils::error(tr("The shortcut \"%1\" was not found.").
                         arg(QFileInfo(cmdLine).completeBaseName()), this);
            showNormal();
        }
    }
    else
    {
        hide();
        SetupWizard *iw = new SetupWizard(cmdLine, ui->prefixes->model());
        connect(iw, &SetupWizard::destroyed, this, &MainWindow::checkState);
        iw->show();
    }
    checkState();
}

void MainWindow::setVisible(bool visible)
{
    if (visible)
        move(QApplication::desktop()->geometry().center() - QPoint(width(), height()) * 0.5);
    else
        getUpdate();
    QMainWindow::setVisible(visible);
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);
    else
        QMainWindow::changeEvent(e);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (!ui->actionControl->isChecked() && e->key() == Qt::Key_Escape)
        hide();
    QMainWindow::keyPressEvent(e);
}

void MainWindow::getUpdate()
{
    QString out = Utils::temp().absoluteFilePath(Utils::genID());
    NetManager *nm = new NetManager(this);
    nm->setProperty("out", out);
    connect(nm, &NetManager::finished, this, &MainWindow::updateFinished);
    connect(nm, &NetManager::error,    this, &MainWindow::error);
    QSettings s("winewizard", "settings");
    QLocale::Language lang = static_cast<QLocale::Language>(s.value("Language", QLocale::English).toInt());
    nm->start(NetManager::RequestList() << NetManager::makeGetRequest(UPDATE_URL.arg(QLocale(lang).name()), out));
}

void MainWindow::updateFinished()
{
    QString out = sender()->property("out").toString();
    QJsonObject data = Utils::readJson(out);
    if (QFile::exists(out))
        QFile::remove(out);
    QString v = data.value("v").toString();
    if (v != VERSION)
    {
        if (ui->ads->layout()->isEmpty())
        {
            Ticker *t = new Ticker(tr("A new version of Wine Wizard(%1) is available!").arg(v),
                                   "http://wwizard.net/download", ui->ads);
            ui->ads->layout()->addWidget(t);
            ui->ads->show();
        }
    }
    else
    {
        QJsonArray b = data.value("b").toArray();
        for (QJsonArray::ConstIterator i = b.begin(); i != b.end(); ++i)
        {
            QString out = Utils::temp().absoluteFilePath(Utils::genID());
            NetManager *nm = new NetManager(this);
            QJsonObject o = (*i).toObject();
            nm->setProperty("out", out);
            nm->setProperty("url", o.value("u").toString());
            nm->setProperty("h", data.value("h").toInt());
            nm->setProperty("m", data.value("m").toInt());
            connect(nm, &NetManager::finished, this, &MainWindow::finished);
            connect(nm, &NetManager::error,    this, &MainWindow::error);
            nm->start(NetManager::RequestList() << NetManager::makeGetRequest(o.value("i").toString(), out));
        }
    }
    sender()->deleteLater();
}

void MainWindow::finished()
{
    QString out = sender()->property("out").toString();
    if (QFile::exists(out))
    {
        QString url = sender()->property("url").toString();
        int h = sender()->property("h").toInt();
        int m = sender()->property("m").toInt();
        QHBoxLayout *l = static_cast<QHBoxLayout *>(ui->ads->layout());
        for (int i = l->count() - 1; i >= 0; --i)
            if (static_cast<AdsLabel *>(l->itemAt(i)->widget())->url() == url)
                l->takeAt(i)->widget()->deleteLater();
        if (l->count() > m)
            l->takeAt(0)->widget()->deleteLater();
        l->addWidget(new AdsLabel(out, url, h, ui->ads), 1);
        ui->ads->show();
    }
    sender()->deleteLater();
}

void MainWindow::error()
{
    QString out = sender()->property("out").toString();
    if (QFile::exists(out))
        QFile::remove(out);
    sender()->deleteLater();
}

void MainWindow::checkState()
{
    QAbstractItemModel *m = ui->prefixes->model();
    if (!ui->prefixes->currentIndex().isValid())
        ui->prefixes->setCurrentIndex(m->index(0, 0));
    if (ui->shortcuts->rootIndex() != ui->prefixes->currentIndex())
    {
        ui->shortcuts->setRootIndex(ui->prefixes->currentIndex());
        ui->shortcuts->setCurrentIndex(m->index(0, 0, ui->prefixes->currentIndex()));
    }
    QModelIndexList rl = m->match(m->index(0, 0), MainModel::RunningRole, true);
    bool running = !rl.isEmpty();
    if (running && ui->actionControl->isChecked())
        ui->actionControl->setChecked(false);
    ui->actionControl->setDisabled(running || SetupWizard::running());
    bool controlChecked = ui->actionControl->isChecked();
    ui->actionSettings->setDisabled(running || controlChecked || SetupWizard::running());
    bool valid = ui->shortcuts->rootIndex().isValid();
    ui->actionBrowse->setEnabled(!controlChecked && valid);
    ui->deletePrefixBtn->setEnabled(valid);
    ui->editPrefixBtn->setEnabled(valid);
    ui->newShortcutBtn->setEnabled(valid);
    ui->actionTerminate->setEnabled(!controlChecked && running);
    bool currentShortcut = ui->shortcuts->model()->rowCount(ui->shortcuts->rootIndex()) > 0;
    if (!ui->shortcuts->currentIndex().isValid())
        ui->shortcuts->setCurrentIndex(m->index(0, 0, ui->prefixes->currentIndex()));
    ui->actionExecute->setEnabled(!controlChecked && currentShortcut);
    ui->deleteShortcutBtn->setEnabled(currentShortcut);
    ui->editShortcutBtn->setEnabled(currentShortcut);
    if (isHidden() && !SetupWizard::running() && !running)
        showNormal();
}

void MainWindow::trayActivated()
{
    if (!isVisible() || isMinimized())
        showNormal();
    else
        hide();
}

void MainWindow::on_actionQuit_triggered()
{
    QAbstractItemModel *m = ui->prefixes->model();
    if (m->match(m->index(0, 0), MainModel::RunningRole, true).isEmpty())
        QApplication::quit();
    else if (Utils::warning(tr("Are you sure you want to quit from Wine Wizard?"), this))
        QApplication::quit();
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog sd(this);
    if (sd.exec() == SettingsDialog::Accepted)
        setLanguage(sd.language());
}

void MainWindow::setLanguage(QLocale::Language language)
{
    if (language == QLocale::English)
    {
        mStdTrans.load(QString());
        mTrans.load(QString());
    }
    else
    {
        QString stdPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        mStdTrans.load(QString("qt_%1").arg(QLocale(language).name()), stdPath);
        mTrans.load(QString(":/translations/%1").arg(language));
    }
}

void MainWindow::setActionIcon(QAction *action, const QString &name, QStyle::StandardPixmap alter)
{
    action->setIcon(QIcon::fromTheme(name, style()->standardIcon(alter)).pixmap(32, 32).
                            scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::setBtnIcon(QPushButton *button, const QString &iconName, QStyle::StandardPixmap alter)
{
    button->setIcon(QIcon::fromTheme(iconName, style()->standardIcon(alter)));
}

void MainWindow::on_deleteShortcutBtn_clicked()
{
    QModelIndex shortcut = ui->shortcuts->currentIndex();
    QString name = shortcut.data(Qt::EditRole).toString();
    if (Utils::warning(tr("Are you sure you want to delete \"%1\"?").arg(name), this))
        ui->shortcuts->model()->removeRow(shortcut.row(), ui->shortcuts->rootIndex());
}

void MainWindow::on_deletePrefixBtn_clicked()
{
    QModelIndex prefix = ui->prefixes->currentIndex();
    QString name = prefix.data(Qt::EditRole).toString();
    if (Utils::warning(tr("Are you sure you want to delete \"%1\"?").arg(name), this))
        ui->prefixes->model()->removeRow(prefix.row());
}

void MainWindow::on_editShortcutBtn_clicked()
{
    EditShortcutDialog(ui->shortcuts->model(), ui->shortcuts->currentIndex(), this).exec();
}

void MainWindow::on_editPrefixBtn_clicked()
{
    EditPrefixDialog(ui->prefixes->model(), ui->prefixes->currentIndex(), this).exec();
}

void MainWindow::on_actionBrowse_triggered()
{
    QString dir = ui->prefixes->currentIndex().data(MainModel::PathRole).toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

void MainWindow::on_actionExecute_triggered()
{
    QModelIndex shortcut = ui->shortcuts->currentIndex();
    ui->shortcuts->model()->setData(shortcut, true, MainModel::ExecuteRole);
    hide();
}

void MainWindow::on_actionTerminate_triggered()
{
    TerminateDialog(ui->prefixes->model(), this).exec();
}

void MainWindow::on_actionControl_toggled(bool checked)
{
    if (checked)
    {
        ui->actionControl->setShortcut(Qt::Key_Escape);
        ui->shortcuts->setDragDropMode(QListView::DragOnly);
    }
    else
    {
        ui->actionControl->setShortcut(Qt::Key_unknown);
        ui->shortcuts->setDragDropMode(QListView::NoDragDrop);
    }
}

void MainWindow::on_newShortcutBtn_clicked()
{
    QString dir = ui->shortcuts->rootIndex().data(MainModel::PathRole).toString();
    QFileInfo info = Utils::selectExe(dir, this);
    if (info.exists())
    {
        QSortFilterProxyModel *sm = static_cast<QSortFilterProxyModel *>(ui->shortcuts->model());
        MainModel *m = static_cast<MainModel *>(sm->sourceModel());
        QModelIndex i = m->newShortcut(info.absoluteFilePath(), sm->mapToSource(ui->shortcuts->rootIndex()));
        ui->shortcuts->setCurrentIndex(sm->mapFromSource(i));
        on_editShortcutBtn_clicked();
    }
}

void MainWindow::on_shortcuts_doubleClicked(const QModelIndex &index)
{
    if (index.isValid())
    {
        if (ui->actionControl->isChecked())
            on_editShortcutBtn_clicked();
        else
            on_actionExecute_triggered();
    }
}

void MainWindow::on_prefixes_doubleClicked(const QModelIndex &index)
{
    if (index.isValid() && ui->actionControl->isChecked())
        on_editPrefixBtn_clicked();
}

void MainWindow::rowsInserted(const QModelIndex &parent, int row)
{
    if (parent.isValid())
    {
        if (!ui->shortcuts->currentIndex().isValid() && ui->shortcuts->rootIndex() == parent)
            ui->shortcuts->setCurrentIndex(ui->shortcuts->model()->index(0, 0, parent));
    }
    else
        ui->prefixes->setCurrentIndex(ui->prefixes->model()->index(row, 0));
}

void MainWindow::on_actionInstall_triggered()
{
    QString path = Utils::selectExe(QDir::homePath(), this);
    if (!path.isEmpty())
        start(path);
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog(this).exec();
}

void MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
