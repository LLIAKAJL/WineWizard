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

#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QJsonObject>
#include <QProcess>

class NetManager : public QObject
{
    Q_OBJECT

public:
    struct Request
    {
        QString url, out, data;
        bool post;
    };
    typedef QList<Request> RequestList;

    explicit NetManager(QObject *parent = nullptr);
    ~NetManager() override;

    static Request makeGetRequest(const QString &url, const QString &out);
    static Request makePostRequest(const QString &url, const QJsonObject &data, const QString &out = QString());
    void start(const RequestList &request);
    void terminate();

signals:
    void readyOutput(const QString &data);
    void readyError(const QString &data);
    void started();
    void error(int code);
    void finished();

private slots:
    void readyReadOutput();
    void readyReadError();
    void processFinished();

private:
    QProcess mProc;
};

#endif // NETMANAGER_H
