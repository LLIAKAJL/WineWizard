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

#include <QDesktopServices>
#include <QMovie>
#include <QUrl>

#include "adslabel.h"

AdsLabel::AdsLabel(const QString &moviePath, const QString &url, int height, QWidget *parent) :
    QLabel(parent),
    mMoviePath(moviePath),
    mUrl(url)
{
    setMinimumSize(1,1);
    setMaximumHeight(height);
    setScaledContents(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setCursor(Qt::PointingHandCursor);
    setMovie(new QMovie(this));
    setMouseTracking(true);
    movie()->setFileName(mMoviePath);
    setMovie(movie());
    movie()->start();
}

AdsLabel::~AdsLabel()
{
    if (QFile::exists(mMoviePath))
        QFile::remove(mMoviePath);
}

QSize AdsLabel::sizeHint() const
{
    return QSize(width(), height());
}

const QString &AdsLabel::url() const
{
    return mUrl;
}

void AdsLabel::mousePressEvent(QMouseEvent *)
{
    QDesktopServices::openUrl(mUrl);
}
