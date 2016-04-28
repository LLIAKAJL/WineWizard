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

    QDir mainPackageDir()
    {
        return make(cache().absoluteFilePath("main_package"));
    }

    QString mainPart(const QString &packageName)
    {
        return mainPackageDir().absoluteFilePath(packageName);
    }

    QDir solution(const QString &solutionHash)
    {
        return data().absoluteFilePath(solutionHash);
    }

    QDir devices(const QString &solutionHash)
    {
        return solution(solutionHash).absoluteFilePath("dosdevices");
    }

    QDir drive(const QString &solutionHash, const QString &letter)
    {
        return devices(solutionHash).absoluteFilePath(letter);
    }

    QDir driveTarget(const QString &solutionHash, const QString &letter)
    {
        return QFile::symLinkTarget(FS::drive(solutionHash, letter).absolutePath());
    }

    QDir icons(const QString &solutionHash)
    {
        return solution(solutionHash).absoluteFilePath(".icons");
    }

    QDir shortcuts(const QString &solutionHash)
    {
        return solution(solutionHash).absoluteFilePath(".shortcuts");
    }

    QDir documents(const QString &solutionHash)
    {
        return solution(solutionHash).absoluteFilePath(".documents");
    }

    QDir wine(const QString &solutionHash)
    {
        return solution(solutionHash).absoluteFilePath(".wine");
    }

    QDir packages(const QString &solutionHash)
    {
        return solution(solutionHash).absoluteFilePath(".packages");
    }

    QDir windows(const QString &solutionHash)
    {
        return drive(solutionHash).absoluteFilePath("windows");
    }

    QDir sys32(const QString &solutionHash)
    {
        return windows(solutionHash).absoluteFilePath("system32");
    }

    QDir users(const QString &solutionHash)
    {
        return drive(solutionHash).absoluteFilePath("users");
    }

    QDir user(const QString &solutionHash)
    {
        return users(solutionHash).absoluteFilePath(qgetenv("USER"));
    }

    QDir links(const QString &solutionHash)
    {
        return solution(solutionHash).absoluteFilePath(".links");
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

    QString hash(const QString &solutionName)
    {
        return QCryptographicHash::hash(solutionName.toUtf8(), QCryptographicHash::Sha1).toHex();
    }

    QString md5(const QString &filePath)
    {
        QFile f(filePath);
        f.open(QFile::ReadOnly);
        return QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5).toHex();
    }

    QString pathWithDrive(const QString &solutionHash, const QString &path)
    {
        auto res = path;
        auto c = driveTarget(solutionHash).absolutePath();
        if (res.startsWith(c))
            res.replace(c, "c:");
        else
        {
            auto y = FS::driveTarget(solutionHash, "y:").absolutePath();
            if (res.startsWith(y))
                res.replace(y, "y:");
            else
                res.push_front("z:");
        }
        return res;
    }

    void removeSolution(const QString &solutionHash)
    {
        WaitDialog wd;
        auto worker = new QObject;
        worker->moveToThread(new QThread);
        wd.connect(worker->thread(), &QThread::started, worker, [worker, solutionHash]()
        {
            solution(solutionHash).removeRecursively();
            worker->deleteLater();
        });
        wd.connect(worker, &QObject::destroyed, worker->thread(), &QThread::quit);
        wd.connect(worker->thread(), &QThread::finished, worker->thread(), &QThread::deleteLater);
        wd.connect(worker->thread(), &QThread::destroyed, &wd, &WaitDialog::accept);
        worker->thread()->start();
        wd.exec();
    }
}
