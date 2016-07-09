/***************************************************************************
 *   Copyright (C) 2016 by Vitalii Kachemtsev <LLIAKAJI@wwizard.net>         *
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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QDir>

namespace FS
{
    QDir cache();
    QDir data();
    QDir config();
    QDir temp();

    QDir prefix(const QString &prefixDirName);
    QDir devices(const QString &prefix);
    QDir drive(const QString &prefix, const QString &letter = "c:");
    QDir driveTarget(const QString &prefix, const QString &letter = "c:");
    QDir icons(const QString &prefix);
    QDir shortcuts(const QString &prefix);
    QDir links(const QString &prefix);
    QDir documents(const QString &prefix);
    QDir wine(const QString &prefix);
    QDir packages(const QString &prefix);
    QDir windows(const QString &prefix);
    QDir sys32(const QString &prefix, const QString &arch = "32");
    QDir sys64(const QString &prefix);
    QDir users(const QString &prefix);
    QDir user(const QString &prefix);

    QByteArray readFile(const QString &path);
    void browse(const QString &path);
    QString hash(const QString &str);

    QString toWinPath(const QString &prefix, const QString &path);
    QString toUnixPath(const QString &prefix, const QString &path);
}

#endif // FILESYSTEM_H
