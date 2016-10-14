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

#include <QDesktopServices>
#include <QApplication>
#include <QTimerEvent>
#include <QPainter>
#include <QUrl>

#include "ticker.h"

Ticker::Ticker(const QString &text, const QString &url, QWidget *parent) :
    QWidget(parent),
    mText(text),
    mUrl(url),
    mOffset(0),
    mTimerId(0)
{
    QFont f = QApplication::font();
    f.setUnderline(true);
    setFont(f);
    QPalette p = QApplication::palette();
    p.setColor(QPalette::WindowText, p.color(QPalette::Link));
    setPalette(p);
    setCursor(Qt::PointingHandCursor);
}

QSize Ticker::sizeHint() const
{
    return fontMetrics().size(0, mText);
}

void Ticker::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int x = -mOffset;
    while (x < width())
    {
        painter.drawText(x, 0, width(), height(), Qt::AlignLeft | Qt::AlignVCenter, mText);
        x += width();
    }
}

void Ticker::showEvent(QShowEvent *)
{
    mTimerId = startTimer(30);
}

void Ticker::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == mTimerId)
    {
        ++mOffset;
        if (mOffset >= width())
            mOffset = 0;
        scroll(-1, 0);
    }
    else
        QWidget::timerEvent(event);
}

void Ticker::hideEvent(QHideEvent *)
{
    killTimer(mTimerId);
    mTimerId = 0;
}

void Ticker::mousePressEvent(QMouseEvent *)
{
    if (!mUrl.isEmpty())
        QDesktopServices::openUrl(QUrl(mUrl));
}
