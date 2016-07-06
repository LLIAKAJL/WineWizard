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

#include <QProcess>

#include "mainwindow.h"
#include "filesystem.h"
#include "executor.h"
#include "wizard.h"

#define FINISHED static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished)

Executor::ExList Executor::mInstances;

Executor::Executor(const QString &prefix,QObject *parent) :
    QObject(parent),
    mPrefix(prefix)
{
    mInstances.append(this);
}

Executor::~Executor()
{
    if (mProc.state() == QProcess::Running)
    {
        mProc.disconnect();
        if (!mPrefix.isEmpty())
        {
            QProcess killProc;
            killProc.setProcessEnvironment(env());
            killProc.setProcessChannelMode(QProcess::ForwardedChannels);
            killProc.start("sh", QStringList("-c") << FS::readFile(":/terminate"), QProcess::NotOpen);
            killProc.waitForStarted();
            killProc.waitForFinished();
        }
        if (mProc.state() == QProcess::Running)
        {
            mProc.terminate();
            mProc.waitForFinished();
        }
    }
    mInstances.removeOne(this);
    MainWindow::update();
    Wizard::update();
}

void Executor::start(const QString &script, bool release, QProcess::ProcessChannelMode mode)
{
    QProcessEnvironment e = env();
    mProc.setProcessEnvironment(e);
    if (mode == QProcess::SeparateChannels)
    {
        connect(&mProc, &QProcess::readyReadStandardOutput, this, &Executor::readyReadOutput);
        connect(&mProc, &QProcess::readyReadStandardError, this, &Executor::readyReadError);
    }
    else
        mProc.setProcessChannelMode(mode);
    if (release)
        e.insert("WINEDEBUG", "-all");
    connect(&mProc, FINISHED, this, &Executor::finished);
    connect(this, &Executor::finished, this, &Executor::deleteLater);
    connect(&mProc, &QProcess::started, this, &Executor::started);
    mProc.start("sh", QStringList("-c") << script, (mode == QProcess::ForwardedChannels) ?
                    QProcess::NotOpen : QProcess::ReadOnly);
    MainWindow::update();
}

const QString &Executor::prefix() const
{
    return mPrefix;
}

Executor::ExList &Executor::instances()
{
    return mInstances;
}

bool Executor::contains(const QString &prefix)
{
    for (Executor *e : mInstances)
        if (e->prefix() == prefix)
            return true;
    return false;
}

void Executor::readyReadOutput()
{
    QProcess *proc = static_cast<QProcess *>(sender());
    emit readyOutput(proc->readAllStandardOutput());
}

void Executor::readyReadError()
{
    QProcess *proc = static_cast<QProcess *>(sender());
    emit readyError(proc->readAllStandardError());
}

QProcessEnvironment Executor::env() const
{
    QProcessEnvironment res = QProcessEnvironment::systemEnvironment();
    if (!mPrefix.isEmpty())
    {
        res.insert("WINEPREFIX", FS::prefix(mPrefix).absolutePath());
        QString winePath = FS::wine(mPrefix).absolutePath();
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
