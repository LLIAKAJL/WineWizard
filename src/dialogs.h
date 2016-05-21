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

#ifndef DIALOGS_H
#define DIALOGS_H

#include <QMessageBox>
#include <QDir>

#include "singletonwidget.h"

namespace Dialogs
{
    enum FinishStatus { Yes, No, Finish };

    class MessageDialog : public QMessageBox, public SingletonWidget
    {
        Q_OBJECT

        friend void error(const QString &text, QWidget *parent);
        friend bool confirm(const QString &text, QWidget *parent);
        friend bool retry(const QString &text, QWidget *parent);
        friend bool finish(const QString &prefixHash, QWidget *parent);

    protected:
        explicit MessageDialog(Icon icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QWidget *parent = nullptr);
    };

    //--------------------------------------------------------------------------------------------

    void error(const QString &text, QWidget *parent = nullptr);
    bool confirm(const QString &text, QWidget *parent = nullptr);
    bool retry(const QString &text, QWidget *parent = nullptr);
    bool finish(const QString &prefixHash, QWidget *parent = nullptr);
    QString open(const QString &title, const QString &filter, QWidget *parent = nullptr, const QString &dir = QDir::homePath());
    QString selectDir(const QString &start, QWidget *parent = nullptr);
}

#endif // DIALOGS_H
