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

#include <QWidget>

namespace Ex
{
    typedef QPair<QString, QString> Out;
    void release(const QString &script, const QString &prefixHash = QString());
    Out debug(const QString &script, const QString &prefixHash = QString());
    void wait(const QString &script, const QString &prefixHash = QString(), QWidget *parent = nullptr);
    void terminal(const QString &script, const QString &prefixHash = QString(), QWidget *parent = nullptr);
}

#endif // EXECUTOR_H
