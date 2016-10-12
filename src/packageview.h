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

#ifndef PACKAGEVIEW_H
#define PACKAGEVIEW_H

#include <QProxyStyle>
#include <QListView>
#include <QPainter>

#include "delegates.h"

class PackageView : public QListView
{
    Q_OBJECT

    class DropIndicatorStyle : public QProxyStyle
    {
    public:
        void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                           QPainter *painter, const QWidget *widget) const override;
    };

    friend class DropIndicatorStyle;

public:
    explicit PackageView(QWidget *parent = nullptr);

private:
    PackageDelegate mDelegate;
    DropIndicatorStyle mStyle;
};

#endif // PACKAGEVIEW_H
