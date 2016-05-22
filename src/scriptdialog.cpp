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
#include <QSettings>

#include "ui_scriptdialog.h"
#include "scriptdialog.h"

ScriptDialog::ScriptDialog(const QString &script, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptDialog)
{
    ui->setupUi(this);
    ui->script->setPlainText(script);
    QSettings s("winewizard", "settings");
    s.beginGroup("ScriptDialog");
    resize(s.value("Size", size()).toSize());
}

ScriptDialog::~ScriptDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("ScriptDialog");
    s.setValue("Size", size());
    delete ui;
}

void ScriptDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
