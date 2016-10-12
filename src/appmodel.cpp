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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "netmanager.h"
#include "appmodel.h"
#include "utils.h"

const QString URL = "http://wwizard.net/api3/?c=search&se=%1&pn=%2";
const QString ADD_URL = "http://wwizard.net/api3";
const QString ALTER_STYLE = "<p style='color:#dd4b39'>%1</p>";
const QString LINK_STYLE = "<i><a href='%1'>%1</a></i>";

AppModel::AppModel(QObject *parent) :
    QAbstractListModel(parent),
    mPageCount(1)
{
    connect(&mNetMgr, &NetManager::started,     this, &AppModel::started);
    connect(&mNetMgr, &NetManager::error,       this, &AppModel::error);
    connect(&mNetMgr, &NetManager::error,       this, &AppModel::clear);
    connect(&mNetMgr, &NetManager::readyOutput, this, &AppModel::readyOutput);
    connect(&mNetMgr, &NetManager::readyError,  this, &AppModel::readyError);
}

int AppModel::rowCount(const QModelIndex &) const
{
    return mItems.count();
}

QVariant AppModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case PageCountRole:
        return mPageCount;
    case AlterRole:
        return mAlter;
    }
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return mItems.at(index.row()).name;
        case IdRole:
            return mItems.at(index.row()).id;
        case PathRole:
            return mItems.at(index.row()).path;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool AppModel::setData(const QModelIndex &, const QVariant &value, int role)
{
    switch (role)
    {
    case ResetRole:
        reset(value.value<QPair<QString, int>>());
        return true;
    case NewRole:
        newApp(value.toString());
        return true;
    default:
        return false;
    }
}

void AppModel::downloadFinished()
{
    disconnect(&mNetMgr, &NetManager::finished, this, &AppModel::downloadFinished);
    beginResetModel();
    QJsonObject o = Utils::readJson(mOut);
    clear();
    mPageCount = o.value("pcount").toInt();
    QJsonArray data = o.value("data").toArray();
    for (QJsonArray::ConstIterator i = data.constBegin(); i != data.constEnd(); ++i)
    {
        QJsonObject o = (*i).toObject();
        Item item;
        item.id = o.value("i").toInt();
        item.name = o.value("n").toString();
        item.path = Utils::data().absoluteFilePath(Utils::genID());
        mItems.append(item);
    }
    QJsonArray alter = o.value("alter").toArray();
    QStringList alterList;
    QString search = mSearch;
    for (QJsonArray::ConstIterator i = alter.constBegin(); i != alter.constEnd(); ++i)
    {
        QString str = (*i).toString();
        if (str != search)
            alterList.append(LINK_STYLE.arg(str));
    }
    if (!alterList.isEmpty())
        mAlter = ALTER_STYLE.arg(tr("Did you mean: %1").arg(alterList.join(", ")));
    endResetModel();
    emit finished();
}

void AppModel::addFinished()
{
    disconnect(&mNetMgr, &NetManager::finished, this, &AppModel::addFinished);
    beginResetModel();
    QJsonObject o = Utils::readJson(mOut);
    clear();
    mPageCount = 1;
    Item item;
    item.id = o.value("i").toInt();
    item.name = o.value("n").toString();
    item.path = Utils::data().absoluteFilePath(Utils::genID());
    mItems.append(item);
    endResetModel();
    emit finished();
}

void AppModel::reset(const QPair<QString, int> &args)
{
    mNetMgr.terminate();
    clear();
    beginResetModel();
    mItems.clear();
    mAlter.clear();
    mPageCount = 1;
    endResetModel();
    mSearch = args.first;
    mOut = Utils::temp().absoluteFilePath(Utils::genID());
    NetManager::Request request = NetManager::makeGetRequest(URL.arg(mSearch).arg(args.second), mOut);
    connect(&mNetMgr, &NetManager::finished, this, &AppModel::downloadFinished);
    mNetMgr.start(NetManager::RequestList() << request);
}

void AppModel::newApp(const QString &name)
{
    mNetMgr.terminate();
    clear();
    beginResetModel();
    mItems.clear();
    mPageCount = 1;
    mAlter.clear();
    endResetModel();
    mOut = Utils::temp().absoluteFilePath(Utils::genID());
    QJsonObject data;
    data.insert("c", "add");
    data.insert("name", name);
    NetManager::Request request = NetManager::makePostRequest(ADD_URL, data, mOut);
    connect(&mNetMgr, &NetManager::finished, this, &AppModel::addFinished);
    mNetMgr.start(NetManager::RequestList() << request);
}

void AppModel::clear()
{
    if (QFile::exists(mOut))
        QFile::remove(mOut);
}
