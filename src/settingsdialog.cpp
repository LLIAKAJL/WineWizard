/***************************************************************************
 *   Copyright (C) 2016 by Vitalii Kachemtsev <LLIAKAJI@wwizard.net>         *
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
#include "wizard.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
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
    ui->autoquit->setChecked(s.value("Autoquit").toBool());
    int lang = s.value("Language", -1).toInt();
    if (lang < 0)
        ui->language->setCurrentIndex(localeToLangNum(QLocale::system().name()));
    else
        ui->language->setCurrentIndex(lang);
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
    s.setValue("Autoquit", ui->autoquit->isChecked());
    s.setValue("Language", ui->language->currentIndex());
    Wizard::setAutoquit(ui->autoquit->isChecked());
    Wizard::retranslate(ui->language->currentIndex());
    QDialog::accept();
}

void SettingsDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help/#settings"));
}

int SettingsDialog::localeToLangNum(const QString &locale) const
{
    if (locale.startsWith("ru"))
        return 1;
    return 0;
}
