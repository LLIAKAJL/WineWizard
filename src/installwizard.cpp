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

#include <QDesktopServices>
#include <QPushButton>
#include <QSettings>
#include <QLayout>
#include <QUrl>

#include "ui_installwizard.h"
#include "installwizard.h"
#include "solutionpage.h"
#include "searchmodel.h"
#include "installpage.h"
#include "filesystem.h"
#include "mainwindow.h"
#include "debugpage.h"
#include "intropage.h"
#include "executor.h"
#include "dialogs.h"
#include "winpage.h"
#include "wizard.h"

InstallWizard *InstallWizard::mInstance = nullptr;

InstallWizard::InstallWizard(const QString &exe, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::InstallWizard)
{
    ui->setupUi(this);
    QSettings s("winewizard", "settings");
    s.beginGroup("InstallWizard");
    resize(s.value("Size", size()).toSize());
    s.endGroup();

    for (const QFileInfo &f : FS::temp().entryInfoList(QDir::Files))
        QFile::remove(f.absoluteFilePath());
    mInstance = this;
    setAttribute(Qt::WA_DeleteOnClose);
    setPixmap(QWizard::BackgroundPixmap, QPixmap(":/icons/background"));
    setPage(PageSolution, new SolutionPage(this));
    setPage(PageIntro, new IntroPage(exe, this));
    setPage(PageInstall, new InstallPage(this));
    setPage(PageDebug, new DebugPage(this));
    setPage(PageWin, new WinPage(this));
    for (int id : pageIds())
        connect(page(id), &QWizardPage::completeChanged, this, &InstallWizard::pageChanged);
    MainWindow::update();
}

InstallWizard::~InstallWizard()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("InstallWizard");
    s.setValue("Size", size());
    s.endGroup();
    for (const QFileInfo &f : FS::temp().entryInfoList(QDir::Files))
        QFile::remove(f.absoluteFilePath());
    delete ui;
    mInstance = nullptr;
    MainWindow::update();
    Wizard::instance()->start();
}

InstallWizard *InstallWizard::instance()
{
    return mInstance;
}

void InstallWizard::reject()
{
    if (property("finish").toBool() || Dialogs::confirm(tr("Installation is not yet completed. " \
                            "Are you sure you want to cancel?"), this))
    {
        if (currentId() != PageIntro && currentId() != PageSolution)
        {
            QModelIndex index = field("app").toModelIndex();
            QString prefix = index.data(SearchModel::PrefixRole).toString();
            if (!prefix.isEmpty())
            {
                for (Executor *e : Executor::instances())
                    if (e->prefix() == prefix)
                        delete e;
                FS::prefix(prefix).removeRecursively();
            }
        }
        QWizard::reject();
    }
}

void InstallWizard::on_InstallWizard_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help/#installation"));
}

void InstallWizard::pageChanged()
{
    button(BackButton)->setVisible(currentId() == PageSolution);
}
