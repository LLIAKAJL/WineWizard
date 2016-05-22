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
#include "netdialog.h"

TerminalDialog::TerminalDialog(QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::TerminalDialog)
{
    ui->setupUi(this);
    QSettings s("winewizard", "settings");
    s.beginGroup("TerminalDialog");
    resize(s.value("Size", size()).toSize());
    ui->splitter->restoreState(s.value("Splitter").toByteArray());
    ui->close->setChecked(s.value("Close", false).toBool());
    s.endGroup();
    ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(false);
}

TerminalDialog::~TerminalDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("TerminalDialog");
    s.setValue("Size", size());
    s.setValue("Splitter", ui->splitter->saveState());
    s.setValue("Close", ui->close->isChecked());
    s.endGroup();
    delete ui;
}

void TerminalDialog::appendOut(const QString &text)
{
    QTextCursor prev = ui->out->textCursor();
    ui->out->moveCursor(QTextCursor::End);
    ui->out->insertPlainText(text);
    ui->out->setTextCursor(prev);
}

void TerminalDialog::appendErr(const QString &text)
{
    QTextCursor prev = ui->err->textCursor();
    ui->err->moveCursor(QTextCursor::End);
    ui->err->insertPlainText(text);
    ui->err->setTextCursor(prev);
}

void TerminalDialog::reject()
{
    if (ui->buttonBox->button(QDialogButtonBox::Close)->isEnabled())
        QDialog::reject();
}

void TerminalDialog::executeFinished()
{
    bool empty = ui->out->toPlainText().isEmpty() && ui->err->toPlainText().isEmpty();
    if (ui->close->isChecked() || empty)
        QDialog::accept();
    ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(true);
}

void TerminalDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl(HELP_URL));
}
