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
#include <QEventLoop>
#include <QProcess>

#include "terminaldialog.h"
#include "waitdialog.h"
#include "filesystem.h"
#include "executor.h"
#include "dialogs.h"

namespace Ex
{
    QProcessEnvironment env(const QString &prefixHash)
    {
        QProcessEnvironment res = QProcessEnvironment::systemEnvironment();
        if (!prefixHash.isEmpty())
        {
            res.insert("WINEPREFIX", FS::prefix(prefixHash).absolutePath());
            QString winePath = FS::wine(prefixHash).absolutePath();
            res.insert("WINEVERPATH", winePath);
            res.insert("PATH", winePath + "/bin:" + res.value("PATH"));
            res.insert("WINESERVER", winePath + "/bin/wineserver");
            res.insert("WINELOADER", winePath + "/bin/wine");
            res.insert("WINEDLLPATH", winePath + "/lib/wine/fakedlls");
            res.insert("LD_LIBRARY_PATH", winePath + "/lib:" + res.value("LD_LIBRARY_PATH"));
            res.insert("WINEDLLOVERRIDES", "winemenubuilder.exe=n");
        }
        return res;
    }

    void release(const QString &script, const QString &prefixHash)
    {
        QProcess proc;
        QProcessEnvironment e = env(prefixHash);
        e.insert("WINEDEBUG", "-all");
        proc.setProcessEnvironment(e);
        QEventLoop loop;
        proc.connect(&proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), &loop, &QEventLoop::quit);
        proc.start("sh", QStringList("-c") << script);
        loop.exec();
    }

    Out debug(const QString &script, const QString &prefixHash)
    {
        QProcess proc;
        proc.setProcessEnvironment(env(prefixHash));
        QEventLoop loop;
        proc.connect(&proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), &loop, &QEventLoop::quit);
        proc.start("sh", QStringList("-c") << script);
        loop.exec();
        return qMakePair(proc.readAllStandardOutput(), proc.readAllStandardError());
    }

    void wait(const QString &script, const QString &prefixHash, QWidget *parent)
    {
        QProcess proc;
        proc.setProcessEnvironment(env(prefixHash));
        WaitDialog wd(parent);
        proc.connect(&proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), &wd, &WaitDialog::accept);
        proc.start("sh", QStringList("-c") << script);
        wd.exec();
    }

    void terminal(const QString &script, const QString &prefixHash, QWidget *parent)
    {
        QProcess proc;
        proc.setProcessEnvironment(env(prefixHash));
        TerminalDialog td(parent);
        proc.connect(&proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), &td, &TerminalDialog::executeFinished);
        proc.connect(&proc, &QProcess::readyReadStandardOutput, &td, [&td, &proc](){ td.appendOut(proc.readAllStandardOutput()); });
        proc.connect(&proc, &QProcess::readyReadStandardError, &td, [&td, &proc](){ td.appendErr(proc.readAllStandardError()); });
        proc.start("sh", QStringList("-c") << script);
        td.exec();
    }
}
