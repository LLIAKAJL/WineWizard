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
#include <QUrl>

#include "ui_settingsdialog.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    QSettings s("winewizard", "settings");
    s.beginGroup("VideoSettings");
    int sw = s.value("ScreenWidth", -1).toInt();
    int sh = s.value("ScreenHeight", -1).toInt();
    int vm = s.value("VideoMemorySize", -1).toInt();
    s.endGroup();
    ui->scrSizeAuto->setChecked(sw < 0 || sh < 0);
    ui->vmAuto->setChecked(vm < 0);
    ui->width->setValue(sw);
    ui->height->setValue(sh);
    ui->vm->setValue(vm);
    ui->useScripts->setChecked(s.value("UseScripts", false).toBool());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::accept()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("VideoSettings");
    if (ui->scrSizeAuto->isChecked())
    {
        s.setValue("ScreenWidth", -1);
        s.setValue("ScreenHeight", -1);
    }
    else
    {
        s.setValue("ScreenWidth", ui->width->value());
        s.setValue("ScreenHeight", ui->height->value());
    }
    s.setValue("VideoMemorySize", ui->vmAuto->isChecked() ? -1 : ui->vm->value());
    s.endGroup();
    s.setValue("UseScripts", ui->useScripts->isChecked());
    QDialog::accept();
}

void SettingsDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
