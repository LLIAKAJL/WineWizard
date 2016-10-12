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
#include <QJsonArray>
#include <QDateTime>

#include "solutionmodel.h"
#include "utils.h"

const QString URL = "http://wwizard.net/api3/?c=solutions&os=%1&arch=%2&app=%3";
const QString SCRIPT_URL = "https://raw.githubusercontent.com/LLIAKAJL/WineWizard-Utils/master/%1%2.script";
const QString REPO_URL = "https://raw.githubusercontent.com/LLIAKAJL/WineWizard-Utils/master/%1%2.repo3";

SolutionModel::SolutionModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    connect(this,     &SolutionModel::finished, this, &SolutionModel::clear);
    connect(this,     &SolutionModel::error,    this, &SolutionModel::clear);
    connect(&mNetMgr, &NetManager::started,     this, &SolutionModel::started);
    connect(&mNetMgr, &NetManager::finished,    this, &SolutionModel::downloadFinished);
    connect(&mNetMgr, &NetManager::error,       this, &SolutionModel::error);
    connect(&mNetMgr, &NetManager::readyOutput, this, &SolutionModel::readyOutput);
    connect(&mNetMgr, &NetManager::readyError,  this, &SolutionModel::readyError);
}

QVariant SolutionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        switch (role)
        {
        case Qt::DisplayRole:
            switch (section)
            {
            case 0:
                return tr("Rating");
            case 1:
                return tr("Last Modified");
            case 2:
                return tr("Wine Versions");
            case 3:
                return tr("Packages");
            default:
                return QVariant();
            }
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

int SolutionModel::rowCount(const QModelIndex &) const
{
    return mItems.count();
}

int SolutionModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant SolutionModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case ScriptRole:
        return mScript;
    case PackagesRole:
        return QVariant::fromValue(mPackages);
    case WinesRole:
        return QVariant::fromValue(mWines);
    case ErrorsRole:
        return QVariant::fromValue(mErrors);
    case CatRole:
        return QVariant::fromValue(mCategories);
    }
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
            switch (index.column())
            {
            case 0:
                return mItems.at(index.row()).rating;
            case 1:
                return mItems.at(index.row()).date;
            case 2:
                return displayWines(index);
            case 3:
                return displayPackages(index);
            default:
                return QVariant();
            }
        case Qt::BackgroundRole:
            return ratingToColor(mItems.at(index.row()).rating);
        case Qt::ForegroundRole:
            return QColor(Qt::black);
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
            return data(index, Qt::DisplayRole);
        case IdRole:
            return mItems.at(index.row()).id;
        case BWRole:
            return mItems.at(index.row()).bw;
        case AWRole:
            return mItems.at(index.row()).aw;
        case BPRole:
            return QVariant::fromValue(mItems.at(index.row()).bp);
        case APRole:
            return QVariant::fromValue(mItems.at(index.row()).ap);
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool SolutionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case ResetRole:
        reset(value.value<QPair<QString, int>>());
        return true;
    }
    if (index.isValid())
    {
        switch (role)
        {
        case CloneRole:
            cloneItem(index);
            return true;
        default:
            return false;
        }
    }
    return false;
}

bool SolutionModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    if (index.isValid() && (data(index, BWRole) != roles.value(BWRole) ||
                            data(index, AWRole) != roles.value(AWRole) ||
                            data(index, BPRole) != roles.value(BPRole) ||
                            data(index, APRole) != roles.value(APRole)))
    {
        Item &item = mItems[index.row()];
        item.bw = roles.value(BWRole).toInt();
        item.aw = roles.value(AWRole).toInt();
        item.bp = roles.value(BPRole).value<SolutionModel::IntList>();
        item.ap = roles.value(APRole).value<SolutionModel::IntList>();
        mItems[index.row()].date = QDateTime::currentDateTime().toString("dd.MM.yyyy, hh:mm:ss");
        mItems[index.row()].rating = "-";
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

void SolutionModel::downloadFinished()
{
    beginResetModel();
    mScript = Utils::readFile(mScriptOut);
    QJsonArray data = QJsonDocument::fromJson(Utils::readFile(mOut)).array();
    for (QJsonArray::ConstIterator i = data.constBegin(); i != data.constEnd(); ++i)
    {
        QJsonObject o = (*i).toObject();
        Item item;
        item.id = o.value("i").toInt();
        item.rating = o.value("r").toString();
        item.date = o.value("d").toString();
        item.bw = o.value("bw").toInt();
        item.aw = o.value("aw").toInt();
        QJsonArray bpa = o.value("bp").toArray();
        for (QJsonArray::ConstIterator i = bpa.begin(); i != bpa.end(); ++i)
            item.bp.append((*i).toInt());
        QJsonArray apa = o.value("ap").toArray();
        for (QJsonArray::ConstIterator i = apa.begin(); i != apa.end(); ++i)
            item.ap.append((*i).toInt());
        mItems.append(item);
    }
    QJsonObject repo = Utils::readJson(mRepo);
    QJsonArray pa = repo.value("packages").toArray();
    for (QJsonArray::ConstIterator i = pa.constBegin(); i != pa.constEnd(); ++i)
    {
        QJsonObject o = (*i).toObject();
        Package p;
        p.name = o.value("n").toString();
        QJsonArray ca = o.value("c").toArray();
        for (QJsonArray::ConstIterator i = ca.constBegin(); i != ca.constEnd(); ++i)
            p.categories.append((*i).toInt());
        mPackages.insert(o.value("i").toInt(), p);
    }
    QJsonArray wa = repo.value("wines").toArray();
    for (QJsonArray::ConstIterator i = wa.constBegin(); i != wa.constEnd(); ++i)
    {
        QJsonObject o = (*i).toObject();
        Package w;
        w.name = o.value("n").toString();
        mWines.insert(o.value("i").toInt(), w);
    }
    QJsonArray ea = repo.value("errors").toArray();
    for (QJsonArray::ConstIterator i = ea.constBegin(); i != ea.constEnd(); ++i)
    {
        QJsonObject o = (*i).toObject();
        mErrors.insert(o.value("r").toString(), o.value("p").toInt());
    }
    QJsonArray ca = repo.value("categories").toArray();
    for (QJsonArray::ConstIterator i = ca.constBegin(); i != ca.constEnd(); ++i)
    {
        QJsonObject o = (*i).toObject();
        mCategories.insert(o.value("i").toInt(), o.value("n").toString());
    }
    endResetModel();
    emit finished();
}

QColor SolutionModel::ratingToColor(const QString &rating) const
{
    bool ok;
    int r = rating.toInt(&ok);
    if (!ok)
        return QColor(Qt::lightGray);
    if (r > 0)
        return Qt::green;
    else if (r < 0)
        return Qt::red;
    else
        return Qt::yellow;
}

void SolutionModel::reset(const QPair<QString, int> &args)
{
    clear();
    mNetMgr.terminate();
    beginResetModel();
    mItems.clear();
    mPackages.clear();
    mWines.clear();
    endResetModel();
    NetManager::RequestList rl;
    mScriptOut = Utils::temp().absoluteFilePath(Utils::genID());
    mOut = Utils::temp().absoluteFilePath(Utils::genID());
    mRepo = Utils::temp().absoluteFilePath(Utils::genID());
    rl << NetManager::makeGetRequest(SCRIPT_URL.arg(OS).arg(args.first), mScriptOut);
    rl << NetManager::makeGetRequest(URL.arg(OS).arg(args.first).arg(args.second), mOut);
    rl << NetManager::makeGetRequest(REPO_URL.arg(OS).arg(args.first), mRepo);
    mNetMgr.start(rl);
}

void SolutionModel::cloneItem(const QModelIndex &index)
{
    Item item;
    item.bw = index.data(BWRole).toInt();
    item.aw = index.data(AWRole).toInt();
    item.bp = index.data(BPRole).value<QList<int>>();
    item.ap = index.data(APRole).value<QList<int>>();
    item.rating = "-";
    item.date = QDateTime::currentDateTime().toString("dd.MM.yyyy, hh:mm:ss");
    int row = mItems.count();
    beginInsertRows(QModelIndex(), row, row);
    mItems.append(item);
    endInsertRows();
}

QString SolutionModel::displayWines(const QModelIndex &index) const
{
    const Item &i = mItems.at(index.row());
    return QString("%1 / %2").arg(mWines.value(i.bw).name).arg(mWines.value(i.aw).name);
}

QString SolutionModel::displayPackages(const QModelIndex &index) const
{
    const Item &i = mItems.at(index.row());
    QStringList bp;
    for (int id : i.bp)
        bp.append(mPackages.value(id).name);
    QStringList ap;
    for (int id : i.ap)
        ap.append(mPackages.value(id).name);
    QString bps = bp.isEmpty() ? "-" : bp.join(", ");
    QString aps = ap.isEmpty() ? "-" : ap.join(", ");
    return QString("%1 / %2").arg(bps).arg(aps);
}

void SolutionModel::clear()
{
    if (QFile::exists(mScriptOut))
        QFile::remove(mScriptOut);
    if (QFile::exists(mOut))
        QFile::remove(mOut);
    if (QFile::exists(mRepo))
        QFile::remove(mRepo);
}
