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

#include <QMessageBox>
#include <QFileDialog>

#include "editshortcutdialog.h"
#include "setupwizard.h"
#include "ui_debugpage.h"
#include "intropage.h"
#include "debugpage.h"
#include "appmodel.h"
#include "utils.h"

DebugPage::DebugPage(QAbstractItemModel *model, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::DebugPage),
    mModel(static_cast<QSortFilterProxyModel *>(model))
{
    ui->setupUi(this);
    ui->editBtn->setIcon(QIcon::fromTheme("document-properties"));
    ui->newBtn->setIcon(QIcon::fromTheme("list-add"));
    ui->delBtn->setIcon(QIcon::fromTheme("list-remove"));
    ui->shortcuts->setModel(mModel);
    registerField("shortcut", this, "shortcut");
    registerField("debug", this, "debug");
    QItemSelectionModel *ssm = ui->shortcuts->selectionModel();
    connect(ssm, &QItemSelectionModel::currentRowChanged, this, &DebugPage::currentChanged);
}

DebugPage::~DebugPage()
{
    delete ui;
}

void DebugPage::initializePage()
{
    QVariant path = field("app").toModelIndex().data(AppModel::PathRole);
    QModelIndexList l = mModel->match(mModel->index(0, 0), MainModel::PathRole, path, -1, Qt::MatchFixedString);
    if (!l.isEmpty())
    {
        ui->shortcuts->setRootIndex(l.first());
        ui->shortcuts->setCurrentIndex(mModel->index(0, 0, l.first()));
        if (ui->shortcuts->currentIndex().isValid())
            ui->debug->setChecked(true);
        else
            ui->reinstall->setChecked(true);
    }
}

bool DebugPage::validatePage()
{
    if (ui->end->isChecked())
        return true;
    else if (ui->debug->isChecked())
    {
        wizard()->back();
        wizard()->back();
        wizard()->next();
    }
    else
    {
        wizard()->back();
        wizard()->back();
    }
    return false;
}

int DebugPage::nextId() const
{
    return SetupWizard::PageFinal;
}

void DebugPage::edit()
{
    EditShortcutDialog(ui->shortcuts->model(), ui->shortcuts->currentIndex(), wizard()).exec();
}

void DebugPage::currentChanged(const QModelIndex &index)
{
    bool valid = index.isValid();
    ui->debug->setEnabled(valid);
    ui->editBtn->setEnabled(valid);
    ui->delBtn->setEnabled(valid);
    if (!valid && ui->debug->isChecked())
        ui->reinstall->setChecked(true);
}

QModelIndex DebugPage::shortcut() const
{
    return ui->shortcuts->currentIndex();
}

bool DebugPage::debug() const
{
    return ui->debug->isChecked();
}

void DebugPage::on_newBtn_clicked()
{
    QString dir = ui->shortcuts->rootIndex().data(MainModel::PathRole).toString();
    QFileInfo info = Utils::selectExe(dir, this);
    if (info.exists())
    {
        QSortFilterProxyModel *sm = static_cast<QSortFilterProxyModel *>(ui->shortcuts->model());
        MainModel *m = static_cast<MainModel *>(sm->sourceModel());
        QModelIndex i = m->newShortcut(info.absoluteFilePath(), sm->mapToSource(ui->shortcuts->rootIndex()));
        ui->shortcuts->setCurrentIndex(sm->mapFromSource(i));
        edit();
    }
}

void DebugPage::on_delBtn_clicked()
{
    QModelIndex i = ui->shortcuts->currentIndex();
    QString name = i.data().toString();
    if (Utils::warning(tr("Are you sure you want to delete \"%1\"?").arg(name), wizard()))
        mModel->removeRow(i.row(), ui->shortcuts->rootIndex());
}

void DebugPage::on_shortcuts_doubleClicked(const QModelIndex &index)
{
    if (index.isValid())
        edit();
}
