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

#include "installwizard.h"
#include "searchmodel.h"
#include "prefixmodel.h"
#include "filesystem.h"

PrefixModel::PrefixModel(QObject *parent) :
    QAbstractListModel(parent)
{
    QString busy;
    if (InstallWizard::instance())
    {
        QModelIndex ai = InstallWizard::instance()->field("app").toModelIndex();
        if (ai.isValid())
            busy = ai.data(SearchModel::PrefixRole).toString();
    }
    QStringList pList = FS::data().entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (const QString &pEntry : pList)
    {
        if (pEntry == busy)
            continue;
        Prefix prefix;
        prefix.prefix = pEntry;
        QDir pDir = FS::prefix(prefix.prefix);
        QSettings s(pDir.absoluteFilePath(".settings"), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        prefix.name = s.value("Name").toString();
        QString iconPath = pDir.absoluteFilePath(".icon");
        if (!QFile::exists(iconPath))
        {
            QFileInfoList icons = FS::icons(prefix.prefix).entryInfoList(QDir::Files);
            if (!icons.isEmpty())
            {
                QFile::copy(icons.first().absoluteFilePath(), iconPath);
                prefix.icon = QIcon(iconPath);
            }
            else
                prefix.icon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
        }
        else
            prefix.icon = QIcon(iconPath);
        mList.append(prefix);
    }
}

int PrefixModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mList.count();
}

QVariant PrefixModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return mList.at(index.row()).name;
        case Qt::DecorationRole:
            return mList.at(index.row()).icon;
        case PrefixRole:
            return mList.at(index.row()).prefix;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool PrefixModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::EditRole:
            {
                QString newName = value.toString();
                if (newName != mList.at(index.row()).name && !newName.isEmpty() && !exists(newName))
                {
                    QString newHash = FS::hash(newName);
                    FS::data().rename(mList.at(index.row()).prefix, newHash);
                    {
                        QSettings s(FS::prefix(newHash).absoluteFilePath(".settings"),
                                    QSettings::IniFormat);
                        s.setIniCodec("UTF-8");
                        s.setValue("Name", newName);
                    }
                    mList[index.row()].name = newName;
                    mList[index.row()].prefix = newHash;
                    emit dataChanged(index, index);
                    return true;
                }
            }
            return false;
        case Qt::DecorationRole:
            {
                QString iconPath = value.toString();
                QDir pDir = FS::prefix(mList.at(index.row()).prefix);
                if (pDir.exists(".icon"))
                    pDir.remove(".icon");
                QString myIconPath = pDir.absoluteFilePath(".icon");
                QFile::copy(iconPath, myIconPath);
                mList[index.row()].icon = QIcon(myIconPath);
                emit dataChanged(index, index);
            }
            return true;
        default:
            return false;
        }
    }
    return false;
}

bool PrefixModel::removeRows(int row, int/* count*/, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    mList.removeAt(row);
    endRemoveRows();
    return true;
}

Qt::ItemFlags PrefixModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool PrefixModel::exists(const QString &name) const
{
    return !match(index(0), Qt::DisplayRole, name, -1, Qt::MatchFixedString).empty();
}
