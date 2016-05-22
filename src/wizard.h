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

#ifndef WIZARD_H
#define WIZARD_H

#include <QStringList>
#include <QFileInfo>
#include <QSettings>

class Wizard : public QObject
{
    Q_OBJECT

public:
    explicit Wizard(QObject *parent = nullptr);

public slots:
    void start(const QString &cmdLine = QString());

private slots:
    void showMenu();

private:
    QStringList mBusyList, mRunList;
    bool mQuit;

    void install(const QString &cmdLine);
    bool testSuffix(const QFileInfo &path) const;
    bool prepare(QString &arch, QString &bs, QString &acs, QString &as) const;
    void required(const QString &package, QSet<QString> &res, QSettings *r) const;
    void clearRepository() const;
    QString makeConstScript(const QString &arch) const;
};

#endif // WIZARD_H
