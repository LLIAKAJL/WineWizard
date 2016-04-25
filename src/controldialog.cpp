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
#include <QSettings>
#include <QUrl>

#include "createshortcutdialog.h"
#include "ui_controldialog.h"
#include "shortcutsmodel.h"
#include "settingsdialog.h"
#include "solutionmodel.h"
#include "controldialog.h"
#include "filesystem.h"
#include "executor.h"
#include "dialogs.h"

ControlDialog::ControlDialog(const QStringList &busiList, const QStringList &runList, QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::ControlDialog),
    mBusiList(busiList),
    mRunList(runList)
{
    ui->setupUi(this);
    auto model = new ShortcutsModel(this);
    auto sortModel = new QSortFilterProxyModel(this);
    sortModel->setSourceModel(model);
    sortModel->sort(0);
    sortModel->setDynamicSortFilter(true);
    ui->shortcuts->setModel(sortModel);
    connect(ui->shortcuts->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ControlDialog::shortcutChanged);

    auto solModel = new SolutionModel(this);
    auto solSortModel = new QSortFilterProxyModel(this);
    solSortModel->setSourceModel(solModel);
    solSortModel->sort(0);
    solSortModel->setDynamicSortFilter(true);
    ui->solutions->setModel(solSortModel);

    ui->addBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    ui->shortcutSettingsBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    ui->solutionSettingsBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    ui->deleteSolutionBtn->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    ui->deleteShortcutBtn->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    ui->browseBtn->setIcon(style()->standardIcon(QStyle::SP_DriveHDIcon));
}

ControlDialog::~ControlDialog()
{
    delete ui;
}

void ControlDialog::shortcutChanged(const QModelIndex &index)
{
    auto ok = index.isValid();
    ui->deleteShortcutBtn->setEnabled(ok);
    ui->shortcutSettingsBtn->setEnabled(ok);
}

void ControlDialog::on_deleteShortcutBtn_clicked()
{
    auto index = ui->shortcuts->currentIndex();
    auto name = index.data().toString();
    if (Dialogs::confirm(tr(R"(Are you sure you want to delete "%1"?)").arg(name), this))
        ui->shortcuts->model()->removeRow(index.row());
}

void ControlDialog::on_addBtn_clicked()
{
    auto solution = ui->solutions->currentData().toString();
    auto driveC = FS::driveTarget(solution).absolutePath();
    QString exe = Dialogs::open(tr("Select Application"), tr("MS Windows applications (*.exe *.msi)"), this, driveC);
    if (!exe.isEmpty())
    {
        CreateShortcutDialog csd(ui->shortcuts->model(), exe, this);
        if (csd.exec() == QDialog::Accepted)
        {
            exe = FS::pathWithDrive(solution, exe);
            auto path = FS::pathWithDrive(solution, csd.workDir());
            Ex::wait(FS::readFile(":/shortcut").arg(FS::hash(csd.name()), exe, path, csd.args()), solution, this);
            auto sortModel = static_cast<QSortFilterProxyModel *>(ui->shortcuts->model());
            auto model = sortModel->sourceModel();
            model->insertRow(0);
            model->setData(model->index(0, 0), csd.name());
            ui->shortcuts->setCurrentIndex(sortModel->mapFromSource(model->index(0, 0)));
        }
    }
}

void ControlDialog::on_deleteSolutionBtn_clicked()
{
    auto name = ui->solutions->currentText();
    auto hash = ui->solutions->currentData().toString();
    if (Dialogs::confirm(tr(R"(Are you sure you want to delete "%1"?)").arg(name), this))
    {
        ui->solutions->removeItem(ui->solutions->currentIndex());
        FS::removeSolution(hash);
    }
}

void ControlDialog::on_browseBtn_clicked()
{
    FS::browse(FS::solution(ui->solutions->currentData().toString()).absolutePath());
}

void ControlDialog::on_shortcutSettingsBtn_clicked()
{
    auto index = ui->shortcuts->currentIndex();
    SettingsDialog(index, this).exec();
}

void ControlDialog::on_solutionSettingsBtn_clicked()
{
    auto index = ui->solutions->model()->index(ui->solutions->currentIndex(), 0);
    auto name = index.data().toString();
    if (SettingsDialog(index, this).exec() == QDialog::Accepted && ui->solutions->currentText() != name)
        on_solutions_currentIndexChanged(ui->solutions->currentIndex());
}

void ControlDialog::on_solutions_currentIndexChanged(int index)
{
    auto model = ui->shortcuts->model();
    model->setData(QModelIndex(), ui->solutions->currentData().toString(), Qt::UserRole);
    if (model->rowCount() > 0)
        ui->shortcuts->setCurrentIndex(model->index(0, 0));
    auto empty = index < 0;
    ui->solutionsGB->setDisabled(empty);
    ui->shortcutsGB->setDisabled(empty);
    if (!empty)
    {
        auto current = ui->solutions->currentData().toString();
        auto rb = mBusiList.contains(current) || mRunList.contains(current);
        ui->deleteSolutionBtn->setDisabled(rb);
        ui->solutionSettingsBtn->setDisabled(rb);
    }
}

void ControlDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
