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

#include <QProcessEnvironment>
#include <QStringList>
#include <QSettings>
#include <QFileInfo>
#include <QObject>

class Wizard : public QObject
{
    Q_OBJECT

    enum ExeType { EXE_X86, EXE_X64, EXE_INVALID };

public:
    explicit Wizard(QObject *parent = nullptr);

public slots:
    void start(const QString &exe = QString());

private:
    QStringList mBusyList, mRunList;

    void showMenu();
    void install(const QString &exe);
    QString required(const QStringList &packages) const;
    void killSolution(const QString &solution);
    bool testSuffix(const QFileInfo &path) const;
};

#endif // WIZARD_H
