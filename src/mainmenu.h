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

#ifndef MAINMENU_H
#define MAINMENU_H

#include <QFileInfoList>
#include <QMenu>

#include "singletonwidget.h"

class MainMenu : public QMenu, public SingletonWidget
{
    Q_OBJECT

public:
    enum { Empty, Install, Debug, Run, RunFile, Browse, Delete,
           Edit, Terminate, Settings, About, Help, Quit };

    explicit MainMenu(bool autoclose, const QStringList &runList, const QStringList &busyList, QWidget *parent = nullptr);

private:
    void addEmpty(QMenu *menu);
    QIcon getPrefixIcon(const QString &prefixHash) const;
    void sortList(QFileInfoList &list, bool prefix = true);
};

#endif // MAINMENU_H
