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

#include <QDesktopServices>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QDir>

#include "solutionmodel.h"
#include "ui_finalpage.h"
#include "netmanager.h"
#include "finalpage.h"
#include "appmodel.h"
#include "utils.h"

const QString URL = "http://wwizard.net/api3";

FinalPage::FinalPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::FinalPage),
    mFinished(false)
{
    ui->setupUi(this);
    connect(&mNetMgr, &NetManager::readyOutput, ui->wait,  &WaitForm::readyOutput);
    connect(&mNetMgr, &NetManager::readyError,  ui->wait,  &WaitForm::readyError);
    connect(&mNetMgr, &NetManager::error,       this,      &FinalPage::finished);
    connect(&mNetMgr, &NetManager::finished,    this,      &FinalPage::finished);
    connect(&mNetMgr, &NetManager::started,     ui->wait,  &WaitForm::start);
}

FinalPage::~FinalPage()
{
    delete ui;
}

void FinalPage::initializePage()
{
    QJsonObject data;
    data.insert("c", "fork");
    data.insert("os", OS);
    data.insert("arch", field("arch").toString());
    data.insert("app", field("app").toModelIndex().data(AppModel::IdRole).toInt());
    QModelIndex s = field("solution").toModelIndex();
    data.insert("id", s.data(SolutionModel::IdRole).toInt());
    data.insert("bw", s.data(SolutionModel::BWRole).toInt());
    data.insert("aw", s.data(SolutionModel::AWRole).toInt());
    QJsonArray bp;
    for (int id : s.data(SolutionModel::BPRole).value<SolutionModel::IntList>())
        bp.append(id);
    if (!bp.isEmpty())
        data.insert("bp", bp);
    QJsonArray ap;
    for (int id : s.data(SolutionModel::APRole).value<SolutionModel::IntList>())
        ap.append(id);
    if (!ap.isEmpty())
        data.insert("ap", ap);
    mNetMgr.start(NetManager::RequestList() << NetManager::makePostRequest(URL, data));
}

bool FinalPage::isComplete() const
{
    return mFinished;
}

int FinalPage::nextId() const
{
    return -1;
}

void FinalPage::finished()
{
    mFinished = true;
    emit completeChanged();
    wizard()->accept();
}
