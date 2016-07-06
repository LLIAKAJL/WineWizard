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

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QProcess>
#include <QWidget>

class Executor : public QObject
{
    Q_OBJECT

    typedef QList<Executor *> ExList;

public:
    explicit Executor(const QString &prefix = QString(), QObject *parent = nullptr);
    ~Executor() override;

    void start(const QString &script, bool release = false,
               QProcess::ProcessChannelMode mode = QProcess::SeparateChannels);
    const QString &prefix() const;

    static ExList &instances();
    static bool contains(const QString &prefix);

signals:
    void readyOutput(const QByteArray &output);
    void readyError(const QByteArray &data);
    void started();
    void finished(int code);

private slots:
    void readyReadOutput();
    void readyReadError();

private:
    static ExList mInstances;
    QProcess mProc;
    QString mPrefix;

    QProcessEnvironment env() const;
};

#endif // EXECUTOR_H
