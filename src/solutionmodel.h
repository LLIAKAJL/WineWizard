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

#ifndef SOLUTIONMODEL_H
#define SOLUTIONMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QIcon>

#include "repository.h"

class SolutionModel : public QAbstractListModel
{
    Q_OBJECT

    struct Item
    {
        int rating, bw, aw, id;
        QString date, bs, as;
        IntList bp, ap;
        bool approved;
    };
    typedef QList<Item> ItemList;

public:

    enum { IdRole = Qt::UserRole + 1, BWRole, AWRole, BPRole, APRole, BSRole, ASRole, ApprovedRole };

    explicit SolutionModel(const QString &prefix, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

private:
    ItemList mList;

    QIcon ratingToIcon(const QModelIndex &index) const;
    void resetItem(const QModelIndex &index);
};

#endif // SOLUTIONMODEL_H
