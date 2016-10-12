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

#include <QStandardPaths>
#include <QJsonDocument>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QUuid>

#include "editprefixdialog.h"
#include "setupwizard.h"
#include "ui_intropage.h"
#include "newappdialog.h"
#include "intropage.h"
#include "appmodel.h"
#include "utils.h"

IntroPage::IntroPage(const QString &exe, QAbstractItemModel *model, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::IntroPage),
    mExe(exe),
    mPageNum(1)
{
    ui->setupUi(this);
    ui->alter->hide();
    ui->leftBtn->setIcon(QIcon::fromTheme("go-previous", style()->standardIcon(QStyle::SP_ArrowLeft)));
    ui->rightBtn->setIcon(QIcon::fromTheme("go-next", style()->standardIcon(QStyle::SP_ArrowRight)));
    ui->browseBtn->setIcon(QIcon::fromTheme("folder-open", style()->standardIcon(QStyle::SP_DirOpenIcon)));
    ui->addBtn->setIcon(QIcon::fromTheme("list-add"));
    ui->searchBtn->setIcon(QIcon::fromTheme("edit-find"));
    registerField("app", this, "app");
    registerField("exe", this, "exe");
    registerField("args", this, "args");
    registerField("workDir", this, "workDir");
    registerField("prefix", this, "prefix");
    ui->workDir->setText(QFileInfo(mExe).absolutePath());
    ui->prefixes->setModel(model);
    QItemSelectionModel *psm = ui->prefixes->selectionModel();
    connect(psm, &QItemSelectionModel::currentRowChanged, this, &IntroPage::prefixChanged);
    ui->prefixes->setCurrentIndex(model->index(0, 0));
    AppModel *appModel = new AppModel(this);
    connect(appModel, &AppModel::readyOutput, ui->wait,   &WaitForm::readyOutput);
    connect(appModel, &AppModel::readyError,  ui->wait,   &WaitForm::readyError);
    connect(appModel, &AppModel::started,     ui->result, &QListView::hide);
    connect(appModel, &AppModel::started,     ui->alter,  &QLabel::hide);
    connect(appModel, &AppModel::started,     ui->wait,   &WaitForm::start);
    connect(appModel, &AppModel::finished,    ui->wait,   &WaitForm::hide);
    connect(appModel, &AppModel::finished,    ui->result, &QListView::show);
    connect(appModel, &AppModel::error,       ui->wait,   &WaitForm::error);
    connect(ui->wait, &WaitForm::retry,       this,       &IntroPage::retry);
    connect(appModel, &AppModel::finished,    this,       &IntroPage::finished);
    ui->result->setModel(appModel);
    ui->prefixes->setCurrentIndex(model->index(0, 0));
    search();
}

IntroPage::~IntroPage()
{
    delete ui;
}

bool IntroPage::isComplete() const
{
    QFileInfo wdi(workDir());
    return ((ui->updateRB->isChecked() && ui->prefixes->currentIndex().isValid()) ||
           (ui->installRB->isChecked() && app().isValid())) &&
            wdi.exists() && wdi.isDir() && QFile::exists(exe());
}

int IntroPage::nextId() const
{
    return ui->installRB->isChecked() ? SetupWizard::PageSolution : SetupWizard::PageUpdate;
}

void IntroPage::finished()
{
    ui->search->setEnabled(true);
    ui->searchBtn->setEnabled(true);
    ui->addBtn->setEnabled(true);
    QAbstractItemModel *m = ui->result->model();
    int count = m->data(QModelIndex(), AppModel::PageCountRole).toInt();
    ui->pagenum->setText(/*(mPageNum <= count) ? */tr("%1 from %2").arg(mPageNum).arg(count)/* : tr("wait")*/);
    ui->leftBtn->setEnabled(mPageNum > 1);
    ui->rightBtn->setEnabled(mPageNum < count);
    QString alter = m->data(QModelIndex(), AppModel::AlterRole).toString();
    ui->alter->setText(alter);
    ui->alter->setHidden(alter.isEmpty());
    QModelIndexList l = m->match(m->index(0, 0), Qt::DisplayRole, prepareSearch(), -1, Qt::MatchFixedString);
    if (!l.isEmpty())
        ui->result->setCurrentIndex(l.first());
    else
        ui->result->setCurrentIndex(m->index(0, 0));
    if (prepareSearch().isEmpty())
        ui->search->setFocus();
    emit completeChanged();
}

void IntroPage::on_installRB_toggled(bool checked)
{
    ui->stack->setCurrentWidget(checked ? ui->pageInstall : ui->pageUpdate);
    if (checked)
        ui->search->setFocus();
    else
        ui->prefixes->setFocus();
    emit completeChanged();
}

const QString &IntroPage::exe() const
{
    return mExe;
}

QString IntroPage::args() const
{
    return ui->args->text().trimmed();
}

QString IntroPage::workDir() const
{
    return ui->workDir->text().trimmed();
}

QModelIndex IntroPage::app() const
{
    return ui->result->currentIndex();
}

QModelIndex IntroPage::prefix() const
{
    return ui->prefixes->currentIndex();
}

void IntroPage::search(int pagenum)
{
    ui->search->setEnabled(false);
    ui->searchBtn->setEnabled(false);
    ui->leftBtn->setEnabled(false);
    ui->rightBtn->setEnabled(false);
    ui->addBtn->setEnabled(false);
//    ui->pagenum->setText(tr("wait"));
    mPageNum = pagenum;
    ui->wait->start();
    QPair<QString, int> request;
    request.first = prepareSearch();
    request.second = mPageNum;
    ui->result->model()->setData(QModelIndex(), QVariant::fromValue(request), AppModel::ResetRole);
    emit completeChanged();
}

QString IntroPage::prepareSearch() const
{
    return ui->search->text().split(QChar::Space, QString::SkipEmptyParts).join(QChar::Space);
}

void IntroPage::on_workDir_textChanged(const QString &)
{
    QPalette p = palette();
    QFileInfo i(workDir());
    if (!i.exists() || !i.isDir())
        p.setColor(QPalette::Text, QColor(Qt::red));
    ui->workDir->setPalette(p);
    emit completeChanged();
}

void IntroPage::on_addBtn_clicked()
{
    NewAppDialog nd(ui->search->text());
    if (nd.exec() == NewAppDialog::Accepted)
        ui->result->model()->setData(QModelIndex(), nd.name(), AppModel::NewRole);
}

void IntroPage::on_browseBtn_clicked()
{
    QString dir = workDir();
    QFileInfo i(workDir());
    if (!i.exists() || !i.isDir())
        dir = QDir::homePath();
    QFileInfo info = Utils::selectDir(dir, wizard());
    if (info.exists())
        ui->workDir->setText(info.absoluteFilePath());
}

void IntroPage::retry()
{
    search(mPageNum);
}

void IntroPage::prefixChanged(const QModelIndex &index)
{
    ui->updateRB->setEnabled(index.isValid());
}

void IntroPage::on_leftBtn_clicked()
{
    search(mPageNum - 1);
}

void IntroPage::on_rightBtn_clicked()
{
    search(mPageNum + 1);
}

void IntroPage::on_searchBtn_clicked()
{
    search();
}

void IntroPage::on_alter_linkActivated(const QString &link)
{
    ui->search->setText(link);
    search();
}

void IntroPage::on_search_textChanged(const QString &)
{
    ui->leftBtn->setEnabled(false);
    ui->rightBtn->setEnabled(false);
    ui->addBtn->setEnabled(false);
}
