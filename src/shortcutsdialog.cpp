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
#include <QPushButton>
#include <QUrl>

#include "ui_shortcutsdialog.h"
#include "shortcutsdialog.h"
#include "shortcutsmodel.h"
#include "dialogs.h"

ShortcutsDialog::ShortcutsDialog(const QString &solution, QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::ShortcutsDialog)
{
    ui->setupUi(this);
    auto model = new ShortcutsModel(this);
    model->setData(QModelIndex(), solution, Qt::UserRole);
    auto sortModel = new QSortFilterProxyModel(this);
    sortModel->setSourceModel(model);
    sortModel->sort(0);
    ui->shortcuts->setModel(sortModel);
    ui->shortcuts->setCurrentIndex(sortModel->index(0, 0));
}

ShortcutsDialog::~ShortcutsDialog()
{
    delete ui;
}

QString ShortcutsDialog::shortcut() const
{
    return ui->shortcuts->currentIndex().data(Qt::UserRole).toString();
}

void ShortcutsDialog::reject()
{
    if (Dialogs::confirm(tr("Are you sure you want to break debugger session?"), this))
        QDialog::reject();
}

void ShortcutsDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
