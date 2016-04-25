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

#include <QApplication>
#include <QSettings>
#include <QStyle>

#include "solutionmodel.h"
#include "filesystem.h"

SolutionModel::SolutionModel(QObject *parent) :
    QAbstractListModel(parent)
{
    auto stdIcon = QApplication::style()->standardIcon(QStyle::SP_DirLinkIcon);
    for (auto solution : FS::data().entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
    {
        auto sDir = FS::solution(solution);
        QFileInfo iconPath = sDir.absoluteFilePath(".icon");
        auto icon = iconPath.exists() ? QIcon(iconPath.absoluteFilePath()) : stdIcon;
        QSettings s(sDir.absoluteFilePath(".settings"), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        mList.append(Solution{ s.value("Name").toString(), solution, icon });
    }
}

int SolutionModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mList.count();
}

QVariant SolutionModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::DisplayRole:
            return mList.at(index.row()).name;
        case Qt::DecorationRole:
            return mList.at(index.row()).icon;
        case Qt::UserRole:
            return mList.at(index.row()).hash;
        }
    }
    return QVariant();
}

bool SolutionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::EditRole:
            {
                auto name = mList.at(index.row()).name;
                auto newName = value.toString();
                if (newName != name)
                {
                    auto hash = mList.at(index.row()).hash;
                    auto newHash = FS::hash(newName);
                    auto sPath = FS::solution(hash).absoluteFilePath(".settings");
                    {
                        QSettings s(sPath, QSettings::IniFormat);
                        s.setIniCodec("UTF-8");
                        s.setValue("Name", newName);
                    }
                    FS::data().rename(hash, newHash);
                    mList[index.row()].name = newName;
                    mList[index.row()].hash = newHash;
                    emit dataChanged(index, index);
                    return true;
                }
                return false;
            }
        case Qt::DecorationRole:
            {
                auto iPath = FS::solution(mList.at(index.row()).hash).absoluteFilePath(".icon");
                if (QFile::exists(iPath))
                    QFile::remove(iPath);
                QFile::copy(value.toString(), iPath);
                mList[index.row()].icon = QIcon(iPath);
                emit dataChanged(index, index);
                return true;
            }
        }
    }
    return false;
}

bool SolutionModel::removeRows(int row, int/* count*/, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    mList.removeAt(row);
    endRemoveRows();
    return true;
}
