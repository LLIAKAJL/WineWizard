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

#include <QSortFilterProxyModel>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

#include "qtsingleapplication/QtSingleApplication"
#include "installwizard.h"
#include "ui_intropage.h"
#include "prefixmodel.h"
#include "filesystem.h"
#include "intropage.h"
#include "executor.h"
#include "dialogs.h"

IntroPage::IntroPage(const QString &exe, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::IntroPage),
    mExe(exe)
{
    ui->setupUi(this);
    ui->browseBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->typePages->setCurrentIndex(0);
    PrefixModel *model = new PrefixModel(this);
    QSortFilterProxyModel *sortModel = new QSortFilterProxyModel(this);
    sortModel->setSourceModel(model);
    sortModel->sort(0);
    ui->prefixes->setModel(sortModel);
    ui->updateRB->setEnabled(sortModel->rowCount() > 0);

    QString workDir = QFileInfo(mExe).absolutePath();
    ui->workDir->setText(workDir);
    ui->workDir->setToolTip(workDir);
    connect(ui->search, &QLineEdit::textChanged, this, &IntroPage::completeChanged);
    registerField("arch*", this, "arch");
    registerField("search*", this, "search");
    registerField("exe", this, "exe");
    registerField("args", ui->args);
    registerField("work_dir", ui->workDir);
}

IntroPage::~IntroPage()
{
    delete ui;
}

int IntroPage::nextId() const
{
    return InstallWizard::PageSolution;
}

bool IntroPage::isComplete() const
{
    switch (ui->typePages->currentIndex())
    {
    case 0:
        return !search().isEmpty();
    case 1:
        return ui->prefixes->currentIndex() >= 0;
    default:
        return false;
    }
}

bool IntroPage::validatePage()
{
    if (ui->updateRB->isChecked())
    {
        QString prefix = ui->prefixes->currentData(PrefixModel::PrefixRole).toString();
        Executor *e = new Executor(prefix);
        QString dir = ui->workDir->text();
        QString args = ui->args->text();
        e->start(QString(FS::readFile(":/run")).arg(mExe).arg(dir).arg(args),
                 false, QProcess::ForwardedChannels);
        wizard()->setProperty("finish", true);
        wizard()->reject();
        return false;
    }
    else
        return true;
}

QString IntroPage::arch() const
{
    return ui->win32->isChecked() ? "32" : "64";
}

const QString &IntroPage::exe() const
{
    return mExe;
}

QString IntroPage::search() const
{
    QString s = ui->search->text().trimmed();
    if (!s.isEmpty())
    {
        QChar first = s.at(0);
        if (first.isLetter() && first.isLower())
        {
            QStringList words = s.split(QChar::Space, QString::SkipEmptyParts);
            for (int i = words.count() - 1; i >= 0; --i)
                words[i][0] = words.at(i).at(0).toUpper();
            s = words.join(QChar::Space);
        }
    }
    return s;
}

void IntroPage::on_browseBtn_clicked()
{
    QString workDir = Dialogs::dir(ui->workDir->text(), this);
    if (!workDir.isEmpty())
    {
        ui->workDir->setText(workDir);
        ui->workDir->setToolTip(workDir);
    }
}

void IntroPage::on_newRB_toggled(bool checked)
{
    ui->typePages->setCurrentIndex(checked ? 0 : 1);
    if (checked)
        ui->search->setFocus();
    emit completeChanged();
}
