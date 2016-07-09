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

#include "winesortmodel.h"
#include "winemodel.h"

WineSortModel::WineSortModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool WineSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QString l = left.data().toString();
    QString r = right.data().toString();
    QStringList ll = l.split('.', QString::SkipEmptyParts);
    QStringList rl = r.split('.', QString::SkipEmptyParts);
    if (ll.first() == rl.first())
    {
        ll.removeFirst();
        rl.removeFirst();
        if (ll.first() == rl.first())
        {
            ll.removeFirst();
            rl.removeFirst();
            int li = ll.first().indexOf('-');
            int ri = rl.first().indexOf('-');
            QString lm = li > -1 ? ll.first().left(li) : ll.first();
            QString rm = ri > -1 ? rl.first().left(ri) : rl.first();
            if (lm == rm)
            {
                QString lm = li > -1 ? ll.first().remove(li + 1) : QString();
                QString rm = ri > -1 ? rl.first().remove(ri + 1) : QString();
                return lm > rm;
            }
            return lm.toInt() > rm.toInt();
        }
        return ll.first().toInt() > rl.first().toInt();
    }
    return ll.first().toInt() > rl.first().toInt();
}
