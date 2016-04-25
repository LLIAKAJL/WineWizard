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

#include "ui_solutiondialog.h"
#include "solutiondialog.h"
#include "filesystem.h"
#include "dialogs.h"

SolutionDialog::SolutionDialog(QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::SolutionDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    auto searchBtn = ui->buttonBox->addButton(tr("Search"), QDialogButtonBox::ActionRole);
    connect(searchBtn, &QPushButton::clicked, this, &SolutionDialog::searchClicked);
    searchBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
}

SolutionDialog::~SolutionDialog()
{
    delete ui;
}

QString SolutionDialog::url() const
{
    return ui->url->text();
}

void SolutionDialog::on_url_textChanged(const QString &url)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(url.isEmpty());
}

void SolutionDialog::searchClicked()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/solutions/"));
}

void SolutionDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
