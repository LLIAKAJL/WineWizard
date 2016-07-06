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

#include <QMimeData>

#include "packagemodel.h"
#include "filesystem.h"

PackageModel::PackageModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int PackageModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mList.count();
}

QVariant PackageModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case IdRole:
            return mList.at(index.row()).id;
        case Qt::DisplayRole:
            return mList.at(index.row()).name;
        case CategoryRole:
            return QVariant::fromValue(mList.at(index.row()).categories);
        case Qt::ToolTipRole:
            return mList.at(index.row()).tooltip;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        }
    }
    return QVariant();
}

bool PackageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        switch (role)
        {
        case IdRole:
            mList[index.row()].id = value.toInt();
            emit dataChanged(index, index);
            return true;
        case Qt::EditRole:
            mList[index.row()].name = value.toString();
            emit dataChanged(index, index);
            return true;
        case CategoryRole:
            mList[index.row()].categories.append(value.value<IntList>());
            emit dataChanged(index, index);
            return true;
        case Qt::ToolTipRole:
            mList[index.row()].tooltip = value.toString();
            emit dataChanged(index, index);
            return true;
        default:
            return false;
        }
    }
    if (role == ResetRole)
    {
        beginResetModel();
        mList = value.value<Repository::PackageList>();
        endResetModel();
        return true;
    }
    return false;
}

Qt::DropActions PackageModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::ItemFlags PackageModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

bool PackageModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
    {
        Repository::Package p;
        p.id = -1;
        p.categories = IntList() << -1;
        mList.insert(row, p);
    }
    endInsertRows();
    return true;
}

bool PackageModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        mList.takeAt(row);
    endRemoveRows();
    return true;
}

QStringList PackageModel::mimeTypes() const
{
    return QStringList("application/vnd.text.list");
}

QMimeData *PackageModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    foreach (QModelIndex index, indexes)
    {
        if (index.isValid())
        {
            stream << data(index, IdRole).toInt();
            stream << data(index).toString();
            stream << data(index, CategoryRole).value<IntList>();
            stream << data(index, Qt::ToolTipRole).toString();
        }
    }
    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

bool PackageModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasFormat("application/vnd.text.list"))
        return false;
    if (column > 0)
        return false;
    int beginRow;
    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());
    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    int rows = 0;
    Repository::PackageList tmpList;
    while (!stream.atEnd())
    {
        Repository::Package p;
        stream >> p.id;
        stream >> p.name;
        stream >> p.categories;
        stream >> p.tooltip;
        tmpList.append(p);
        ++rows;
    }
    insertRows(beginRow, rows, QModelIndex());
    for (const Repository::Package &package : tmpList)
    {
        QModelIndex idx = index(beginRow, 0, QModelIndex());
        setData(idx, package.id, IdRole);
        setData(idx, package.name);
        setData(idx, QVariant::fromValue(package.categories), CategoryRole);
        setData(idx, package.tooltip, Qt::ToolTipRole);
        ++beginRow;
    }
    return true;
}
