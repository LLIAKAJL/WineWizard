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
#include <QUrl>

#include "ui_outputdialog.h"
#include "outputdialog.h"
#include "filesystem.h"
#include "dialogs.h"

OutputDialog::OutputDialog(QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::OutputDialog)
{
    ui->setupUi(this);
    QSettings s("winewizard", "settings");
    s.beginGroup("OutputDialog");
    resize(s.value("Size", size()).toSize());
    s.endGroup();
    auto viewBtn = ui->buttonBox->addButton(tr("View Solution"), QDialogButtonBox::ActionRole);
    connect(viewBtn, &QPushButton::clicked, this, &OutputDialog::viewClicked);
    viewBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    auto errors = FS::readFile(FS::temp().absoluteFilePath("errors"));
    ui->errors->appendPlainText(errors);

    QSettings e(FS::mainPart("errors"), QSettings::IniFormat);
    for (auto package : e.childKeys())
        for (auto error : e.value(package).toStringList())
            if (errors.contains(error))
            {
                ui->advice->appendPlainText(tr("You can try to install package: %1.").arg(package));
                break;
            }
    if (ui->advice->toPlainText().isEmpty())
        ui->advice->appendPlainText(tr("You can try to change the version of Wine."));
}

OutputDialog::~OutputDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("OutputDialog");
    s.setValue("Size", size());
    s.endGroup();
    delete ui;
}

void OutputDialog::viewClicked()
{
    QSettings s(FS::temp().absoluteFilePath("solution"), QSettings::IniFormat);
    s.setIniCodec("UTF-8");
    QDesktopServices::openUrl(QUrl(s.value("URL").toString()));
}

void OutputDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
