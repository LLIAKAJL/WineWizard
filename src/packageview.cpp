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

#include "packageview.h"

PackageView::PackageView(QWidget *parent) :
    QListView(parent)
{
    setItemDelegate(&mDelegate);
    setStyle(&mStyle);
}

//------------------------------------------------------------------------------------------

void PackageView::DropIndicatorStyle::drawPrimitive(QStyle::PrimitiveElement element,
                           const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == QStyle::PE_IndicatorItemViewItemDrop)
    {
        painter->setPen(QPen(option->palette.highlight().color(), 6));
        const PackageView *v = static_cast<const PackageView *>(const_cast<QWidget *>(widget));
        DropIndicatorPosition dip = v->dropIndicatorPosition();
        QRect drawRect = option->rect;
        if (v->indexAt(v->mapFromGlobal(QCursor::pos())).isValid())
        {
            if (dip == AboveItem)
                drawRect.setY(drawRect.y() - 1);
            else if (dip == BelowItem)
                drawRect.setY(drawRect.y() + 1);
        }
        else if (v->model())
        {
            int count = v->model()->rowCount();
            QStyleOptionViewItem opt;
            int itemHeight = v->itemDelegate()->sizeHint(opt, QModelIndex()).height();
            int y = count * itemHeight - 1;
            drawRect.setTopLeft(QPoint(0, y));
            drawRect.setBottomRight(QPoint(v->width(), y));
        }
        painter->drawRect(drawRect);
    }
    else
        QProxyStyle::drawPrimitive(element, option, painter, widget);
}
