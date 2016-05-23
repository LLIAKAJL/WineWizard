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


#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>

#include "ui_editshortcutdialog.h"
#include "editshortcutdialog.h"
#include "filesystem.h"
#include "netdialog.h"
#include "dialogs.h"

EditShortcutDialog::EditShortcutDialog(const QString &prefixHash, const QModelIndex &index, QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::EditShortcutDialog),
    mIndex(index),
    mPrefixHash(prefixHash)
{
    ui->setupUi(this);

    ui->browseBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));

    ui->name->setValidator(new QRegExpValidator(QRegExp(R"([^\\]*)"), this));
    ui->name->setText(index.data().toString());
    ui->workDir->setText(index.data(WorkDirRole).toString());
    ui->args->setText(index.data(ArgsRole).toString());
    ui->icon->setIcon(index.data(Qt::DecorationRole).value<QIcon>());
}

EditShortcutDialog::~EditShortcutDialog()
{
    delete ui;
}

void EditShortcutDialog::accept()
{
    QAbstractItemModel *model = const_cast<QAbstractItemModel *>(mIndex.model());
    model->setData(mIndex, ui->name->text().trimmed());
    bool debug = false;
//    QString dir = FS::toWinPath(mPrefixHash, ui->workDir->text());
    if (mIndex.data(WorkDirRole).toString() != ui->workDir->text())
        debug = true;
    model->setData(mIndex, ui->workDir->text(), WorkDirRole);
    QString tArgs = ui->args->text().trimmed();
    if (mIndex.data(ArgsRole).toString() != tArgs)
        debug = true;
    model->setData(mIndex, tArgs, ArgsRole);
    if (debug)
        model->setData(mIndex, true, DebugRole);
    if (!mIcon.isEmpty())
    {
        model->setData(mIndex, QIcon(mIcon), Qt::DecorationRole);
        model->setData(mIndex, mIcon, IconRole);
    }
    QDialog::accept();
}

bool EditShortcutDialog::exists(const QString &name) const
{
    if (mIndex.data().toString() == name)
        return false;
    QAbstractItemModel *aModel = const_cast<QAbstractItemModel *>(mIndex.model());
    QStandardItemModel *model = static_cast<QStandardItemModel *>(aModel);
    return !model->findItems(name, Qt::MatchCaseSensitive).isEmpty();
}

void EditShortcutDialog::on_name_textChanged(const QString &name)
{
    QString tName = name.trimmed();
    if (tName.isEmpty())
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->status->setText(tr("Empty name!"));
    }
    else if (exists(tName))
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->status->setText(tr("Name is already used!"));
    }
    else
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->status->setText(tr("OK"));
    }
}

void EditShortcutDialog::on_icon_clicked()
{
    auto path = Dialogs::open(tr("Select Icon"), tr("Icon files (*.png)"), this);
    if (!path.isEmpty())
    {
        mIcon = path;
        ui->icon->setIcon(QIcon(mIcon));
    }
}

void EditShortcutDialog::on_browseBtn_clicked()
{
    QString dir = Dialogs::selectDir(ui->workDir->text(), this);
    if (!dir.isEmpty())
        ui->workDir->setText(dir);
}

void EditShortcutDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl(HELP_URL));
}
