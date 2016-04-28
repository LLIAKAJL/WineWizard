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

#include "ui_scriptsdialog.h"
#include "scriptsdialog.h"
#include "filesystem.h"

ScriptsDialog::ScriptsDialog(const QString &before, const QString &after, QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::ScriptsDialog)
{
    ui->setupUi(this);
    ui->iconLbl->setPixmap(style()->standardPixmap(QStyle::SP_MessageBoxWarning));
    auto viewBtn = ui->buttonBox->addButton(tr("View Solution"), QDialogButtonBox::ActionRole);
    connect(viewBtn, &QPushButton::clicked, this, &ScriptsDialog::viewClicked);
    viewBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ScriptsDialog::accept);
    connect(ui->buttonBox->button(QDialogButtonBox::Discard), &QPushButton::clicked, this, &ScriptsDialog::reject);
    QSettings s("winewizard", "settings");
    s.beginGroup("ScriptsDialog");
    resize(s.value("Size", size()).toSize());
    s.endGroup();
    ui->before->setPlainText(before);
    ui->after->setPlainText(after);
}

ScriptsDialog::~ScriptsDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("ScriptsDialog");
    s.setValue("Size", size());
    s.endGroup();
    delete ui;
}

void ScriptsDialog::viewClicked()
{
    QSettings s(FS::temp().absoluteFilePath("solution"), QSettings::IniFormat);
    s.setIniCodec("UTF-8");
    QDesktopServices::openUrl(QUrl(s.value("URL").toString()));
}
