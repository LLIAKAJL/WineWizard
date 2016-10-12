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

#include "categorymodel.h"

CategoryModel::CategoryModel(const SolutionModel::CategoryList &categories, QObject *parent) :
    QAbstractListModel(parent)
{
    for (SolutionModel::CategoryList::ConstIterator i = categories.begin(); i != categories.end(); ++i)
    {
        Item item;
        item.id = i.key();
        item.name = i.value();
        mItems.append(item);
    }
}

int CategoryModel::rowCount(const QModelIndex &) const
{
    return mItems.count();
}

QVariant CategoryModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
            return mItems.at(index.row()).name;
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
