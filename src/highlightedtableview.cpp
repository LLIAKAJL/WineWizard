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

#include "highlightedtableview.h"

HighlightedTableView::HighlightedTableView(QWidget *parent) :
    QTableView(parent)
{
    setItemDelegate(&mDelegate);
}

void HighlightedTableView::mousePressEvent(QMouseEvent *e)
{
    QTableView::mousePressEvent(e);
    processEvent();
}

void HighlightedTableView::mouseMoveEvent(QMouseEvent *e)
{
    QTableView::mouseMoveEvent(e);
    if (e->buttons() != Qt::NoButton)
        processEvent();
}

void HighlightedTableView::mouseReleaseEvent(QMouseEvent *e)
{
    QTableView::mouseReleaseEvent(e);
    processEvent();
}

void HighlightedTableView::keyPressEvent(QKeyEvent *e)
{
    QTableView::keyPressEvent(e);
    processEvent();
}

void HighlightedTableView::keyReleaseEvent(QKeyEvent *e)
{
    QTableView::keyReleaseEvent(e);
    processEvent();
}

void HighlightedTableView::processEvent()
{
    QModelIndex item = currentIndex();
    if (item.isValid())
    {
        QItemSelection s(item.sibling(item.row(), 0), item.sibling(item.row(), model()->columnCount() - 1));
        clearSelection();
        selectionModel()->select(s, QItemSelectionModel::Select);
    }
}
