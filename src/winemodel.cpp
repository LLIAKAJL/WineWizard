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

#include "winemodel.h"

WineModel::WineModel(const Repository::WineList &list, QObject *parent) :
    QAbstractListModel(parent),
    mList(list)
{
}

int WineModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mList.count();
}

QVariant WineModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
            return mList.at(index.row()).version;
        case IdRole:
            return mList.at(index.row()).id;
        case CategoryRole:
            return QVariant::fromValue<IntList>(mList.at(index.row()).categories);
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        default:
            return QVariant();
        }
    }
    return QVariant();
}
