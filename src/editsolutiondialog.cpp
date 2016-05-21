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

#include <QPushButton>
#include <QSettings>

#include "ui_editsolutiondialog.h"
#include "editsolutiondialog.h"
#include "packagesortmodel.h"
#include "packagemodel.h"
#include "selectdialog.h"
#include "filesystem.h"
#include "postdialog.h"
#include "dialogs.h"

bool wineLessThan(const QString &l, const QString &r)
{
    QStringList ll = l.split('.', QString::SkipEmptyParts);
    QStringList rl = r.split('.', QString::SkipEmptyParts);
    if (ll.first() == rl.first())
    {
        ll.removeFirst();
        rl.removeFirst();
        if (ll.first() == rl.first())
        {
            ll.removeFirst();
            rl.removeFirst();
            auto li = ll.first().indexOf('-');
            auto ri = rl.first().indexOf('-');
            auto lm = li > -1 ? ll.first().left(li) : ll.first();
            auto rm = ri > -1 ? rl.first().left(ri) : rl.first();
            if (lm == rm)
            {
                auto lm = li > -1 ? ll.first().remove(li + 1) : QString();
                auto rm = ri > -1 ? rl.first().remove(ri + 1) : QString();
                return lm > rm;
            }
            return lm.toInt() > rm.toInt();
        }
        return ll.first().toInt() > rl.first().toInt();
    }
    return ll.first().toInt() > rl.first().toInt();
}

EditSolutionDialog::EditSolutionDialog(const QString &arch, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSolutionDialog),
    mArch(arch)
{
    ui->setupUi(this);
    ui->bMoveWine->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->aMoveWine->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    QSettings settings("winewizard", "settings");
    settings.beginGroup("EditSolutionDialog");
    resize(settings.value("Size", size()).toSize());
    settings.endGroup();
    QSettings s(FS::temp().absoluteFilePath("solution"), QSettings::IniFormat);
    s.setIniCodec("UTF-8");
    mSolData.slug = s.value("Slug").toString();
    mSolData.bWine = s.value("BWine").toString();
    mSolData.aWine = s.value("AWine").toString();
    mSolData.bPackages = s.value("BPackages").toStringList();
    mSolData.aPackages = s.value("APackages").toStringList();
    setWindowTitle(s.value("Name").toString());
    QSettings r(FS::cache().absoluteFilePath("main.wwrepo"), QSettings::IniFormat);
    r.setIniCodec("UTF-8");
    r.beginGroup("Packages" + arch);
    PackageModel::PackageList all, bList, aList;
    for (auto name : mSolData.bPackages)
    {
        r.beginGroup(name);
        QString category = r.value("Category", tr("Other")).toString();
        QString tooltip = r.value("Help").toString();
        bList.append(PackageModel::Package{ name, category, tooltip, PT_PACKAGE});
        r.endGroup();
    }
    for (auto name : mSolData.aPackages)
    {
        r.beginGroup(name);
        QString category = r.value("Category", tr("Other")).toString();
        QString tooltip = r.value("Help").toString();
        aList.append(PackageModel::Package{ name, category, tooltip, PT_PACKAGE});
        r.endGroup();
    }
    for (auto name : r.childGroups())
    {
        r.beginGroup(name);
        auto type = r.value("Type").toInt();
        switch (type)
        {
        case PT_PACKAGE:
            if (!mSolData.bPackages.contains(name) && !mSolData.aPackages.contains(name))
            {
                QString category = r.value("Category", tr("Other")).toString();
                QString tooltip = r.value("Help").toString();
                all.append(PackageModel::Package{ name, category, tooltip, PT_PACKAGE});
            }
            break;
        case PT_WINE:
            mWineList.append(name);
            break;
        }
        r.endGroup();
    }
    r.endGroup();
    qSort(mWineList.begin(), mWineList.end(), wineLessThan);
    mCategoryList = r.value("Categories").toStringList();
    mCategoryList.sort();
    mCategoryList.insert(0, tr("All Packages"));
    ui->bWine->setText(mSolData.bWine);
    ui->aWine->setText(mSolData.aWine);
    auto cModel = new PackageModel(all, this);
    auto cSortModel = new PackageSortModel(this);
    cSortModel->setSourceModel(cModel);
    cSortModel->sort(0);
    cSortModel->setDynamicSortFilter(true);
    ui->cPackages->setModel(cSortModel);
    auto bModel = new PackageModel(bList, this);
    ui->bPackages->setModel(bModel);
    auto aModel = new PackageModel(aList, this);
    ui->aPackages->setModel(aModel);
}

EditSolutionDialog::~EditSolutionDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("EditSolutionDialog");
    s.setValue("Size", size());
    s.endGroup();
    delete ui;
}

void EditSolutionDialog::accept()
{
    QUrlQuery data;
    data.addQueryItem("slug", mSolData.slug);
    data.addQueryItem("bw", ui->bWine->text());
    data.addQueryItem("aw", ui->aWine->text());
    auto bpModel = ui->bPackages->model();
    for (auto i = 0, count = bpModel->rowCount(); i < count; ++i)
        data.addQueryItem("bp[]", bpModel->index(i, 0).data().toString());
    auto apModel = ui->aPackages->model();
    for (auto i = 0, count = apModel->rowCount(); i < count; ++i)
        data.addQueryItem("ap[]", apModel->index(i, 0).data().toString());
    data.addQueryItem("arch", mArch);
    PostDialog pd(API_URL + "?c=update", data, this);
    if (pd.exec() == QDialog::Accepted)
        QDialog::accept();
}

void EditSolutionDialog::on_category_clicked()
{
    SelectDialog sd(mCategoryList, ui->category->text(), this);
    sd.setWindowTitle(tr("Select Category"));
    if (sd.exec() == QDialog::Accepted)
    {
        auto cat = sd.selectedItem();
        ui->cPackages->model()->setData(QModelIndex(), cat, Qt::UserRole);
        ui->category->setText(cat);
    }
}

void EditSolutionDialog::on_bWine_clicked()
{
    SelectDialog sd(mWineList, ui->bWine->text(), this);
    sd.setWindowTitle(tr("Select Wine"));
    if (sd.exec() == QDialog::Accepted)
        ui->bWine->setText(sd.selectedItem());
}

void EditSolutionDialog::on_aWine_clicked()
{
    SelectDialog sd(mWineList, ui->aWine->text(), this);
    sd.setWindowTitle(tr("Select Wine"));
    if (sd.exec() == QDialog::Accepted)
        ui->aWine->setText(sd.selectedItem());
}

void EditSolutionDialog::on_bMoveWine_clicked()
{
    ui->aWine->setText(ui->bWine->text());
}

void EditSolutionDialog::on_aMoveWine_clicked()
{
    ui->bWine->setText(ui->aWine->text());
}
