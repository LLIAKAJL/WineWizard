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
#include <QUrl>

#include "ui_createshortcutdialog.h"
#include "createshortcutdialog.h"
#include "shortcutsmodel.h"
#include "dialogs.h"

CreateShortcutDialog::CreateShortcutDialog(QAbstractItemModel *model, const QFileInfo &exe, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateShortcutDialog),
    mModel(model)
{
    ui->setupUi(this);
    ui->browseBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->name->setValidator(new QRegExpValidator(QRegExp(R"([^\\]*)"), this));
    ui->name->setText(exe.completeBaseName());
    ui->workDir->setText(exe.absolutePath());
}

CreateShortcutDialog::~CreateShortcutDialog()
{
    delete ui;
}

QString CreateShortcutDialog::name() const
{
    return ui->name->text();
}

QString CreateShortcutDialog::workDir() const
{
    return ui->workDir->text();
}

QString CreateShortcutDialog::args() const
{
    return ui->args->text();
}

void CreateShortcutDialog::on_browseBtn_clicked()
{
    auto dir = Dialogs::selectDir(ui->workDir->text(), this);
    if (!dir.isEmpty())
        ui->workDir->setText(dir);
}

void CreateShortcutDialog::on_name_textChanged(const QString &name)
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

bool CreateShortcutDialog::exists(const QString &name) const
{
    return !mModel->match(mModel->index(0, 0), Qt::DisplayRole, name, -1, Qt::MatchCaseSensitive).isEmpty();
}

void CreateShortcutDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
