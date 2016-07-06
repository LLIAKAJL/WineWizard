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
            mList[index.row()].bw = value.toInt();
            emit dataChanged(index, index);
            return true;
        case AWRole:
            mList[index.row()].aw = value.toInt();
            emit dataChanged(index, index);
            return true;
        case BPRole:
            mList[index.row()].bp = value.value<IntList>();
            emit dataChanged(index, index);
            return true;
        case APRole:
            mList[index.row()].ap = value.value<IntList>();
            emit dataChanged(index, index);
            return true;
        case BSRole:
            mList[index.row()].bs = value.toString();
            emit dataChanged(index, index);
            return true;
        case ASRole:
            mList[index.row()].as = value.toString();
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

QIcon SolutionModel::ratingToIcon(const QModelIndex &index) const
{
    const Item &item = mList.at(index.row());
    if (item.rating > 0)
        return item.approved ? QIcon(":/icons/gstar") : QIcon(":/icons/gcircle");
    if (item.rating == 0)
        return item.approved ? QIcon(":/icons/ystar") : QIcon(":/icons/ycircle");
    return item.approved ? QIcon(":/icons/rstar") : QIcon(":/icons/rcircle");
}
