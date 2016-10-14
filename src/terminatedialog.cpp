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

#include "ui_terminatedialog.h"
#include "terminatedialog.h"
#include "terminatemodel.h"
#include "mainmodel.h"

TerminateDialog::TerminateDialog(QAbstractItemModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TerminateDialog)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(tr("Terminate"), QDialogButtonBox::AcceptRole);
    QDesktopWidget *dw = QApplication::desktop();
    QSettings s("winewizard", "settings");
    s.beginGroup("TerminateDialog");
    resize(s.value("Size", QSize(dw->width() * 0.3, dw->height() * 0.4)).toSize());
    s.endGroup();
    TerminateModel *m = new TerminateModel(this);
    m->setSourceModel(model);
    ui->prefixes->setModel(m);
    ui->prefixes->setCurrentIndex(m->index(0, 0));
}

TerminateDialog::~TerminateDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("TerminateDialog");
    s.setValue("Size", size());
    s.endGroup();
    delete ui;
}

void TerminateDialog::accept()
{
    QAbstractItemModel *m = ui->prefixes->model();
    for (int i = m->rowCount() - 1; i >= 0; --i)
        if (m->index(i, 0).data(Qt::CheckStateRole).toUInt() == Qt::Checked)
            m->setData(m->index(i, 0), false, MainModel::ExecuteRole);
    QDialog::accept();
}

void TerminateDialog::on_prefixes_clicked(const QModelIndex &index)
{
    QAbstractItemModel *m = ui->prefixes->model();
    Qt::CheckState state = static_cast<Qt::CheckState>(m->data(index, Qt::CheckStateRole).toUInt());
    m->setData(index, (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);
}

void TerminateDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
