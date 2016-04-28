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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QDir>

namespace FS
{
    QDir cache();
    QDir data();
    QDir config();
    QDir temp();
    QDir mainPackageDir();
    QString mainPart(const QString &packageName);

    QDir solution(const QString &solutionHash);
    QDir devices(const QString &solutionHash);
    QDir drive(const QString &solutionHash, const QString &letter = "c:");
    QDir driveTarget(const QString &solutionHash, const QString &letter = "c:");
    QDir icons(const QString &solutionHash);
    QDir shortcuts(const QString &solutionHash);
    QDir documents(const QString &solutionHash);
    QDir wine(const QString &solutionHash);
    QDir packages(const QString &solutionHash);
    QDir windows(const QString &solutionHash);
    QDir sys32(const QString &solutionHash);
    QDir users(const QString &solutionHash);
    QDir user(const QString &solutionHash);
    QDir links(const QString &solutionHash);

    QString readFile(const QString &filePath);
    void browse(const QString &path);
    QString hash(const QString &solutionName);
    QString md5(const QString &filePath);
    QString pathWithDrive(const QString &solutionHash, const QString &path);

    void removeSolution(const QString &solutionHash);
}

#endif // FILESYSTEM_H
