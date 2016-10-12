/***************************************************************************
 *   Copyright (C) 2016 by Vitalii Kachemtsev <LLIAKAJI@wwizard.net>       *
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

#include <QJsonDocument>
#include <QProcess>

#include "netmanager.h"
#include "utils.h"

NetManager::NetManager(QObject *parent) :
    QObject(parent)
{
    mProc.connect(&mProc, &QProcess::readyReadStandardOutput, this, &NetManager::readyReadOutput);
    mProc.connect(&mProc, &QProcess::readyReadStandardError, this, &NetManager::readyReadError);
    mProc.connect(&mProc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &NetManager::processFinished);
}

NetManager::~NetManager()
{
    terminate();
}

NetManager::Request NetManager::makeGetRequest(const QString &url, const QString &out)
{
    Request res;
    res.out = out;
    res.post = false;
    res.url = url;
    return res;
}

NetManager::Request NetManager::makePostRequest(const QString &url, const QJsonObject &data, const QString &out)
{
    Request res;
    res.data = QJsonDocument(data).toJson();
    res.out = out;
    res.post = true;
    res.url = url;
    return res;
}

void NetManager::start(const RequestList &request)
{
    QString get = Utils::readFile(":/scripts/get");
    QString post = Utils::readFile(":/scripts/post");
    QString script;
    for (const Request &r : request)
    {
        if (r.post)
            script += post.arg(r.url).arg(r.data).arg(r.out);
        else
            script += get.arg(r.url).arg(r.out);
    }
    mProc.start("sh", QStringList("-c") << script, QProcess::ReadOnly);
    mProc.waitForStarted();
    emit started();
}

void NetManager::terminate()
{
    if (mProc.state() == QProcess::Running)
    {
        mProc.terminate();
        mProc.waitForFinished();
    }
}

void NetManager::readyReadOutput()
{
    emit readyOutput(mProc.readAllStandardOutput());
}

void NetManager::readyReadError()
{
    emit readyError(mProc.readAllStandardError());
}

void NetManager::processFinished()
{
    if (mProc.exitCode() != 0)
        emit error(mProc.exitCode());
    else
        emit finished();
}
