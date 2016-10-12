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

#include <QSet>

#include "packagesortmodel.h"

PackageSortModel::PackageSortModel(PackageModel *pModel, QObject *parent) :
    QSortFilterProxyModel(parent),
    mCat(0)
{
    setSourceModel(pModel);
    sort(0);
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool PackageSortModel::filterAcceptsRow(int sRow, const QModelIndex &sParent) const
{
    QModelIndex i = sourceModel()->index(sRow, 0, sParent);
    if (filterRegExp().isEmpty() || i.data().toString().contains(filterRegExp()))
        if (i.data(PackageModel::CatRole).value<SolutionModel::IntList>().contains(mCat))
            return true;
    return false;
}

bool PackageSortModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == CatRole)
    {
        mCat = value.toInt();
        invalidate();
        return true;
    }
    return QSortFilterProxyModel::setData(index, value, role);
}
