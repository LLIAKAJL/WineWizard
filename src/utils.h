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

#ifndef UTILS_H
#define UTILS_H

#include <QWidget>
#include <QImage>
#include <QDir>

namespace Utils
{
    bool warning(const QString &message, QWidget *parent = nullptr);
    void error(const QString &message, QWidget *parent = nullptr);
    QString selectExe(const QString &dir, QWidget *parent = nullptr);
    QString selectIcon(const QString &dir, QWidget *parent = nullptr);
    QString selectDir(const QString &dir, QWidget *parent = nullptr);

    QDir temp();
    QDir data();
    QByteArray readFile(const QString &path);
    QJsonObject readJson(const QString &path);
    QImage extractIcon(const QString &path);

    QString genID();
}

#endif // UTILS_H
