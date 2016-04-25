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

#include "shortcutsmodel.h"
#include "filesystem.h"

ShortcutsModel::ShortcutsModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int ShortcutsModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mList.count();
}

QVariant ShortcutsModel::data(const QModelIndex &index, int role) const
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

bool ShortcutsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::UserRole)
    {
        beginResetModel();
        mSolution = value.toString();
        mList.clear();
        auto icons = FS::icons(mSolution);
        auto stdIcon = QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon);
        for (auto shortcut : FS::shortcuts(mSolution).entryInfoList(QDir::Files | QDir::Hidden))
        {
            auto path = shortcut.absoluteFilePath();
            auto fileName = shortcut.fileName();
            auto icon = icons.exists(fileName) ? QIcon(icons.absoluteFilePath(fileName)) : stdIcon;
            QSettings s(path, QSettings::IniFormat);
            s.setIniCodec("UTF-8");
            mList.append(Shortcut{ s.value("Name").toString(), fileName, icon });
        }
        endResetModel();
        return true;
    }
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::EditRole:
            {
                auto name = mList.at(index.row()).name;
                auto newName = value.toString();
                auto hash = mList.at(index.row()).hash;
                auto newHash = FS::hash(newName);
                if (hash.isEmpty())
                {
                    mList[index.row()].name = newName;
                    mList[index.row()].hash = newHash;
                    auto iDir = FS::icons(mSolution);
                    mList[index.row()].icon = iDir.exists(newHash) ? QIcon(iDir.absoluteFilePath(newHash)) : QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon);
                    QSettings s(FS::shortcuts(mSolution).absoluteFilePath(newHash), QSettings::IniFormat);
                    s.setIniCodec("UTF-8");
                    s.setValue("Name", newName);
                    emit dataChanged(index, index);
                    return true;
                }
                else if (name != newName)
                {
                    mList[index.row()].name = newName;
                    mList[index.row()].hash = newHash;
                    auto sDir = FS::shortcuts(mSolution);
                    if (sDir.exists(hash))
                        sDir.rename(hash, newHash);
                    auto iDir = FS::icons(mSolution);
                    if (iDir.exists(hash))
                        iDir.rename(hash, newHash);
                    QSettings s(sDir.absoluteFilePath(newHash), QSettings::IniFormat);
                    s.setIniCodec("UTF-8");
                    auto exe = s.value("Exe").toString();
                    if (exe.startsWith("y:/.links"))
                    {
                        FS::links(mSolution).rename(hash + ".lnk", newHash + ".lnk");
                        s.setValue("Exe", QString("y:/.links/%1.lnk").arg(newHash));
                    }
                    s.setValue("Name", newName);
                    emit dataChanged(index, index);
                    return true;
                }
                return false;
            }
        case Qt::DecorationRole:
            {
                auto hash = mList.at(index.row()).hash;
                auto iPath = FS::icons(mSolution).absoluteFilePath(hash);
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

bool ShortcutsModel::insertRows(int row, int/* count*/, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row);
    mList.insert(row, Shortcut{ QString(), QString(), QIcon() });
    endInsertRows();
    return true;
}

bool ShortcutsModel::removeRows(int row, int/* count*/, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    auto hash = mList.at(row).hash;
    auto iDir = FS::icons(mSolution);
    if (iDir.exists(hash))
        iDir.remove(hash);
    FS::shortcuts(mSolution).remove(hash);
    auto link = FS::links(mSolution).absoluteFilePath(hash + ".lnk");
    if (QFile::exists(link))
        QFile::remove(link);
    mList.removeAt(row);
    endRemoveRows();
    return true;
}
