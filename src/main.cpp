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

#include <QDesktopServices>
#include <QFileInfo>
#include <QIcon>
#include <QDir>

#include "qtsingleapplication/QtSingleApplication"
#include "installwizard.h"
#include "wizard.h"

int main(int argc, char *argv[])
{
    QtSingleApplication app(argc, argv);
    if (app.isRunning())
    {
        switch (argc)
        {
        case 3:
            app.sendMessage(QString(argv[1]) + '\n' + argv[2]);
            return 0;
        case 2:
            app.sendMessage(QFileInfo(argv[1]).absoluteFilePath());
            return 0;
        default:
            app.sendMessage(QString());
            return 0;
        }
    }

    app.setQuitOnLastWindowClosed(false);
    app.setApplicationDisplayName("Wine Wizard");
    app.setApplicationName("winewizard");
    app.setApplicationVersion(APP_VERSION);

    Wizard w;
    w.connect(&app, &QtSingleApplication::messageReceived, &w, &Wizard::start);
    switch (argc)
    {
    case 3:
        w.start(QString(argv[1]) + '\n' + argv[2]);
        break;
    case 2:
        w.start(QFileInfo(argv[1]).absoluteFilePath());
        break;
    default:
        w.start();
        break;
    }

    return app.exec();
}
