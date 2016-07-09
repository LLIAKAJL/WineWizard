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
#include <QJsonDocument>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>

#include "ui_solutiondialog.h"
#include "categorysortmodel.h"
#include "packagesortmodel.h"
#include "solutiondialog.h"
#include "categorymodel.h"
#include "solutionmodel.h"
#include "winesortmodel.h"
#include "packagemodel.h"
#include "searchmodel.h"
#include "filesystem.h"
#include "winemodel.h"
#include "dialogs.h"

const int MAX_LENGTH = 5000;

SolutionDialog::SolutionDialog(QWizard *wizard, QWidget *parent, bool debug) :
    QDialog(parent),
    ui(new Ui::SolutionDialog),
    mWizard(wizard),
    mDebug(debug)
{
    ui->setupUi(this);

    QModelIndex index = mWizard->field("app").toModelIndex();
    Repository *r = mWizard->field("repository").value<Repository *>();
    ui->cancelBtn->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    ui->cancelBtn->hide();
    ui->saveBtn->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->saveBtn->hide();

    ui->bPackages->setModel(new PackageModel(this));
    ui->aPackages->setModel(new PackageModel(this));
    PackageSortModel *allSortModel = new PackageSortModel(this);
    allSortModel->setSourceModel(new PackageModel(this));
    allSortModel->setDynamicSortFilter(true);
    ui->allPackages->setModel(allSortModel);

    WineModel *wineModel = new WineModel(r->wines(), this);
    WineSortModel *wineSortModel = new WineSortModel(this);
    wineSortModel->setSourceModel(wineModel);
    wineSortModel->sort(0);
    ui->bWine->setModel(wineSortModel);
    ui->aWine->setModel(wineSortModel);

    SolutionModel *model = index.data(SearchModel::ModelRole).value<SolutionModel *>();
    ui->solutions->setModel(model);
    ui->solutions->setCurrentIndex(index.data(SearchModel::SolutionRole).toInt());

    CategoryModel *catModel = new CategoryModel(r->pCategories(), this);
    CategorySortModel *catSortModel = new CategorySortModel(this);
    catSortModel->setSourceModel(catModel);
    catSortModel->sort(0);
    ui->category->setModel(catSortModel);
    setEditable(false);
    if (mDebug)
    {
        setWindowTitle(tr("%1 - Debug").arg(index.data().toString()));
        QString out = mWizard->field("out").toString();
        QString err = mWizard->field("err").toString();
        ui->out->appendPlainText(out);
        ui->err->appendPlainText(err);
        QSet<QString> reqList;
        for (const Repository::Error &error : r->errors())
        {
            QRegExp re(error.re);
            re.setMinimal(true);
            if (re.indexIn(err) >= 0)
            {
                reqList.insert(r->package(error.package).name);
                continue;
            }
        }
        if (!reqList.isEmpty())
        {
            QStringList req = reqList.toList();
            ui->advice->appendPlainText(tr("You can try to install next packages: %1.").
                                        arg(req.join(", ")));
        }
        if (ui->advice->toPlainText().isEmpty())
            ui->adviceWidget->hide();
        ui->pages->setCurrentIndex(ui->pages->indexOf(ui->logPage));
    }
    else
    {
        setWindowTitle(tr("%1 - Solutions").arg(index.data().toString()));
        ui->pages->findChild<QTabBar *>()->hide();
        ui->pages->removeTab(ui->pages->indexOf(ui->logPage));
    }
    QSettings s("winewizard", "settings");
    s.beginGroup("SolutionDialog");
    resize(s.value("Size", size()).toSize());
    if (mDebug)
    {
        if (!ui->advice->toPlainText().isEmpty())
            ui->vSplitter->restoreState(s.value("VSplitter").toByteArray());
        ui->hSplitter->restoreState(s.value("HSplitter").toByteArray());
    }
    ui->shSplitter->restoreState(s.value("SHSplitter").toByteArray());
    ui->svSplitter->restoreState(s.value("SVSplitter").toByteArray());
    s.endGroup();
}

SolutionDialog::~SolutionDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("SolutionDialog");
    s.setValue("Size", size());
    if (mDebug)
    {
        if (!ui->advice->toPlainText().isEmpty())
            s.setValue("VSplitter", ui->vSplitter->saveState());
        s.setValue("HSplitter", ui->hSplitter->saveState());
    }
    s.setValue("SHSplitter", ui->shSplitter->saveState());
    s.setValue("SVSplitter", ui->svSplitter->saveState());
    s.endGroup();
    delete ui;
}

void SolutionDialog::accept()
{
    QModelIndex index = mWizard->field("app").toModelIndex();
    QAbstractItemModel *model = const_cast<QAbstractItemModel *>(index.model());
    model->setData(index, ui->solutions->currentIndex(), SearchModel::SolutionRole);
    QDialog::accept();
}

void SolutionDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help/#edit"));
}

void SolutionDialog::on_lockBtn_toggled(bool checked)
{
    ui->lockBtn->setIcon(checked ? QIcon(":/icons/lock") : QIcon(":/icons/lock-open"));
    if (checked)
        ui->aWine->setCurrentIndex(ui->bWine->currentIndex());
}

void SolutionDialog::setCategory(const QModelIndex &index)
{
    QString catId = index.data(CategoryModel::IdRole).toString();
    ui->allPackages->model()->setData(QModelIndex(), catId, PackageSortModel::CatIdRole);
}

void SolutionDialog::on_category_currentIndexChanged(int index)
{
    QString id = ui->category->itemData(index, CategoryModel::IdRole).toString();
    ui->allPackages->model()->setData(QModelIndex(), id, PackageSortModel::CatIdRole);
}

void SolutionDialog::on_solutions_currentIndexChanged(int index)
{
    Repository *r = mWizard->field("repository").value<Repository *>();
    int bwId = ui->solutions->itemData(index, SolutionModel::BWRole).toInt();
    int awId = ui->solutions->itemData(index, SolutionModel::AWRole).toInt();
    ui->lockBtn->setChecked(bwId == awId);
    ui->bWine->setCurrentText(r->wine(bwId).version);
    ui->aWine->setCurrentText(r->wine(awId).version);

    QList<int> bpIds = ui->solutions->itemData(index, SolutionModel::BPRole).value<IntList>();
    Repository::PackageList bp = r->packagesFromArr(bpIds);
    ui->bPackages->model()->setData(QModelIndex(), QVariant::fromValue(bp), PackageModel::ResetRole);

    QList<int> apIds = ui->solutions->itemData(index, SolutionModel::APRole).value<IntList>();
    Repository::PackageList ap = r->packagesFromArr(apIds);
    ui->aPackages->model()->setData(QModelIndex(), QVariant::fromValue(ap), PackageModel::ResetRole);

    QString bs = ui->solutions->itemData(index, SolutionModel::BSRole).toString();
    ui->bScript->setPlainText(bs);
    QString as = ui->solutions->itemData(index, SolutionModel::ASRole).toString();
    ui->aScript->setPlainText(as);

    Repository::PackageList all;
    for (const Repository::Package &p : r->packages())
        if (!bp.contains(p) && !ap.contains(p))
            all.append(p);
    PackageSortModel *allModel = static_cast<PackageSortModel *>(ui->allPackages->model());
    allModel->sourceModel()->setData(QModelIndex(), QVariant::fromValue(all), PackageModel::ResetRole);
    allModel->sort(0);
}

void SolutionDialog::setEditable(bool editable)
{
    ui->solutions->setEnabled(!editable);
    ui->bWine->setVisible(editable);
    ui->aWine->setVisible(editable);
    ui->bWineLbl->setVisible(!editable);
    ui->aWineLbl->setVisible(!editable);
    ui->allPackages->setVisible(editable);
    ui->categoryFrame->setVisible(editable);
    ui->bPackages->setDragEnabled(editable);
    ui->aPackages->setDragEnabled(editable);
    if (!editable)
        on_solutions_currentIndexChanged(ui->solutions->currentIndex());
    ui->cancelBtn->setVisible(editable);
    ui->saveBtn->setVisible(editable);
    ui->editBtn->setVisible(!editable);
    ui->lockFrame->setVisible(editable);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!editable);
//    ui->lockBtn->setChecked(true);
    ui->bScript->setReadOnly(!editable);
    ui->aScript->setReadOnly(!editable);
}

void SolutionDialog::on_editBtn_clicked()
{
    setEditable(true);
}

void SolutionDialog::on_cancelBtn_clicked()
{
    setEditable(false);
}

void SolutionDialog::on_saveBtn_clicked()
{
    ui->bWineLbl->setText(ui->bWine->currentText());
    ui->aWineLbl->setText(ui->aWine->currentText());
    int index = ui->solutions->currentIndex();
    int bWine = ui->bWine->currentData(WineModel::IdRole).toInt();
    ui->solutions->setItemData(index, bWine, SolutionModel::BWRole);
    int aWine = ui->aWine->currentData(WineModel::IdRole).toInt();
    ui->solutions->setItemData(index, aWine, SolutionModel::AWRole);
    IntList bp;
    QAbstractItemModel *bpModel = ui->bPackages->model();
    for (int i = 0, count = bpModel->rowCount(); i < count; ++i)
        bp.append(bpModel->index(i, 0).data(PackageModel::IdRole).toInt());
    ui->solutions->setItemData(index, QVariant::fromValue(bp), SolutionModel::BPRole);
    IntList ap;
    QAbstractItemModel *apModel = ui->aPackages->model();
    for (int i = 0, count = apModel->rowCount(); i < count; ++i)
        ap.append(apModel->index(i, 0).data(PackageModel::IdRole).toInt());
    ui->solutions->setItemData(index, QVariant::fromValue(ap), SolutionModel::APRole);
    QString bs = ui->bScript->toPlainText().trimmed();
    ui->solutions->setItemData(index, bs, SolutionModel::BSRole);
    QString as = ui->aScript->toPlainText().trimmed();
    ui->solutions->setItemData(index, as, SolutionModel::ASRole);
    setEditable(false);
}

void SolutionDialog::on_aWine_currentIndexChanged(int index)
{
    ui->aWineLbl->setText(ui->aWine->currentText());
    if (ui->lockBtn->isChecked())
        ui->bWine->setCurrentIndex(index);
}

void SolutionDialog::on_bWine_currentIndexChanged(int index)
{
    ui->bWineLbl->setText(ui->bWine->currentText());
    if (ui->lockBtn->isChecked())
        ui->aWine->setCurrentIndex(index);
}
