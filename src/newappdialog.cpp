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
#include <QUrl>

#include "ui_newappdialog.h"
#include "newappdialog.h"

NewAppDialog::NewAppDialog(const QString &name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAppDialog)
{
    ui->setupUi(this);
    resize(QApplication::desktop()->width() * 0.5, sizeHint().height());
    setFixedHeight(height());
    ui->name->setText(prepareName(name));
}

NewAppDialog::~NewAppDialog()
{
    delete ui;
}

QString NewAppDialog::name() const
{
    return prepareName(ui->name->text());
}

void NewAppDialog::on_name_textChanged(const QString &name)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(name.trimmed().isEmpty());
}

QString NewAppDialog::prepareName(const QString &name) const
{
    return name.split(QChar::Space, QString::SkipEmptyParts).join(QChar::Space);
}

void NewAppDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
