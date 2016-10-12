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
#include <QDir>

#include "ui_installpage.h"
#include "solutionmodel.h"
#include "setupwizard.h"
#include "installpage.h"
#include "mainmodel.h"
#include "appmodel.h"

InstallPage::InstallPage(QAbstractItemModel *model, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallPage),
    mModel(static_cast<QSortFilterProxyModel *>(model))
{
    ui->setupUi(this);
    registerField("out", this, "out");
    registerField("err", this, "err");
    connect(mModel, &QAbstractItemModel::dataChanged, this, &InstallPage::dataChanged);
    QSortFilterProxyModel *sm = static_cast<QSortFilterProxyModel *>(mModel);
    MainModel *m = static_cast<MainModel *>(sm->sourceModel());
    connect(m, &MainModel::readyOutput, this, &InstallPage::readyOutput);
    connect(m, &MainModel::readyError, this, &InstallPage::readyError);
}

InstallPage::~InstallPage()
{
    delete ui;
}

void InstallPage::initializePage()
{
    mOut.clear();
    mErr.clear();
    ui->output->clear();
    if (field("debug").toBool())
    {
        QModelIndex s = field("shortcut").toModelIndex();
        mModel->setData(s, true, MainModel::DebugRole);
        wizard()->showMinimized();
    }
    else
    {
        int appID = field("app").toModelIndex().data(AppModel::IdRole).toInt();
        QModelIndex s = field("solution").toModelIndex();
        QString path = field("app").toModelIndex().data(AppModel::PathRole).toString();
        QString name = field("app").toModelIndex().data().toString();
        QByteArray script = s.data(SolutionModel::ScriptRole).toByteArray();
        QString exe = field("exe").toString();
        QString args = field("args").toString();
        QString workDir = field("workDir").toString();
        SolutionModel::PackageList wines = s.data(SolutionModel::WinesRole).value<SolutionModel::PackageList>();
        SolutionModel::PackageList packages = s.data(SolutionModel::PackagesRole).value<SolutionModel::PackageList>();
        QString bw = wines.value(s.data(SolutionModel::BWRole).toInt()).name;
        QString aw = wines.value(s.data(SolutionModel::AWRole).toInt()).name;
        QStringList bp;
        for (int id : s.data(SolutionModel::BPRole).value<SolutionModel::IntList>())
            bp.append(packages.value(id).name);
        QStringList ap;
        for (int id : s.data(SolutionModel::APRole).value<SolutionModel::IntList>())
            ap.append(packages.value(id).name);
        MainModel *m = static_cast<MainModel *>(mModel->sourceModel());
        m->install(script, name, appID, path, exe, args, workDir, bw, aw, bp, ap);
    }
}

bool InstallPage::isComplete() const
{
    QString path = field("app").toModelIndex().data(AppModel::PathRole).toString();
    QModelIndex i = mModel->index(0, 0);
    QModelIndexList l = mModel->match(i, MainModel::PathRole, path, -1, Qt::MatchFixedString);
    return l.isEmpty() || !l.first().data(MainModel::RunningRole).toBool();
}

int InstallPage::nextId() const
{
    return SetupWizard::PageDebug;
}

void InstallPage::dataChanged(const QModelIndex &index)
{
    if (index.data(MainModel::PathRole) == field("app").toModelIndex().data(AppModel::PathRole))
    {
        if (!index.data(MainModel::RunningRole).toBool())
        {
            emit completeChanged();
            wizard()->next();
            wizard()->showNormal();
        }
    }
}

void InstallPage::readyOutput(const QString &path, const QString &data)
{
    if (path == field("app").toModelIndex().data(AppModel::PathRole))
    {
        mOut += data;
        appendOut(data);
    }
}

void InstallPage::readyError(const QString &path, const QString &data)
{
    if (path == field("app").toModelIndex().data(AppModel::PathRole))
    {
        mErr += data;
        appendOut(QString(R"(<span style=" color:#ff0000;">%1</span>)").arg(data));
    }
}

void InstallPage::appendOut(const QString &text)
{
    ui->output->moveCursor(QTextCursor::End);
    ui->output->insertHtml(QString(text).replace('\n', "<br>"));
}

const QString &InstallPage::out() const
{
    return mOut;
}

const QString &InstallPage::err() const
{
    return mErr;
}
