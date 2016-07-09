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

#ifndef DIALOGS_H
#define DIALOGS_H

#include <QInputDialog>
#include <QMessageBox>
#include <QDir>

namespace Dialogs
{
    void error(const QString &text, QWidget *parent = nullptr);
    bool confirm(const QString &text, QWidget *parent = nullptr);
    bool question(const QString &text, QWidget *parent = nullptr);
    bool retry(const QString &text, QWidget *parent = nullptr);
    QString open(const QString &title, const QString &filter,
                 QWidget *parent = nullptr, const QString &dir = QDir::homePath());
    QString dir(const QString &start, QWidget *parent = nullptr);
    bool getText(const QString &title, const QString &label,
                 QString &text, QWidget *parent = nullptr);
}

#endif // DIALOGS_H
