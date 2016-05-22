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
#include "netdialog.h"
#include "dialogs.h"

OutputDialog::OutputDialog(const Ex::Out &out, QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::OutputDialog)
{
    ui->setupUi(this);
    QSettings s("winewizard", "settings");
    s.beginGroup("OutputDialog");
    resize(s.value("Size", size()).toSize());
    ui->vSplitter->restoreState(s.value("VSplitter").toByteArray());
    ui->hSplitter->restoreState(s.value("HSplitter").toByteArray());
    s.endGroup();
    ui->out->appendPlainText(out.first);
    ui->err->appendPlainText(out.second);

    QSettings r(FS::cache().absoluteFilePath("main.wwrepo"), QSettings::IniFormat);
    {
        r.beginGroup("Errors");
        for (const QString &package : r.childGroups())
        {
            r.beginGroup(package);
            for (const QString &error : r.value("RE").toStringList())
                if (out.second.contains(error))
                {
                    ui->advice->appendPlainText(tr("You can try to install package: %1.").arg(package));
                    break;
                }
            r.endGroup();
        }
        r.endGroup();
    }
    if (ui->advice->toPlainText().isEmpty())
        ui->advice->appendPlainText(tr("You can try to change the version of Wine."));
}

OutputDialog::~OutputDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("OutputDialog");
    s.setValue("Size", size());
    s.setValue("VSplitter", ui->vSplitter->saveState());
    s.setValue("HSplitter", ui->hSplitter->saveState());
    s.endGroup();
    delete ui;
}

void OutputDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl(HELP_URL));
}
