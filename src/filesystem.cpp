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
#include "waitdialog.h"

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

    QDir prefix(const QString &prefixHash)
    {
        return data().absoluteFilePath(prefixHash);
    }

    QDir devices(const QString &prefixHash)
    {
        return prefix(prefixHash).absoluteFilePath("dosdevices");
    }

    QDir drive(const QString &prefixHash, const QString &letter)
    {
        return devices(prefixHash).absoluteFilePath(letter);
    }

    QDir driveTarget(const QString &prefixHash, const QString &letter)
    {
        return QFile::symLinkTarget(FS::drive(prefixHash, letter).absolutePath());
    }

    QDir icons(const QString &prefixHash)
    {
        return prefix(prefixHash).absoluteFilePath(".icons");
    }

    QDir shortcuts(const QString &prefixHash)
    {
        return prefix(prefixHash).absoluteFilePath(".shortcuts");
    }

    QDir links(const QString &prefixHash)
    {
        return prefix(prefixHash).absoluteFilePath(".links");
    }

    QDir documents(const QString &prefixHash)
    {
        return prefix(prefixHash).absoluteFilePath(".documents");
    }

    QDir wine(const QString &prefixHash)
    {
        return prefix(prefixHash).absoluteFilePath(".wine");
    }

    QDir packages(const QString &prefixHash)
    {
        return prefix(prefixHash).absoluteFilePath(".packages");
    }

    QDir windows(const QString &prefixHash)
    {
        return drive(prefixHash).absoluteFilePath("windows");
    }

    QDir sys32(const QString &prefixHash, const QString &arch)
    {
        if (arch == "32")
            return windows(prefixHash).absoluteFilePath("system32");
        else
            return windows(prefixHash).absoluteFilePath("syswow64");
    }

    QDir sys64(const QString &prefixHash)
    {
        return windows(prefixHash).absoluteFilePath("system32");
    }

    QDir users(const QString &prefixHash)
    {
        return drive(prefixHash).absoluteFilePath("users");
    }

    QDir user(const QString &prefixHash)
    {
        return users(prefixHash).absoluteFilePath(qgetenv("USER"));
    }

    QString readFile(const QString &filePath)
    {
        QFile f(filePath);
        if (f.exists())
        {
            f.open(QFile::ReadOnly);
            return f.readAll();
        }
        return QString();
    }

    void browse(const QString &path)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }

    QString hash(const QString &str)
    {
        return QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha1).toHex();
    }

    bool checkFileSum(const QString &filePath, const QString &checksum)
    {
        bool res = false;
        WaitDialog wd;
        QObject *worker = new QObject;
        worker->moveToThread(new QThread);
        wd.connect(worker->thread(), &QThread::started, worker, [worker, &checksum, &res, &filePath]()
        {
            if (QFile::exists(filePath))
            {
                QFile f(filePath);
                f.open(QFile::ReadOnly);
                QByteArray data = f.readAll();
                f.close();
                QString fSum = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();
                res = fSum == checksum;
            }
            else
                res = false;
            worker->deleteLater();
        });
        wd.connect(worker, &QObject::destroyed, worker->thread(), &QThread::quit);
        wd.connect(worker->thread(), &QThread::finished, worker->thread(), &QThread::deleteLater);
        wd.connect(worker->thread(), &QThread::destroyed, &wd, &WaitDialog::accept);
        worker->thread()->start();
        wd.exec();
        return res;
    }

    void removePrefix(const QString &prefixHash, QWidget *parent)
    {
        WaitDialog wd(parent);
        auto worker = new QObject;
        worker->moveToThread(new QThread);
        wd.connect(worker->thread(), &QThread::started, worker, [worker, prefixHash]()
        {
            prefix(prefixHash).removeRecursively();
            worker->deleteLater();
        });
        wd.connect(worker, &QObject::destroyed, worker->thread(), &QThread::quit);
        wd.connect(worker->thread(), &QThread::finished, worker->thread(), &QThread::deleteLater);
        wd.connect(worker->thread(), &QThread::destroyed, &wd, &WaitDialog::accept);
        worker->thread()->start();
        wd.exec();
    }

    QString toWinPath(const QString &prefixHash, const QString &path)
    {
        QString res(path);
        if (res.contains('/'))
        {
            for (char c = 'a'; c <= 'z'; ++c)
            {
                QString targetDrivePath = driveTarget(prefixHash, QString(c) + ":").absolutePath();
                QString drivePath = FS::drive(prefixHash, QString(c) + ":").absolutePath();
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

    QString toUnixPath(const QString &prefixHash, const QString &path)
    {
        QString res(path);
        if (res.contains('\\'))
        {
            res.replace('\\', '/');
            for (char c = 'a'; c <= 'z'; ++c)
                if (res.startsWith(QString(c) + ":/") || res.startsWith(QString(QChar::toUpper(c)) + ":/"))
                {
                    QString driveLetter = QString(QChar(res.at(0)).toLower()) + ":";
                    QString target = FS::driveTarget(prefixHash, driveLetter).absolutePath();
                    if (!target.endsWith('/'))
                        target += '/';
                    res.replace(0, 3, target);
                    break;
                }
        }
        return res;
    }
}
