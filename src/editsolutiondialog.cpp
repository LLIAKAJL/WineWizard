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
#include <QSettings>
#include <QDateTime>

#include "ui_editsolutiondialog.h"
#include "editsolutiondialog.h"
#include "packagesortmodel.h"
#include "categorymodel.h"
#include "winesortmodel.h"
#include "solutionmodel.h"
#include "packagemodel.h"
#include "winedialog.h"

EditSolutionDialog::EditSolutionDialog(const QModelIndex &solution, const QString &out,
                                                            const QString &err, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSolutionDialog),
    mRow(solution.row()),
    mModel(const_cast<QAbstractItemModel *>(solution.model()))
{
    ui->setupUi(this);
    QDesktopWidget *dw = QApplication::desktop();
    QSettings s("winewizard", "settings");
    s.beginGroup("EditSolutionDialog");
    resize(s.value("Size", QSize(dw->width() * 0.7, dw->height() * 0.6)).toSize());
    ui->vSplitter->restoreState(s.value("VSplitter").toByteArray());
    ui->hSplitter->restoreState(s.value("HSplitter").toByteArray());
    s.endGroup();
    setWindowTitle(solution.sibling(mRow, 1).data().toString());
    int bw = solution.data(SolutionModel::BWRole).toInt();
    int aw = solution.data(SolutionModel::AWRole).toInt();
    ui->lockBtn->setChecked(aw == bw);
    SolutionModel::PackageList wines = solution.data(SolutionModel::WinesRole).value<SolutionModel::PackageList>();
    SolutionModel::PackageList packages = solution.data(SolutionModel::PackagesRole).value<SolutionModel::PackageList>();
    mWineModel = new PackageModel(wines.keys(), wines, this);
    setWine(ui->bw, wines.value(bw).name);
    setWine(ui->aw, wines.value(aw).name);
    SolutionModel::IntList bp = solution.data(SolutionModel::BPRole).value<SolutionModel::IntList>();
    SolutionModel::IntList ap = solution.data(SolutionModel::APRole).value<SolutionModel::IntList>();
    QSet<int> all = QSet<int>::fromList(packages.keys());
    all.subtract(QSet<int>::fromList(bp));
    all.subtract(QSet<int>::fromList(ap));
    ui->bp->setModel(new PackageModel(bp, packages, this));
    ui->ap->setModel(new PackageModel(ap, packages, this));
    PackageModel *pm = new PackageModel(all.toList(), packages, this);
    QSortFilterProxyModel *psm = new PackageSortModel(pm, this);
    connect(ui->search, &QLineEdit::textChanged, psm, &QSortFilterProxyModel::setFilterFixedString);
    ui->all->setModel(psm);
    SolutionModel::CategoryList c = solution.data(SolutionModel::CatRole).value<SolutionModel::CategoryList>();
    ui->categories->setModel(new CategoryModel(c, this));
    ui->output->setPlainText(out);
    ui->errors->setPlainText(err);
    SolutionModel::ErrorList errors = solution.data(SolutionModel::ErrorsRole).value<SolutionModel::ErrorList>();
    for (const QString &e : errors.keys())
    {
        if (err.contains(e))
        {
            QString package = packages.value(errors.value(e)).name;
            ui->advices->appendPlainText(tr("You can try to install \"%1\".").arg(package));
        }
    }
}

EditSolutionDialog::~EditSolutionDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("EditSolutionDialog");
    s.setValue("Size", size());
    s.setValue("VSplitter", ui->vSplitter->saveState());
    s.setValue("HSplitter", ui->hSplitter->saveState());
    s.endGroup();
    delete ui;
}

void EditSolutionDialog::accept()
{
    QVariant bw = ui->bw->property("wine");
    QModelIndexList l = mWineModel->match(mWineModel->index(0, 0), Qt::DisplayRole, bw, -1, Qt::MatchFixedString);
    bw = l.first().data(PackageModel::IdRole);
    QVariant aw = ui->aw->property("wine");
    l = mWineModel->match(mWineModel->index(0, 0), Qt::DisplayRole, aw, -1, Qt::MatchFixedString);
    aw = l.first().data(PackageModel::IdRole);
    SolutionModel::IntList bp;
    QAbstractItemModel *bpm = ui->bp->model();
    for (int i = 0, count = bpm->rowCount(); i < count; ++i)
        bp.append(bpm->index(i, 0).data(PackageModel::IdRole).toInt());
    SolutionModel::IntList ap;
    QAbstractItemModel *apm = ui->ap->model();
    for (int i = 0, count = apm->rowCount(); i < count; ++i)
        ap.append(apm->index(i, 0).data(PackageModel::IdRole).toInt());
    QModelIndex index = mModel->index(mRow, 0);
    QMap<int, QVariant> data;
    data.insert(SolutionModel::BWRole, bw);
    data.insert(SolutionModel::AWRole, aw);
    data.insert(SolutionModel::BPRole, QVariant::fromValue(bp));
    data.insert(SolutionModel::APRole, QVariant::fromValue(ap));
    QSortFilterProxyModel *sm = static_cast<QSortFilterProxyModel *>(mModel);
    sm->sourceModel()->setItemData(sm->mapToSource(index), data);
    QDialog::accept();
}

void EditSolutionDialog::on_lockBtn_toggled(bool checked)
{
    ui->lockBtn->setIcon(QIcon(checked ? ":/images/locked" : ":/images/unlocked"));
}

void EditSolutionDialog::on_bw_clicked()
{
    QString wine = ui->bw->property("wine").toString();
    WineDialog wd(wine, mWineModel, this);
    if (wd.exec() == WineDialog::Accepted)
    {
        setWine(ui->bw, wd.wine());
        if (ui->lockBtn->isChecked())
            setWine(ui->aw, ui->bw->property("wine"));
    }
}

void EditSolutionDialog::on_aw_clicked()
{
    QString wine = ui->aw->property("wine").toString();
    WineDialog wd(wine, mWineModel, this);
    if (wd.exec() == WineDialog::Accepted)
    {
        setWine(ui->aw, wd.wine());
        if (ui->lockBtn->isChecked())
            setWine(ui->bw, ui->aw->property("wine"));
    }
}

void EditSolutionDialog::setWine(QPushButton *button, const QVariant &wine)
{
    button->setProperty("wine", wine);
    button->setText(wine.toString());
}

void EditSolutionDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}

void EditSolutionDialog::on_categories_currentIndexChanged(int)
{
    QAbstractItemModel *m = ui->all->model();
    QVariant cd = ui->categories->currentData(CategoryModel::IdRole);
    m->setData(QModelIndex(), cd, PackageSortModel::CatRole);
}
