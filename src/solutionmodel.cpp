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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>

#include "solutionmodel.h"
#include "filesystem.h"
#include "repository.h"

SolutionModel::SolutionModel(const QString &prefix, QObject *parent) :
    QAbstractListModel(parent)
{
    QByteArray data = FS::readFile(FS::temp().absoluteFilePath(prefix));
    QJsonArray list = QJsonDocument::fromJson(data).array();
    for (QJsonArray::Iterator i = list.begin(); i != list.end(); ++i)
    {
        QJsonObject itemData = (*i).toObject();
        Item item;
        item.date = itemData.value("d").toString();
        if (item.date.isEmpty())
            item.date = tr("New Solution");
        item.rating = itemData.value("r").toInt();
        QJsonArray bpa = itemData.value("bp").toArray();
        for (QJsonArray::ConstIterator i = bpa.begin(); i != bpa.end(); ++i)
            item.bp.append((*i).toInt());
        QJsonArray apa = itemData.value("ap").toArray();
        for (QJsonArray::ConstIterator i = apa.begin(); i != apa.end(); ++i)
            item.ap.append((*i).toInt());
        item.bw = itemData.value("bw").toInt();
        item.aw = itemData.value("aw").toInt();
        item.bs = itemData.value("bs").toString();
        item.as = itemData.value("as").toString();
        item.approved = itemData.value("a").toBool();
        item.id = itemData.value("i").toInt();
        mList.append(item);
    }
}

int SolutionModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mList.count();
}

QVariant SolutionModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
            {
                const Item &item = mList.at(index.row());
                return tr("%1 /// %2").arg(item.rating).arg(item.date);
            }
        case Qt::DecorationRole:
            return ratingToIcon(index);
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case IdRole:
            return mList.at(index.row()).id;
        case BWRole:
            return mList.at(index.row()).bw;
        case AWRole:
            return mList.at(index.row()).aw;
        case BPRole:
            return QVariant::fromValue<IntList>(mList.at(index.row()).bp);
        case APRole:
            return QVariant::fromValue<IntList>(mList.at(index.row()).ap);
        case BSRole:
            return mList.at(index.row()).bs;
        case ASRole:
            return mList.at(index.row()).as;
        case ApprovedRole:
            return mList.at(index.row()).approved;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool SolutionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        switch (role)
        {
        case BWRole:
            if (mList.at(index.row()).bw != value.toInt())
            {
                mList[index.row()].bw = value.toInt();
                resetItem(index);
                emit dataChanged(index, index);
                return true;
            }
            return false;
        case AWRole:
            if (mList.at(index.row()).aw != value.toInt())
            {
                mList[index.row()].aw = value.toInt();
                resetItem(index);
                emit dataChanged(index, index);
                return true;
            }
            return false;
        case BPRole:
            if (mList.at(index.row()).bp != value.value<IntList>())
            {
                mList[index.row()].bp = value.value<IntList>();
                resetItem(index);
                emit dataChanged(index, index);
                return true;
            }
            return false;
        case APRole:
            if (mList.at(index.row()).ap != value.value<IntList>())
            {
                mList[index.row()].ap = value.value<IntList>();
                resetItem(index);
                emit dataChanged(index, index);
                return true;
            }
            return false;
        case BSRole:
            if (mList.at(index.row()).bs != value.toString())
            {
                mList[index.row()].bs = value.toString();
                resetItem(index);
                emit dataChanged(index, index);
                return true;
            }
            return false;
        case ASRole:
            if (mList.at(index.row()).as != value.toString())
            {
                mList[index.row()].as = value.toString();
                resetItem(index);
                emit dataChanged(index, index);
                return true;
            }
            return false;
        }
    }
    return false;
}

QIcon SolutionModel::ratingToIcon(const QModelIndex &index) const
{
    const Item &item = mList.at(index.row());
    bool spec = !item.bs.isEmpty() || !item.as.isEmpty();
    if (item.rating > 0)
        return spec ? QIcon(":/icons/ge") : QIcon(item.approved ? ":/icons/gs" : ":/icons/gc");
    else if (item.rating == 0)
        return spec ? QIcon(":/icons/ye") : QIcon(item.approved ? ":/icons/ys" : ":/icons/yc");
    else
        return spec ? QIcon(":/icons/re") : QIcon(item.approved ? ":/icons/rs" : ":/icons/rc");
}

void SolutionModel::resetItem(const QModelIndex &index)
{
    mList[index.row()].date = tr("New Solution");
    mList[index.row()].rating = 0;
    mList[index.row()].approved = false;
}
