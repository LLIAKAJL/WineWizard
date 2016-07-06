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

#include <QCryptographicHash>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QThread>
#include <QUrl>

#include "filesystem.h"
#include "dialogs.h"

namespace FS
{
    QDir make(const QString &path)
    {
        QDir res(path);
        if (!res.exists())
            res.mkpath(res.absolutePath());
        return res;
    }

    QDir cache()
    {
        return make(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    }

    QDir data()
    {
        return make(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    }

    QDir config()
    {
        return make(QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).absoluteFilePath("winewizard"));
    }

    QDir temp()
    {
        return make(QDir::temp().absoluteFilePath(".winewizard"));
    }

    QDir prefix(const QString &prefixDirName)
    {
        return data().absoluteFilePath(prefixDirName);
    }

    QDir devices(const QString &prefix)
    {
        return FS::prefix(prefix).absoluteFilePath("dosdevices");
    }

    QDir drive(const QString &prefix, const QString &letter)
    {
        return devices(prefix).absoluteFilePath(letter);
    }

    QDir driveTarget(const QString &prefix, const QString &letter)
    {
        return QFile::symLinkTarget(FS::drive(prefix, letter).absolutePath());
    }

    QDir icons(const QString &prefixHash)
    {
        return FS::prefix(prefixHash).absoluteFilePath(".icons");
    }

    QDir shortcuts(const QString &prefix)
    {
        return FS::prefix(prefix).absoluteFilePath(".shortcuts");
    }

    QDir links(const QString &prefix)
    {
        return FS::prefix(prefix).absoluteFilePath(".links");
    }

    QDir documents(const QString &prefix)
    {
        return FS::prefix(prefix).absoluteFilePath(".documents");
    }

    QDir wine(const QString &prefix)
    {
        return FS::prefix(prefix).absoluteFilePath(".wine");
    }

    QDir packages(const QString &prefix)
    {
        return FS::prefix(prefix).absoluteFilePath(".packages");
    }

    QDir windows(const QString &prefix)
    {
        return drive(prefix).absoluteFilePath("windows");
    }

    QDir sys32(const QString &prefix, const QString &arch)
    {
        if (arch == "32")
            return windows(prefix).absoluteFilePath("system32");
        else
            return windows(prefix).absoluteFilePath("syswow64");
    }

    QDir sys64(const QString &prefix)
    {
        return windows(prefix).absoluteFilePath("system32");
    }

    QDir users(const QString &prefix)
    {
        return drive(prefix).absoluteFilePath("users");
    }

    QDir user(const QString &prefix)
    {
        return users(prefix).absoluteFilePath(qgetenv("USER"));
    }

    QByteArray readFile(const QString &path)
    {
        QFile f(path);
        if (f.exists())
        {
            f.open(QFile::ReadOnly);
            return f.readAll();
        }
        return QByteArray();
    }

    void browse(const QString &path)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }

    QString hash(const QString &str)
    {
        return QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha1).toHex();
    }

    QString toWinPath(const QString &prefix, const QString &path)
    {
        QString res(path);
        if (res.contains('/'))
        {
            for (char c = 'a'; c <= 'z'; ++c)
            {
                QString targetDrivePath = driveTarget(prefix, QString(c) + ":").absolutePath();
                QString drivePath = FS::drive(prefix, QString(c) + ":").absolutePath();
                QString driveLetter = QString(QChar::toUpper(c)) + ":/";
                if (res.startsWith(targetDrivePath))
                {
                    res.replace(0, targetDrivePath.length(), driveLetter);
                    break;
                }
                else if (res.startsWith(drivePath))
                {
                    res.replace(0, drivePath.length(), driveLetter);
                    break;
                }
            }
            return res.replace("//", "/").replace('/', '\\');
        }
        return res;
    }

    QString toUnixPath(const QString &prefix, const QString &path)
    {
        QString res(path);
        if (res.contains('\\'))
        {
            res.replace('\\', '/');
            for (char c = 'a'; c <= 'z'; ++c)
                if (res.startsWith(QString(c) + ":/") || res.startsWith(QString(QChar::toUpper(c)) + ":/"))
                {
                    QString driveLetter = QString(QChar(res.at(0)).toLower()) + ":";
                    QString target = FS::driveTarget(prefix, driveLetter).absolutePath();
                    if (!target.endsWith('/'))
                        target += '/';
                    res.replace(0, 3, target);
                    break;
                }
        }
        return res;
    }
}
