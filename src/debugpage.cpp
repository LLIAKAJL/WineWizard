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

#include <QSortFilterProxyModel>
#include <QDesktopServices>
#include <QDesktopWidget>

#include "solutiondialog.h"
#include "installwizard.h"
#include "shortcutmodel.h"
#include "ui_debugpage.h"
#include "searchmodel.h"
#include "filesystem.h"
#include "debugpage.h"
#include "executor.h"
#include "dialogs.h"

DebugPage::DebugPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::DebugPage)
{
    ui->setupUi(this);
    connect(ui->yes, &QRadioButton::toggled, this, &DebugPage::completeChanged);
    connect(ui->no, &QRadioButton::toggled, this, &DebugPage::completeChanged);
    connect(ui->end, &QRadioButton::toggled, this, &DebugPage::completeChanged);
    ShortcutModel *sm = new ShortcutModel(this);
    QSortFilterProxyModel *ssm = new QSortFilterProxyModel(this);
    ssm->setSourceModel(sm);
    ui->shortcuts->setModel(ssm);
}

DebugPage::~DebugPage()
{
    delete ui;
}

void DebugPage::initializePage()
{
    ui->pages->setCurrentIndex(1);
    setSubTitle(tr("Is your application worked correctly?"));
    QSortFilterProxyModel *m = static_cast<QSortFilterProxyModel *>(ui->shortcuts->model());
    QString prefix = field("app").toModelIndex().data(SearchModel::PrefixRole).toString();
    m->setData(QModelIndex(), prefix, ShortcutModel::ResetRole);
    if (m->rowCount() > 0)
    {
        ui->shortcuts->setCurrentIndex(m->index(0, 0));
        ui->yes->setEnabled(true);
    }
    else
        ui->yes->setEnabled(false);
}

int DebugPage::nextId() const
{
    return InstallWizard::PageWin;
}

bool DebugPage::isComplete() const
{
    return ui->yes->isChecked() || ui->end->isChecked() || ui->no->isChecked();
}

bool DebugPage::validatePage()
{
    if (ui->no->isChecked())
    {
        if (SolutionDialog(wizard(), wizard(), true).exec() == SolutionDialog::Accepted)
        {
            uncheckButtons();
            wizard()->back();
            wizard()->back();
            wizard()->next();
        }
        return false;
    }
    else if (ui->yes->isChecked())
    {
        wizard()->showMinimized();
        setField("out", QString());
        setField("err", QString());
        ui->pages->setCurrentIndex(0);
        uncheckButtons();
        setSubTitle(tr("Application is running, please wait..."));
        QString prefix = field("app").toModelIndex().data(SearchModel::PrefixRole).toString();
        QModelIndex index = ui->shortcuts->currentIndex();
        QString exe = index.data(ShortcutModel::ExeRole).toString();
        QString workDir = index.data(ShortcutModel::WorkDirRole).toString();
        QString args = index.data(ShortcutModel::ArgsRole).toString();
        QString script = QString(FS::readFile(":/run")).arg(exe).arg(workDir).arg(args);
        Executor *e = new Executor(prefix, this);
        connect(e, &Executor::destroyed, this, &DebugPage::finished);
        connect(e, &Executor::readyOutput, this, &DebugPage::readyOutput);
        connect(e, &Executor::readyError, this, &DebugPage::readyError);
        e->start(script);
        return false;
    }
    return true;
}

void DebugPage::finished()
{
    wizard()->back();
    wizard()->next();
    if (wizard()->isMinimized())
    {
        wizard()->showNormal();
        QSize center = (QApplication::desktop()->size() - wizard()->size()) * 0.5;
        wizard()->move(center.width(), center.height());
    }
}

void DebugPage::readyOutput(const QString &text)
{
    setField("out", field("out").toString() + text);
    appendOut(text);
}

void DebugPage::readyError(const QString &text)
{
    setField("err", field("err").toString() + text);
    appendOut(QString(R"(<span style=" color:#ff0000;">%1</span>)").arg(QString(text)));
}

void DebugPage::uncheckButtons()
{
    if (ui->buttonGroup->checkedButton())
    {
        ui->buttonGroup->setExclusive(false);
        ui->buttonGroup->checkedButton()->setChecked(false);
        ui->buttonGroup->setExclusive(true);
    }
}

void DebugPage::appendOut(const QString &text)
{
    QTextCursor prev = ui->out->textCursor();
    ui->out->moveCursor(QTextCursor::End);
    ui->out->insertHtml(QString(text).replace('\n', "<br>"));
    ui->out->setTextCursor(prev);
}
