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

#include <QDesktopServices>
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

const int MAX_LENGTH = 5000;

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
            int li = ll.first().indexOf('-');
            int ri = rl.first().indexOf('-');
            QString lm = li > -1 ? ll.first().left(li) : ll.first();
            QString rm = ri > -1 ? rl.first().left(ri) : rl.first();
            if (lm == rm)
            {
                QString lm = li > -1 ? ll.first().remove(li + 1) : QString();
                QString rm = ri > -1 ? rl.first().remove(ri + 1) : QString();
                return lm > rm;
            }
            return lm.toInt() > rm.toInt();
        }
        return ll.first().toInt() > rl.first().toInt();
    }
    return ll.first().toInt() > rl.first().toInt();
}

EditSolutionDialog::EditSolutionDialog(const QString &arch, QWidget *parent, bool edit) :
    QDialog(parent),
    ui(new Ui::EditSolutionDialog),
    mArch(arch)
{
    ui->setupUi(this);
    ui->bMoveWine->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->aMoveWine->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    QPushButton *okBtn = ui->buttonBox->button(QDialogButtonBox::Ok);
    okBtn->setVisible(edit);
    okBtn->setDefault(edit);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(edit);
    QPushButton *closeBtn = ui->buttonBox->button(QDialogButtonBox::Close);
    closeBtn->setVisible(!edit);
    closeBtn->setDefault(!edit);

    ui->cPackagesFrame->setVisible(edit);
    ui->categoryFrame->setVisible(edit);
    ui->bWineFrame->setVisible(edit);
    ui->aWineFrame->setVisible(edit);
    ui->bWineView->setVisible(!edit);
    ui->aWineView->setVisible(!edit);
    ui->bPackages->setDragEnabled(edit);
    ui->aPackages->setDragEnabled(edit);
    ui->bScript->setReadOnly(!edit);
    ui->aScript->setReadOnly(!edit);
    QSettings s("winewizard", "settings");
    s.beginGroup("EditSolutionDialog");
    resize(s.value("Size", size()).toSize());
    s.endGroup();

    QByteArray data = FS::readFile(FS::temp().absoluteFilePath("solution")).toUtf8();
    QJsonObject jo = QJsonDocument::fromJson(data).object();
    QStringList bp;
    QJsonArray bpArr = jo.value("bp").toArray();
    for(QJsonArray::const_iterator iter = bpArr.begin(); iter != bpArr.end(); ++iter)
        bp.append((*iter).toString());
    QStringList ap;
    QJsonArray apArr = jo.value("ap").toArray();
    for(QJsonArray::const_iterator iter = apArr.begin(); iter != apArr.end(); ++iter)
        ap.append((*iter).toString());
    mSlug = jo.value("slug").toString();
    setWindowTitle(jo.value("name").toString());
    ui->bWineView->setText(jo.value("bw").toString());
    ui->aWineView->setText(jo.value("aw").toString());
    ui->bWine->setText(ui->bWineView->text());
    ui->aWine->setText(ui->aWineView->text());
    ui->bScript->setPlainText(jo.value("bs").toString());
    ui->aScript->setPlainText(jo.value("as").toString());

    QSettings r(FS::cache().absoluteFilePath("main.wwrepo"), QSettings::IniFormat);
    r.setIniCodec("UTF-8");
    r.beginGroup("Packages" + arch);
    PackageModel::PackageList all, bList, aList;
    for (const QString &name : bp)
    {
        r.beginGroup(name);
        QString category = r.value("Category", tr("Other")).toString();
        QString tooltip = r.value("Help").toString();
        bList.append(PackageModel::Package{ name, category, tooltip, PT_PACKAGE});
        r.endGroup();
    }
    for (const QString &name : ap)
    {
        r.beginGroup(name);
        QString category = r.value("Category", tr("Other")).toString();
        QString tooltip = r.value("Help").toString();
        aList.append(PackageModel::Package{ name, category, tooltip, PT_PACKAGE});
        r.endGroup();
    }
    for (const QString &name : r.childGroups())
    {
        r.beginGroup(name);
        int type = r.value("Type").toInt();
        switch (type)
        {
        case PT_PACKAGE:
            if (!bp.contains(name) && !ap.contains(name))
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
    PackageModel *cModel = new PackageModel(all, this);
    PackageSortModel *cSortModel = new PackageSortModel(this);
    cSortModel->setSourceModel(cModel);
    cSortModel->sort(0);
    cSortModel->setDynamicSortFilter(true);
    ui->cPackages->setModel(cSortModel);
    PackageModel *bModel = new PackageModel(bList, this);
    ui->bPackages->setModel(bModel);
    PackageModel *aModel = new PackageModel(aList, this);
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
    QJsonObject data;
    data.insert("arch", mArch);
    data.insert("slug", mSlug);
    data.insert("bw", ui->bWineView->text());
    data.insert("aw", ui->aWineView->text());
    QJsonArray bp;
    QAbstractItemModel *bpModel = ui->bPackages->model();
    for (int i = 0, count = bpModel->rowCount(); i < count; ++i)
        bp.append(bpModel->index(i, 0).data().toString());
    data.insert("bp", bp);
    QJsonArray ap;
    QAbstractItemModel *apModel = ui->aPackages->model();
    for (int i = 0, count = apModel->rowCount(); i < count; ++i)
        ap.append(apModel->index(i, 0).data().toString());
    data.insert("ap", ap);
    data.insert("bs", ui->bScript->toPlainText().trimmed());
    data.insert("as", ui->aScript->toPlainText().trimmed());
    PostDialog pd(API_URL + "?c=update", QJsonDocument(data), this);
    if (pd.exec() == QDialog::Accepted)
        QDialog::accept();
}

void EditSolutionDialog::on_category_clicked()
{
    SelectDialog sd(mCategoryList, ui->category->text(), this);
    sd.setWindowTitle(tr("Select Category"));
    if (sd.exec() == QDialog::Accepted)
    {
        QString cat = sd.selectedItem();
        ui->cPackages->model()->setData(QModelIndex(), cat, Qt::UserRole);
        ui->category->setText(cat);
    }
}

void EditSolutionDialog::on_bWine_clicked()
{
    SelectDialog sd(mWineList, ui->bWineView->text(), this);
    sd.setWindowTitle(tr("Select Wine"));
    if (sd.exec() == QDialog::Accepted)
    {
        ui->bWineView->setText(sd.selectedItem());
        ui->bWine->setText(ui->bWineView->text());
    }
}

void EditSolutionDialog::on_aWine_clicked()
{
    SelectDialog sd(mWineList, ui->aWineView->text(), this);
    sd.setWindowTitle(tr("Select Wine"));
    if (sd.exec() == QDialog::Accepted)
    {
        ui->aWineView->setText(sd.selectedItem());
        ui->aWine->setText(ui->aWineView->text());
    }
}

void EditSolutionDialog::on_bMoveWine_clicked()
{
    ui->aWineView->setText(ui->bWineView->text());
    ui->aWine->setText(ui->bWineView->text());
}

void EditSolutionDialog::on_aMoveWine_clicked()
{
    ui->bWineView->setText(ui->aWineView->text());
    ui->bWine->setText(ui->aWineView->text());
}

void EditSolutionDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl(HELP_URL));
}
