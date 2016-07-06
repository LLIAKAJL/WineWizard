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

#ifndef PACKAGESORTMODEL_H
#define PACKAGESORTMODEL_H

#include <QSortFilterProxyModel>

class PackageSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum { CatIdRole = Qt::UserRole + 1 };

    explicit PackageSortModel(QObject *parent = nullptr);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    int mCatId;
};

#endif // PACKAGESORTMODEL_H
