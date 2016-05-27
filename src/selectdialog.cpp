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

#include "ui_selectdialog.h"
#include "selectdialog.h"
#include "selectmodel.h"

SelectDialog::SelectDialog(const QStringList &list, const QString &current, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectDialog)
{
    ui->setupUi(this);
    SelectModel *model = new SelectModel(list, this);
    ui->items->setModel(model);
    QModelIndexList findList = model->match(model->index(0, 0), Qt::DisplayRole, current, -1, Qt::MatchCaseSensitive);
    if (!findList.isEmpty())
        ui->items->setCurrentIndex(findList.first());
}

SelectDialog::~SelectDialog()
{
    delete ui;
}

QString SelectDialog::selectedItem() const
{
    return ui->items->currentIndex().data().toString();
}

void SelectDialog::on_items_clicked(const QModelIndex &index)
{
    if (index.isValid())
        accept();
}
