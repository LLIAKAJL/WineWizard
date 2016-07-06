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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

#include "ui_solutionpage.h"
#include "solutiondialog.h"
#include "installwizard.h"
#include "solutionpage.h"
#include "searchmodel.h"
#include "filesystem.h"
#include "mainwindow.h"
#include "executor.h"
#include "dialogs.h"

const int POSTS_PER_PAGE = 50;

const QString SOLUTIONS_QUERY = "?c=sol_list&os=%1&arch=%2&app=%3";
const QString DOWNLOAD_URL = "http://wwizard.net/download/";
const QString API_URL = "http://localhost/api2/";//"http://wwizard.net/api2/";

const QString UPDATE_URL = "https://raw.githubusercontent.com/LLIAKAJL/WineWizard-Utils/master/update.";
const QString REPO_URL = "https://raw.githubusercontent.com/LLIAKAJL/WineWizard-Utils/master/%1%2.repo";

SolutionPage::SolutionPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::SolutionPage)
{
    ui->setupUi(this);
    ui->leftBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->rightBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    SearchModel *model = new SearchModel(this);
    ui->apps->setModel(model);
    connect(ui->apps->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &SolutionPage::completeChanged);
    registerField("app*", this, "app");
    registerField("repository", this, "repository");
    registerField("sha32", this, "sha32");
    registerField("sha64", this, "sha64");
}

SolutionPage::~SolutionPage()
{
    delete ui;
}

void SolutionPage::initializePage()
{
    ui->out->clear();
    for (const QFileInfo &f : FS::temp().entryInfoList(QDir::Files))
        QFile::remove(f.absoluteFilePath());
    ui->apps->model()->setData(QModelIndex(), QVariant(), SearchModel::ResetRole);
    mPage = 1;
    mPageCount = 1;
    ui->page->setText(tr("%1 from %2").arg(mPage).arg(mPageCount));
    ui->pages->setCurrentIndex(0);
    Executor *e = new Executor(QString(), this);
    connect(e, &Executor::finished, this, &SolutionPage::updateFinished);
    connect(e, &Executor::readyOutput, this, &SolutionPage::readyOutput);
    connect(e, &Executor::readyError, this, &SolutionPage::readyError);
    QString url = UPDATE_URL + OS;
    QString path = FS::temp().absoluteFilePath("update");
    QString script = QString(FS::readFile(":/download")).arg(url).arg(path);
    e->start(script);
}

bool SolutionPage::validatePage()
{
    if (!property("selected").toBool())
    {
        QModelIndex index = ui->apps->currentIndex();
        QString prefix = index.data(SearchModel::PrefixRole).toString();
        if (!index.data(SearchModel::ApprovedRole).toBool() && index.data(SearchModel::SpecRole).toBool())
            if (!Dialogs::confirm(tr("Are you sure you want to use solution with additional scripts?"), wizard()))
                return false;
        if (FS::prefix(prefix).exists())
        {
            QString name = index.data().toString();
            if (!Dialogs::confirm(tr("Application \"%1\" is already installed! " \
                                     "Are you sure you want to reinstall it?").arg(name), wizard()))
                return false;
        }
        getSolutions(true);
        return false;
    }
    return true;
}

bool SolutionPage::isComplete() const
{
    return ui->apps->currentIndex().isValid() && !property("busy").toBool();
}

int SolutionPage::nextId() const
{
    return InstallWizard::PageInstall;
}

void SolutionPage::repoFinished(int code)
{
    if (code != 0)
        Dialogs::error(QObject::tr("Download failed!"));
    else
    {
        QString arch = field("arch").toString();
        QString path = FS::cache().absoluteFilePath(OS + arch + ".repo");
        QJsonParseError err;
        QJsonDocument::fromJson(FS::readFile(path), &err);
        if (err.error != QJsonParseError::NoError)
        {
            Dialogs::error(QObject::tr("Incorrect file format!"));
            wizard()->setProperty("finish", true);
            wizard()->reject();
        }
        else
        {
            mRepository.setArch(arch);
            clearRepository();
            getSearch();
        }
    }
}

void SolutionPage::searchFinished(int code)
{
    if (code != 0)
        Dialogs::error(QObject::tr("Download failed!"));
    else
    {
        QAbstractItemModel *model = ui->apps->model();
        model->setData(QModelIndex(), QVariant(), SearchModel::ResetRole);
        int count = model->data(QModelIndex(), SearchModel::CountRole).toInt();
        mPageCount = count / POSTS_PER_PAGE + 1;
        bool exists = model->data(QModelIndex(), SearchModel::ExistsRole).toBool();
        ui->addBtn->setVisible(!exists);
        ui->notFoundLbl->setVisible(!exists);
        QString app = field("search").toString();
        ui->notFoundLbl->setText(tr("Application \"%1\" not found.").arg(app));
        ui->pages->setCurrentIndex(1);
        setTitle(tr("Select solution"));
        setSubTitle(tr("Select a solution for your application."));
        if (count > 0)
        {
            QModelIndexList s = model->match(model->index(0, 0), Qt::DisplayRole,
                                             app, -1, Qt::MatchFixedString);
            if (!s.isEmpty())
                ui->apps->setCurrentIndex(s.first());
            else
            {
                ui->apps->setCurrentIndex(model->index(0, 0));
                ui->apps->setFocus();
            }
        }
        ui->solutionsBtn->setEnabled(count > 0);
        ui->leftBtn->setEnabled(mPage > 1);
        ui->rightBtn->setEnabled(mPage < mPageCount);
        ui->page->setText(tr("%1 from %2").arg(mPage).arg(mPageCount));
        emit completeChanged();
    }
}

void SolutionPage::solutonsFinished(int code)
{
    if (code != 0)
        Dialogs::error(QObject::tr("Download failed!"));
    else
    {
        setProperty("busy", false);
        emit completeChanged();
        QModelIndex index = ui->apps->currentIndex();
        QString prefix = index.data(SearchModel::PrefixRole).toString();
        for (Executor *e : Executor::instances())
            if (e->prefix() == prefix)
                e->deleteLater();
        QAbstractItemModel *m = ui->apps->model();
        m->setData(index, QVariant(), SearchModel::ModelRole);
        ui->pages->setCurrentIndex(1);
        setProperty("selected", true);
        wizard()->next();
        MainWindow::update();
    }
}

void SolutionPage::solutonsEditFinished(int code)
{
    emit completeChanged();
    if (code != 0)
        Dialogs::error(QObject::tr("Download failed!"));
    else
    {
        setProperty("busy", false);
        emit completeChanged();
        QModelIndex index = ui->apps->currentIndex();
        ui->apps->model()->setData(index, QVariant(), SearchModel::ModelRole);
        ui->pages->setCurrentIndex(1);
        setTitle(tr("Select solution"));
        setSubTitle(tr("Select a solution for your application."));
        SolutionDialog ed(wizard(), wizard());
        ed.exec();
    }
}

void SolutionPage::addFinished(int code)
{
    if (code != 0)
        Dialogs::error(QObject::tr("Upload failed!"));
    else
        getSearch();
}

void SolutionPage::getSearch(int page)
{
    ui->out->clear();
    setTitle(tr("Data updating"));
    setSubTitle(tr("This may take several minutes. Please wait..."));
    ui->pages->setCurrentIndex(0);
    Executor *e = new Executor(QString(), this);
    connect(e, &Executor::finished, this, &SolutionPage::searchFinished);
    connect(e, &Executor::readyOutput, this, &SolutionPage::readyOutput);
    connect(e, &Executor::readyError, this, &SolutionPage::readyError);
    QString arch = field("arch").toString();
    QString searchEncode = QUrl::toPercentEncoding(field("search").toString().trimmed());
    QString url = API_URL + QString("?c=search&se=%1&pagenum=%2&os=%3&arch=%4").
                                    arg(searchEncode).arg(page).arg(OS).arg(arch);
    QString path = FS::temp().absoluteFilePath("search");
    QString script = QString(FS::readFile(":/download")).arg(url).arg(path);
    e->start(script);
}

Repository *SolutionPage::repository()
{
    return &mRepository;
}

QModelIndex SolutionPage::app() const
{
    return ui->apps->currentIndex();
}

void SolutionPage::on_addBtn_clicked()
{
    QString app = field("search").toString().trimmed();
    QChar first = app.at(0);
    if (first.isLetter() && first.isLower())
    {
        QStringList words = app.split(QChar::Space, QString::SkipEmptyParts);
        for (int i = words.count() - 1; i >= 0; --i)
            words[i][0] = words.at(i).at(0).toUpper();
        app = words.join(QChar::Space);
    }
    if (Dialogs::question(tr("Are you sure you want to add application \"%1\"?").arg(app), wizard()))
    {
        ui->out->clear();
        ui->pages->setCurrentIndex(0);
        Executor *e = new Executor(QString(), this);
        connect(e, &Executor::finished, this, &SolutionPage::addFinished);
        connect(e, &Executor::readyOutput, this, &SolutionPage::readyOutput);
        connect(e, &Executor::readyError, this, &SolutionPage::readyError);
        QJsonObject jo;
        jo.insert("c", "add");
        jo.insert("name", app);
        QString data = QJsonDocument(jo).toJson();
        QString path = FS::temp().absoluteFilePath("response");
        QString script = QString(FS::readFile(":/post")).arg(API_URL).arg(data).arg(path);
        e->start(script);
    }
}

void SolutionPage::on_leftBtn_clicked()
{
    --mPage;
    ui->page->setText(tr("%1 from %2").arg(mPage).arg(mPageCount));
    getSearch(mPage);
}

void SolutionPage::on_rightBtn_clicked()
{
    ++mPage;
    ui->page->setText(tr("%1 from %2").arg(mPage).arg(mPageCount));
    getSearch(mPage);
}

void SolutionPage::clearRepository() const
{
    QDir cache = FS::cache();
    QStringList files;
    for (Repository::File f : mRepository.files())
        files.append(f.name);
    for (const QFileInfo &f : cache.entryInfoList(QDir::AllEntries |
                                                  QDir::NoDotAndDotDot |
                                                  QDir::Hidden))
    {
        if (!files.contains(f.fileName()) && f.suffix() != "repo")
        {
            if (f.isDir())
                QDir(f.absoluteFilePath()).removeRecursively();
            else
                QFile::remove(f.absoluteFilePath());
        }
    }
}

QString SolutionPage::sha() const
{
    return mSha;
}

void SolutionPage::readyOutput(const QByteArray &text)
{
    appendOut(text);
}

void SolutionPage::readyError(const QByteArray &text)
{
    appendOut(QString(R"(<span style=" color:#ff0000;">%1</span>)").arg(QString(text)));
}

void SolutionPage::appendOut(const QString &text)
{
    ui->out->moveCursor(QTextCursor::End);
    ui->out->insertHtml(QString(text).replace('\n', "<br>"));
}

void SolutionPage::updateFinished(int code)
{
    if (code != 0)
        Dialogs::error(QObject::tr("Download failed!"));
    else
    {
        QString outPath = FS::temp().absoluteFilePath("update");
        QJsonParseError err;
        QJsonDocument jd = QJsonDocument::fromJson(FS::readFile(outPath), &err);
        if (err.error != QJsonParseError::NoError)
        {
            Dialogs::error(QObject::tr("Incorrect file format!"));
            wizard()->setProperty("finish", true);
            wizard()->reject();
        }
        else
        {
            QJsonObject jo = jd.object();
            QString appVer = QApplication::applicationVersion();
            QString repoVer = jo.value("version").toString();
            if (appVer != repoVer)
            {
                Dialogs::error(tr("Please install a newer version of Wine Wizard.\n" \
                                  "\nThe current version is %1.\n" \
                                  "The required version is %2.").arg(appVer).arg(repoVer), this);
                QDesktopServices::openUrl(QUrl(DOWNLOAD_URL));
                wizard()->setProperty("finish", true);
                wizard()->reject();
            }
            else
            {
                mSha = jo.value("32").toString();
                ui->out->clear();
                QString arch = field("arch").toString();
                QString path = FS::cache().absoluteFilePath(OS + arch + ".repo");
                if (!QFile::exists(path) || FS::hash(FS::readFile(path)) != mSha)
                {
                    Executor *e = new Executor(QString(), this);
                    connect(e, &Executor::finished, this, &SolutionPage::repoFinished);
                    connect(e, &Executor::readyOutput, this, &SolutionPage::readyOutput);
                    connect(e, &Executor::readyError, this, &SolutionPage::readyError);
                    QString url = REPO_URL.arg(OS).arg(arch);
                    QString script = QString(FS::readFile(":/download")).arg(url).arg(path);
                    e->start(script);
                }
                else
                    repoFinished(0);
            }
        }
    }
}

void SolutionPage::getSolutions(bool next)
{
    setProperty("busy", true);
    emit completeChanged();
    setProperty("selected", false);
    QModelIndex index = ui->apps->currentIndex();
    int id = index.data(SearchModel::IdRole).toInt();
    QString prefix = index.data(SearchModel::PrefixRole).toString();
    if (FS::temp().exists(prefix))
    {
        if (next)
            solutonsFinished(0);
        else
            solutonsEditFinished(0);
    }
    else
    {
        ui->out->clear();
        ui->pages->setCurrentIndex(0);
        setTitle(tr("Data updating"));
        setSubTitle(tr("This may take several minutes. Please wait..."));

        Executor *e = new Executor(QString(), this);
        if (next)
            connect(e, &Executor::finished, this, &SolutionPage::solutonsFinished);
        else
            connect(e, &Executor::finished, this, &SolutionPage::solutonsEditFinished);
        connect(e, &Executor::readyOutput, this, &SolutionPage::readyOutput);
        connect(e, &Executor::readyError, this, &SolutionPage::readyError);
        QString arch = field("arch").toString();
        QString url = API_URL + SOLUTIONS_QUERY.arg(OS).arg(arch).arg(id);
        QString path = FS::temp().absoluteFilePath(prefix);
        QString script = QString(FS::readFile(":/download")).arg(url).arg(path);
        e->start(script);
    }
}

void SolutionPage::on_solutionsBtn_clicked()
{
    QModelIndex index = ui->apps->currentIndex();
    if (index.isValid())
        getSolutions(false);
}

void SolutionPage::on_apps_doubleClicked(const QModelIndex &index)
{
    if (index.isValid())
        getSolutions(false);
}
