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

#include "terminatemodel.h"
#include "mainmodel.h"

TerminateModel::TerminateModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    connect(this, &TerminateModel::sourceModelChanged, this, &TerminateModel::resetCheckList);
    connect(this, &TerminateModel::dataChanged, this, &TerminateModel::resetCheckList);
}

QVariant TerminateModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        return mapToSource(index).data(Qt::EditRole);
    case Qt::ForegroundRole:
        return QVariant();
    case Qt::CheckStateRole:
        return mCheckList.at(mapToSource(index).row()) ? Qt::Checked : Qt::Unchecked;
    default:
        return QSortFilterProxyModel::data(index, role);
    }
}

bool TerminateModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole)
    {
        mCheckList[mapToSource(index).row()] = (value.toUInt() == Qt::Checked);
        emit dataChanged(index, index);
        return true;
    }
    return QSortFilterProxyModel::setData(index, value, role);
}

bool TerminateModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    return sourceModel()->index(row, 0, parent).data(MainModel::RunningRole).toBool();
}

void TerminateModel::resetCheckList()
{
    for (int i = 0, count = sourceModel()->rowCount(); i < count; ++i)
        mCheckList.append(true);
}
