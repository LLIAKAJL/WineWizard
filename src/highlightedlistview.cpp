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

#include "highlightedlistview.h"

HighlightedListView::HighlightedListView(QWidget *parent) :
    QListView(parent)
{
    setItemDelegate(&mDelegate);
}

void HighlightedListView::mousePressEvent(QMouseEvent *e)
{
    QListView::mousePressEvent(e);
    processEvent();
}

void HighlightedListView::mouseMoveEvent(QMouseEvent *e)
{
    QListView::mouseMoveEvent(e);
    if (e->buttons() != Qt::NoButton)
        processEvent();
}

void HighlightedListView::mouseReleaseEvent(QMouseEvent *e)
{
    QListView::mouseReleaseEvent(e);
    processEvent();
}

void HighlightedListView::keyPressEvent(QKeyEvent *e)
{
    QListView::keyPressEvent(e);
    processEvent();
}

void HighlightedListView::keyReleaseEvent(QKeyEvent *e)
{
    QListView::keyReleaseEvent(e);
    processEvent();
}

void HighlightedListView::processEvent()
{
    QModelIndex item = currentIndex();
    if (item.isValid())
    {
        clearSelection();
        selectionModel()->select(item, QItemSelectionModel::Select);
    }
}
