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

#include <QFileInfo>
#include <QIcon>

#include "qtsingleapplication/QtSingleApplication"
#include "wizard.h"

int main(int argc, char *argv[])
{
    QtSingleApplication app(argc, argv);
    if (app.isRunning())
    {
        app.sendMessage(argc > 1 ? argv[1] : QString());
        return 0;
    }
    app.setApplicationDisplayName("Wine Wizard");
    app.setApplicationName("winewizard");
    app.setApplicationVersion(APP_VERSION);
    Wizard w;
    app.connect(&app, &QtSingleApplication::messageReceived, &w, &Wizard::start);
    w.start(argc > 1 ? argv[1] : QString());
    return 0;
}
