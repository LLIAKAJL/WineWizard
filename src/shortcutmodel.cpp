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

#include <QFileInfoList>
#include <QApplication>
#include <QSettings>
#include <QMimeData>
#include <QStyle>
#include <QUrl>

#include "shortcutmodel.h"
#include "filesystem.h"

const QString DESKTOP_FILE = "[Desktop Entry]\nExec=winewizard %1 %2\n" \
        "Name=%3\nIcon=%4\nType=Application";

ShortcutModel::ShortcutModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int ShortcutModel::rowCount(const QModelIndex &/*parent*/) const
{
    return mList.count();
}

QVariant ShortcutModel::data(const QModelIndex &index, int role) const
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
        case ShortcutRole:
            return mList.at(index.row()).shortcut;
        case ExeRole:
            return FS::toUnixPath(mPrefix, mList.at(index.row()).exe);
        case WorkDirRole:
            return FS::toUnixPath(mPrefix, mList.at(index.row()).workDir);
        case ArgsRole:
            return mList.at(index.row()).args;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool ShortcutModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == ResetRole)
    {
        beginResetModel();
        mList.clear();
        mPrefix = value.toString();
        QDir iDir = FS::icons(mPrefix);
        QDir sDir = FS::shortcuts(mPrefix);
        QStringList sList = sDir.entryList(QDir::Files);
        for (const QString &sEntry : sList)
        {
            Shortcut shortcut;
            shortcut.icon = iDir.exists(sEntry) ?
                        QIcon(iDir.absoluteFilePath(sEntry)) :
                        QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon);
            QSettings s(sDir.absoluteFilePath(sEntry), QSettings::IniFormat);
            s.setIniCodec("UTF-8");
            shortcut.name = s.value("Name").toString();
            shortcut.shortcut = sEntry;
            shortcut.exe = s.value("Exe").toString();
            shortcut.workDir = s.value("WorkDir").toString();
            shortcut.args = s.value("Args").toString();
            mList.append(shortcut);
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
                QString newName = value.toString();
                if (newName != mList[index.row()].name && !newName.isEmpty() && !exists(newName))
                {
                    QDir sDir = FS::shortcuts(mPrefix);
                    QString hash = mList.at(index.row()).shortcut;
                    QString newHash = FS::hash(newName);
                    if (!hash.isEmpty())
                    {
                        if (sDir.exists(hash))
                            sDir.rename(hash, newHash);
                        QDir iDir = FS::icons(mPrefix);
                        if (iDir.exists(hash))
                            iDir.rename(hash, newHash);
                    }
                    mList[index.row()].name = newName;
                    mList[index.row()].shortcut = newHash;
                    saveValue(index, "Name", newName);
                    emit dataChanged(index, index);
                    return true;
                }
            }
            return false;
        case ExeRole:
            {
                saveValue(index, "Exe", value);
                mList[index.row()].exe = value.toString();
            }
            emit dataChanged(index, index);
            return true;
        case WorkDirRole:
            {
                saveValue(index, "WorkDir", value);
                mList[index.row()].workDir = value.toString();
            }
            emit dataChanged(index, index);
            return true;
        case ArgsRole:
            {
                saveValue(index, "Args", value);
                mList[index.row()].args = value.toString();
            }
            emit dataChanged(index, index);
            return true;
        case Qt::DecorationRole:
            {
                QString iconPath = value.toString();
                QDir iDir = FS::icons(mPrefix);
                QString hash = mList.at(index.row()).shortcut;
                if (iDir.exists(hash))
                    iDir.remove(hash);
                QString myIconPath = iDir.absoluteFilePath(hash);
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

bool ShortcutModel::insertRows(int row, int/* count*/, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row);
    Shortcut s;
    s.icon = QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon);
    mList.insert(row, s);
    endInsertRows();
    return true;
}

bool ShortcutModel::removeRows(int row, int/* count*/, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    QDir iDir = FS::icons(mPrefix);
    QDir sDir = FS::shortcuts(mPrefix);
    QString hash = mList.at(row).shortcut;
    sDir.remove(hash);
    if (iDir.exists(hash))
        iDir.remove(hash);
    mList.removeAt(row);
    endRemoveRows();
    return true;
}

Qt::ItemFlags ShortcutModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}

QStringList ShortcutModel::mimeTypes() const
{
    return QStringList("text/uri-list");
}

QMimeData *ShortcutModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData;
    if (!indexes.isEmpty())
    {
        QModelIndex index = indexes.first();
        if (index.isValid())
        {
            QString shortcut = data(index, ShortcutRole).toString();
            QString name = data(index).toString();
            QString path = FS::temp().absoluteFilePath(name + ".desktop");
            QFile f(path);
            f.open(QFile::WriteOnly);
            f.write(DESKTOP_FILE.arg(mPrefix).arg(shortcut).arg(name).
                    arg(FS::icons(mPrefix).absoluteFilePath(shortcut)).toUtf8());
            f.setPermissions(f.permissions() | QFile::ExeGroup | QFile::ExeOther |
                             QFile::ExeOwner | QFile::ExeUser);
            QList<QUrl> urls;
            urls.append(QUrl::fromLocalFile(path));
            mimeData->setUrls(urls);
        }
    }
    return mimeData;
}

bool ShortcutModel::exists(const QString &name) const
{
    return !match(index(0), Qt::DisplayRole, name, -1, Qt::MatchFixedString).empty();
}

void ShortcutModel::saveValue(const QModelIndex &index, const QString &key, const QVariant &value)
{
    QString hash = mList.at(index.row()).shortcut;
    QDir sDir = FS::shortcuts(mPrefix);
    QSettings s(sDir.absoluteFilePath(hash), QSettings::IniFormat);
    s.setIniCodec("UTF-8");
    s.setValue(key, value.toString());
}
