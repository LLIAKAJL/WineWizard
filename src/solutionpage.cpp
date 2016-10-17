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

#include <QSortFilterProxyModel>
#include <QMouseEvent>
#include <QFile>
#include <QDir>

#include "editsolutiondialog.h"
#include "ui_solutionpage.h"
#include "setupwizard.h"
#include "solutionmodel.h"
#include "solutionpage.h"
#include "wintypes.h"
#include "appmodel.h"
#include "utils.h"

SolutionPage::SolutionPage(const QString &exe, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::SolutionPage)
{
    ui->setupUi(this);
    ui->editBtn->setIcon(QIcon::fromTheme("document-properties"));
    ui->copyBtn->setIcon(QIcon::fromTheme("edit-copy"));
    SolutionModel *m = new SolutionModel(this);
    connect(m, &SolutionModel::readyOutput, ui->wait,   &WaitForm::readyOutput);
    connect(m, &SolutionModel::readyError,  ui->wait,   &WaitForm::readyError);
    connect(m, &SolutionModel::started,     ui->result, &QTableView::hide);
    connect(m, &SolutionModel::started,     ui->wait,   &WaitForm::start);
    connect(m, &SolutionModel::finished,    ui->wait,   &WaitForm::hide);
    connect(m, &SolutionModel::finished,    ui->result, &QFrame::show);
    connect(m, &SolutionModel::finished,    this,       &SolutionPage::finished);
    connect(m, &SolutionModel::error,       ui->wait,   &WaitForm::error);
    QSortFilterProxyModel *sm = new QSortFilterProxyModel(this);
    sm->setSourceModel(m);
    sm->sort(0, Qt::DescendingOrder);
    ui->result->setModel(sm);
    connect(ui->wait, &WaitForm::retry, this, &SolutionPage::updateData);
    registerField("arch", this, "arch");
    registerField("solution", this, "solution");
    if (QFile::exists(exe))
    {
        QFile f(exe);
        f.open(QFile::ReadOnly);
        IMAGE_DOS_HEADER dh;
        f.read(reinterpret_cast<char *>(&dh), sizeof(IMAGE_DOS_HEADER));
        if (f.error() == QFile::NoError)
        {
            if ((dh.e_magic == IMAGE_DOS_SIGNATURE) && (dh.e_lfanew % sizeof(DWORD) == 0))
            {
                f.seek(dh.e_lfanew);
                if (f.error() == QFile::NoError)
                {
                    IMAGE_NT_HEADERS nth;
                    f.read(reinterpret_cast<char *>(&nth), sizeof(IMAGE_NT_HEADERS));
                    if (f.error() == QFile::NoError)
                        ui->win64->setChecked(nth.Signature == IMAGE_NT_SIGNATURE &&
                                    nth.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC);
                }
            }
        }
        f.close();
    }
    connect(ui->win32, &QRadioButton::toggled, this, &SolutionPage::updateData);
    connect(ui->wait, &WaitForm::retry, this, &SolutionPage::updateData);
    QItemSelectionModel *rsm = ui->result->selectionModel();
    connect(rsm, &QItemSelectionModel::currentRowChanged, this, &SolutionPage::currentChanged);
}

SolutionPage::~SolutionPage()
{
    delete ui;
}

void SolutionPage::initializePage()
{
    updateData();
}

bool SolutionPage::isComplete() const
{
    return solution().isValid();
}

int SolutionPage::nextId() const
{
    return SetupWizard::PageInstall;
}

void SolutionPage::updateData()
{
    ui->win32->setEnabled(false);
    ui->win64->setEnabled(false);
    ui->copyBtn->setEnabled(false);
    ui->editBtn->setEnabled(false);
    QAbstractItemModel *m = ui->result->model();
    QPair<QString ,int> request;
    request.first = arch();
    request.second = field("app").toModelIndex().data(AppModel::IdRole).toInt();
    m->setData(QModelIndex(), QVariant::fromValue(request), SolutionModel::ResetRole);
    emit completeChanged();
}

void SolutionPage::finished()
{
    ui->win32->setEnabled(true);
    ui->win64->setEnabled(true);
    ui->result->setCurrentIndex(ui->result->model()->index(0, 0));
    ui->result->setFocus();
    ui->result->resizeColumnsToContents();
    emit completeChanged();
}

void SolutionPage::currentChanged(const QModelIndex &index)
{
    bool valid = index.isValid();
    ui->copyBtn->setEnabled(valid);
    ui->editBtn->setEnabled(valid);
}

QString SolutionPage::arch() const
{
    return ui->win64->isChecked() ? "64" : "32";
}

QModelIndex SolutionPage::solution() const
{
    return ui->result->currentIndex();
}

void SolutionPage::on_copyBtn_clicked()
{
    QAbstractItemModel *m = ui->result->model();
    m->setData(ui->result->currentIndex(), true, SolutionModel::CloneRole);
    ui->result->setCurrentIndex(m->index(m->rowCount() - 1, 0));
}

void SolutionPage::on_editBtn_clicked()
{
    EditSolutionDialog(solution(), field("out").toString(), field("err").toString(), this).exec();
}

void SolutionPage::on_result_doubleClicked(const QModelIndex &index)
{
    if (index.isValid())
        on_editBtn_clicked();
}
