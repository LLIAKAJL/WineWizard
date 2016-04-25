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

#include "ui_terminaldialog.h"
#include "terminaldialog.h"

TerminalDialog::TerminalDialog(QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::TerminalDialog)
{
    ui->setupUi(this);
    QSettings s("winewizard", "settings");
    s.beginGroup("TerminalDialog");
    resize(s.value("Size", size()).toSize());
    s.endGroup();
    connect(this, &TerminalDialog::appendText, ui->terminal, &QPlainTextEdit::appendPlainText);
    ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(false);
}

TerminalDialog::~TerminalDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("TerminalDialog");
    s.setValue("Size", size());
    s.endGroup();
    delete ui;
}

void TerminalDialog::reject()
{
}

void TerminalDialog::executeFinished(int exitCode)
{
    if (exitCode == 0)
        QDialog::accept();
    else
        ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(true);
}

void TerminalDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
