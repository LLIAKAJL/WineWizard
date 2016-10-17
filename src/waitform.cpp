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

#include "ui_waitform.h"
#include "waitform.h"

WaitForm::WaitForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaitForm)
{
    ui->setupUi(this);
    ui->retry->setIcon(QIcon::fromTheme("view-refresh", style()->standardIcon(QStyle::SP_BrowserReload)));
    connect(ui->retry, &QPushButton::clicked, this, &WaitForm::retry);
}

WaitForm::~WaitForm()
{
    delete ui;
}

void WaitForm::start()
{
    ui->output->clear();
    ui->retry->hide();
    ui->error->hide();
    show();
}

void WaitForm::error(int code)
{
    ui->error->setText(tr("Process finished with code %1").arg(code));
    ui->error->show();
    ui->retry->show();
}

void WaitForm::appendOut(const QString &text)
{
    ui->output->moveCursor(QTextCursor::End);
    ui->output->insertHtml(QString(text).replace('\n', "<br>"));
}

void WaitForm::readyOutput(const QString &data)
{
    appendOut(data);
}

void WaitForm::readyError(const QString &data)
{
    appendOut(QString(R"(<span style="color:#ff0000;">%1</span>)").arg(data));
}
