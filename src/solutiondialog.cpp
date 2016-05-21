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
#include <QDesktopWidget>
#include <QPushButton>
#include <QSettings>
#include <QUrl>

#include "editsolutiondialog.h"
#include "ui_solutiondialog.h"
#include "solutiondialog.h"
#include "downloaddialog.h"
#include "searchmodel.h"
#include "filesystem.h"
#include "postdialog.h"
#include "dialogs.h"

const QString INSTALLED_MSG = QObject::tr("Application \"%1\" is already installed! " \
                                          "Are you sure you want to reinstall it?");
const int POSTS_PER_PAGE = 50;

SolutionDialog::SolutionDialog(const QStringList &runList, QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::SolutionDialog),
    mCurrentPage(1),
    mRunList(runList)
{
    ui->setupUi(this);
    QSettings s("winewizard", "settings");
    s.beginGroup("SolutionDialog");
    resize(s.value("Size", size()).toSize());

    ui->search->setValidator(new QRegExpValidator(QRegExp(R"([^\\]*)"), this));
    ui->searchBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    ui->edit32Btn->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    ui->edit64Btn->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    ui->addBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    ui->prevBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->nextBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->viewBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->solutions->setModel(new SearchModel(this));
    connect(ui->solutions->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &SolutionDialog::currentChanged);
    connect(ui->nextBtn, &QPushButton::clicked, this, [this](){ ++mCurrentPage; getSearch(); });
    connect(ui->prevBtn, &QPushButton::clicked, this, [this](){ --mCurrentPage; getSearch(); });
    ui->search->installEventFilter(this);
    getSearch();
    ui->search->setFocus();
}

SolutionDialog::~SolutionDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("SolutionDialog");
    s.setValue("Size", size());
    delete ui;
}

QString SolutionDialog::slug() const
{
    return ui->solutions->currentIndex().data(SearchModel::SlugRole).toString();
}

void SolutionDialog::accept()
{
    QString name = ui->solutions->currentIndex().data().toString();
    QString solution = FS::hash(name);
    if (mRunList.contains(solution))
    {
        Dialogs::error(tr("The application \"%1\" is already running!").arg(name));
        return;
    }
    if (FS::prefix(solution).exists())
        if (!Dialogs::confirm(INSTALLED_MSG.arg(name), this))
            return;
    QDialog::accept();
}

bool SolutionDialog::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->search)
    {
        if (e->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                searchExecute();
                return true;
            }
        }
        else if(e->type() == QEvent::FocusIn)
            ui->solutions->setCurrentIndex(QModelIndex());
    }
    return QDialog::eventFilter(o, e);
}

void SolutionDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}

void SolutionDialog::getSearch()
{
    QString searchEncode = QUrl::toPercentEncoding(ui->search->text().trimmed());
    QString url = API_URL + "?c=search&sv=" + searchEncode + "&pagenum=" + QString::number(mCurrentPage);
    QString outPath = FS::temp().absoluteFilePath("search");
    DownloadDialog dd(QStringList(url), outPath, this->isVisible() ? this : nullptr);
    if (dd.exec() == QDialog::Accepted)
    {
        QAbstractItemModel *model = ui->solutions->model();
        model->setData(QModelIndex(), true, SearchModel::ReloadRole);
        bool exists = model->data(QModelIndex(), SearchModel::ExistsRole).toBool();
        ui->addBtn->setDisabled(exists);
        int count = model->data(QModelIndex(), SearchModel::CountRole).toInt();
        int pageCount = count / POSTS_PER_PAGE + 1;
        ui->prevBtn->setEnabled(mCurrentPage != 1);
        ui->nextBtn->setEnabled(mCurrentPage != pageCount);
        if (count == 0)
        {
            if (!exists)
                ui->addBtn->setFocus();
            ui->navLbl->setText(tr("Empty"));
        }
        else
        {
            ui->navLbl->setText(tr("%1 from %2").arg(mCurrentPage).arg(pageCount));
            ui->solutions->setCurrentIndex(model->index(0, 0));
            ui->solutions->setFocus();
        }
    }
}

void SolutionDialog::currentChanged(const QModelIndex &index)
{
    QPushButton *okBtn = ui->buttonBox->button(QDialogButtonBox::Ok);
    okBtn->setEnabled(index.isValid());
    bool editable = index.data(SearchModel::EditableRole).toBool();
    ui->edit32Btn->setEnabled(index.isValid() && editable);
    ui->edit64Btn->setEnabled(index.isValid() && editable);
    ui->viewBtn->setEnabled(index.isValid());
}

void SolutionDialog::on_search_textChanged(const QString &/*search*/)
{
    ui->addBtn->setEnabled(false);
}

void SolutionDialog::on_addBtn_clicked()
{
    QString solution = ui->search->text().trimmed();
    if (Dialogs::confirm(tr("Are you sure you want to create solution \"%1\"?").arg(solution), this))
    {
        QUrlQuery data;
        data.addQueryItem("sname", solution);
        PostDialog pd(API_URL + "?c=add", data, this);
        if (pd.exec() == QDialog::Accepted)
        {
            searchExecute();
            QAbstractItemModel *model = ui->solutions->model();
            QModelIndexList s = model->match(model->index(0, 0), Qt::DisplayRole, solution, -1, Qt::MatchCaseSensitive);
            if (!s.isEmpty())
                ui->solutions->setCurrentIndex(s.first());
        }
    }
}

void SolutionDialog::on_edit32Btn_clicked()
{
    if (getSolution("32"))
        EditSolutionDialog("32", this).exec();
}

void SolutionDialog::on_edit64Btn_clicked()
{
    if (getSolution("64"))
        EditSolutionDialog("64", this).exec();
}

bool SolutionDialog::getSolution(const QString &arch)
{
    QString id = ui->solutions->currentIndex().data(SearchModel::SlugRole).toString();
    QString url = API_URL + "?c=get&slug=" + id + "&arch=" + arch;
    QString outFile = FS::temp().absoluteFilePath("solution");
    if (DownloadDialog(QStringList(url), outFile, this).exec() == QDialog::Accepted)
    {
        QSettings s(outFile, QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        auto bw = s.value("BWine").toString();
        if (bw.isEmpty())
        {
            Dialogs::error(tr("Incorrect solution file format!"), this);
            return false;
        }
    }
    return true;
}

void SolutionDialog::searchExecute()
{
    mCurrentPage = 1;
    getSearch();
}

void SolutionDialog::on_viewBtn_clicked()
{
    QString slug = ui->solutions->currentIndex().data(SearchModel::SlugRole).toString();
    QDesktopServices::openUrl(QUrl("http://wwizard.net/solutions/" + slug));
}
