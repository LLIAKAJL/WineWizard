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

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QPushButton>
#include <QSettings>
#include <QUrl>

#include "winesortmodel.h"
#include "ui_winedialog.h"
#include "winedialog.h"

WineDialog::WineDialog(const QString &current, PackageModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WineDialog),
    mModel(new WineSortModel(this))
{
    ui->setupUi(this);
    QDesktopWidget *dw = QApplication::desktop();
    QSettings s("winewizard", "settings");
    s.beginGroup("WineDialog");
    resize(s.value("Size", QSize(dw->width() * 0.4, dw->height() * 0.6)).toSize());
    s.endGroup();
    mModel->setSourceModel(model);
    mModel->sort(0);
    mModel->setDynamicSortFilter(true);
    mModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->list->setModel(mModel);
    connect(ui->list->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, [this](const QModelIndex &index)
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(index.isValid());
    });
    QModelIndexList l = mModel->match(mModel->index(0, 0), Qt::DisplayRole, current, -1, Qt::MatchFixedString);
    ui->list->setCurrentIndex(l.isEmpty() ? mModel->index(0, 0) : l.first());
    connect(ui->filter, &QLineEdit::textChanged, this, [this](const QString &filter)
    {
        mModel->setFilterFixedString(filter);
        ui->list->setCurrentIndex(mModel->index(0, 0));
    });
}

WineDialog::~WineDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("WineDialog");
    s.setValue("Size", size());
    s.endGroup();
    delete ui;
}

QString WineDialog::wine() const
{
    return ui->list->currentIndex().data().toString();
}

void WineDialog::on_list_clicked(const QModelIndex &index)
{
    if (index.isValid())
        accept();
}

void WineDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
