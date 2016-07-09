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
#include <QColor>

#include "searchmodel.h"
#include "filesystem.h"

SearchModel::SearchModel(QObject *parent) :
    QAbstractListModel(parent),
    mCount(0)
{
}

int SearchModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mList.count();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
            return mList.at(index.row()).name;
        case Qt::DecorationRole:
            return ratingToIcon(index);
        case Qt::ToolTipRole:
            return data(index);
        case IdRole:
            return mList.at(index.row()).id;
        case PrefixRole:
            return FS::hash(mList.at(index.row()).name);
        case SolutionRole:
            return mList.at(index.row()).solution;
        case ModelRole:
            return QVariant::fromValue(mList.at(index.row()).model);
        case SpecRole:
            {
                SolutionModel *model = mList.at(index.row()).model;
                if (model)
                {
                    QModelIndex si = model->index(mList.at(index.row()).solution);
                    return !si.data(SolutionModel::BSRole).toString().isEmpty() ||
                           !si.data(SolutionModel::ASRole).toString().isEmpty();
                }
            }
            return mList.at(index.row()).spec;
        case ApprovedRole:
            {
                SolutionModel *model = mList.at(index.row()).model;
                if (model)
                {
                    QModelIndex si = model->index(mList.at(index.row()).solution);
                    return si.data(SolutionModel::ApprovedRole).toBool();
                }
            }
            return mList.at(index.row()).approved;
        default:
            return QVariant();
        }
    }
    switch (role)
    {
    case CountRole:
        return mCount;
    case ExistsRole:
        return mExists;
    default:
        return QVariant();
    }
    return QVariant();
}

bool SearchModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        switch (role)
        {
        case SolutionRole:
            mList[index.row()].solution = value.toInt();
            emit dataChanged(index, index);
            return true;
        case ModelRole:
            if (!mList.at(index.row()).model)
                mList[index.row()].model = new SolutionModel(data(index, PrefixRole).toString(), this);
            return true;
        default:
            return false;
        }
    }
    if (role == ResetRole)
    {
        beginResetModel();
        for (Item a : mList)
            if (a.model)
                a.model->deleteLater();
        mList.clear();
        if (FS::temp().exists("search"))
        {
            QByteArray data = FS::readFile(FS::temp().absoluteFilePath("search"));
            QJsonObject jo = QJsonDocument::fromJson(data).object();
            mCount = jo.value("count").toInt();
            mExists = jo.value("exists").toBool();
            QJsonArray apps = jo.value("apps").toArray();
            for (QJsonArray::Iterator i = apps.begin(); i != apps.end(); ++i)
            {
                QJsonObject jo = (*i).toObject();
                Item a;
                a.id = jo.value("i").toInt();
                a.name = jo.value("n").toString();
                a.approved = jo.value("a").toBool();
                a.rating = jo.value("r").toInt();
                a.spec = jo.value("s").toBool();
                a.prefix = FS::hash(a.name);
                a.solution = 0;
                a.model = nullptr;
                mList.append(a);
            }
        }
        endResetModel();
        return true;
    }
    return false;
}

QIcon SearchModel::ratingToIcon(const QModelIndex &index) const
{
    const Item &item = mList.at(index.row());
    if (item.model)
        return item.model->index(item.solution).data(Qt::DecorationRole).value<QIcon>();
    bool spec = item.spec;
    if (item.rating > 0)
        return spec ? QIcon(":/icons/ge") : QIcon(item.approved ? ":/icons/gs" : ":/icons/gc");
    else if (item.rating == 0)
        return spec ? QIcon(":/icons/ye") : QIcon(item.approved ? ":/icons/ys" : ":/icons/yc");
    else
        return spec ? QIcon(":/icons/re") : QIcon(item.approved ? ":/icons/rs" : ":/icons/rc");
}
