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
    QProcessEnvironment env(const QString &solution)
    {
        auto res = QProcessEnvironment::systemEnvironment();
        if (!solution.isEmpty())
        {
            res.insert("WINEPREFIX", FS::solution(solution).absolutePath());
            auto winePath = FS::wine(solution).absolutePath();
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

    void execute(const QString &script, const QString &solution)
    {
        QProcess proc;
        proc.setProcessEnvironment(env(solution));
        QEventLoop loop;
        proc.connect(&proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), &loop, &QEventLoop::quit);
        proc.start("sh", QStringList("-c") << script);
        loop.exec();
    }

    void wait(const QString &script, const QString &solution, QWidget *parent)
    {
        QProcess proc;
        proc.setProcessEnvironment(env(solution));
        WaitDialog wd(parent);
        proc.connect(&proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), &wd, &WaitDialog::accept);
        proc.start("sh", QStringList("-c") << script);
        wd.exec();
    }

    void terminal(const QString &script, const QString &solution, QWidget *parent)
    {
        QProcess proc;
        proc.setProcessEnvironment(env(solution));
        TerminalDialog td(parent);
        proc.connect(&proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), &td, &TerminalDialog::executeFinished);
        proc.connect(&proc, &QProcess::readyReadStandardOutput, &td, [&td, &proc](){ td.appendText(proc.readAllStandardOutput()); });
        proc.connect(&proc, &QProcess::readyReadStandardError, &td, [&td, &proc](){ td.appendText(proc.readAllStandardError()); });
        proc.start("sh", QStringList("-c") << script);
        td.exec();
    }
}
