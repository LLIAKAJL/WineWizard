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

#include "searchmodel.h"
#include "filesystem.h"

SearchModel::SearchModel(QObject *parent) :
    QAbstractListModel(parent),
    mPostsCount(0),
    mExists(false)
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
        case EditableRole:
            return mList.at(index.row()).editable;
        case SlugRole:
            return mList.at(index.row()).slug;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        }
    }
    switch (role)
    {
        case ExistsRole:
            return mExists;
        case CountRole:
            return mPostsCount;
    }
    return QVariant();
}

bool SearchModel::setData(const QModelIndex &/*index*/, const QVariant &/*value*/, int role)
{
    if (role == ReloadRole)
    {
        beginResetModel();
        mList.clear();
        QStringList fData = FS::readFile(FS::temp().absoluteFilePath("search")).split('\n', QString::SkipEmptyParts);
        mPostsCount = fData.takeFirst().toInt();
        mExists = fData.takeFirst().toInt() == 1;
        for (int i = 0, count = fData.count(); i < count; i += 3)
        {
            QString name = fData.at(i);
            bool editable = fData.at(i + 1).toInt() == 1;
            QString slug = fData.at(i + 2);
            mList.append(Solution{ name, slug, editable });
        }
        endResetModel();
        return true;
    }
    return false;
}
