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
#include <QUrl>

#include "ui_settingsdialog.h"
#include "settingsdialog.h"
#include "filesystem.h"
#include "dialogs.h"

SettingsDialog::SettingsDialog(const QModelIndex &index, QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::SettingsDialog),
    mIndex(index)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->name->setValidator(new QRegExpValidator(QRegExp(R"([^\\]*)"), this));
    ui->name->setText(index.data().toString());
    ui->icon->setIcon(index.data(Qt::DecorationRole).value<QIcon>());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::accept()
{
    auto model = const_cast<QAbstractItemModel *>(mIndex.model());
    model->setData(mIndex, ui->name->text());
    if (!mIcon.isEmpty())
        model->setData(mIndex, mIcon, Qt::DecorationRole);
    QDialog::accept();
}

void SettingsDialog::on_name_textChanged(const QString &name)
{
    if (name.isEmpty())
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->status->setText(tr("Empty name!"));
    }
    else if (exists(name))
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

bool SettingsDialog::exists(const QString &name) const
{
    if (mIndex.data().toString() == name)
        return false;
    auto model = mIndex.model();
    return !model->match(model->index(0, 0), Qt::DisplayRole, name, -1, Qt::MatchCaseSensitive).isEmpty();
}

void SettingsDialog::on_icon_clicked()
{
    auto path = Dialogs::open(tr("Select Icon"), tr("Icon files (*.png)"), this);
    if (!path.isEmpty())
    {
        mIcon = path;
        ui->icon->setIcon(QIcon(mIcon));
    }
}

void SettingsDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
