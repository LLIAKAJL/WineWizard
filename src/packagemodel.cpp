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

#include <QDataStream>
#include <QMimeData>

#include "packagemodel.h"

PackageModel::PackageModel(const SolutionModel::IntList &list,
                           const SolutionModel::PackageList &all, QObject *parent) :
    QAbstractListModel(parent)
{
    for (int id : list)
    {
        Item item;
        item.id = id;
        item.package = all.value(id);
        mItems.append(item);
    }
}

int PackageModel::rowCount(const QModelIndex &) const
{
    return mItems.count();
}

QVariant PackageModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return mItems.at(index.row()).package.name;
        case CatRole:
            return QVariant::fromValue(mItems.at(index.row()).package.categories);
        case IdRole:
            return mItems.at(index.row()).id;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        default:
            return QVariant();
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
        case Qt::EditRole:
            mItems[index.row()].package.name = value.toString();
            emit dataChanged(index, index);
            return true;
        case CatRole:
            mItems[index.row()].package.categories = value.value<SolutionModel::IntList>();
            emit dataChanged(index, index);
            return true;
        case IdRole:
            mItems[index.row()].id = value.toInt();
            emit dataChanged(index, index);
            return true;
        default:
            return false;
        }
    }
    return false;
}

Qt::DropActions PackageModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::ItemFlags PackageModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsDropEnabled;
}

bool PackageModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        mItems.insert(row, Item{ -1, { QString(), SolutionModel::IntList() } });
    endInsertRows();
    return true;
}

bool PackageModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        mItems.takeAt(row);
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
    for (const QModelIndex &index : indexes)
        if (index.isValid())
        {
            stream << data(index, IdRole).toInt()
                   << data(index).toString()
                   << data(index, CatRole).value<SolutionModel::IntList>();
        }
    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

bool PackageModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                int row, int column, const QModelIndex &parent)
{
    if (action == Qt::MoveAction)
    {
        if (data->hasFormat("application/vnd.text.list") && column < 1)
        {
            int beginRow;
            if (row != -1)
                beginRow = row;
            else if (parent.isValid())
                beginRow = parent.row();
            else
                beginRow = rowCount();
            QByteArray encodedData = data->data("application/vnd.text.list");
            QDataStream stream(&encodedData, QIODevice::ReadOnly);
            int rows = 0;
            ItemList tmpList;
            while (!stream.atEnd())
            {
                Item item;
                stream >> item.id >> item.package.name >> item.package.categories;
                tmpList.append(item);
                ++rows;
            }
            insertRows(beginRow, rows, QModelIndex());
            for (const Item &item : tmpList)
            {
                QModelIndex i = index(beginRow, 0, QModelIndex());
                setData(i, item.id, IdRole);
                setData(i, item.package.name);
                setData(i, QVariant::fromValue(item.package.categories), CatRole);
                ++beginRow;
            }
            return true;
        }
    }
    return false;
}
