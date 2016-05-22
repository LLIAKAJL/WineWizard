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
#include <QDir>

#include "qtsingleapplication/QtSingleApplication"
#include "wizard.h"

int main(int argc, char *argv[])
{
    QtSingleApplication app(argc, argv);
    QStringList list = app.arguments();
    list.removeFirst();
    if (app.isRunning())
    {
        if (list.isEmpty())
            app.sendMessage(QString());
        else
        {
            QString exe = QFileInfo(list.takeFirst()).absoluteFilePath();
            app.sendMessage(exe + '\n' + QDir::currentPath() + '\n' + list.join('\n'));
        }
        return 0;
    }
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationDisplayName("Wine Wizard");
    app.setApplicationName("winewizard");
    app.setApplicationVersion(APP_VERSION);
    Wizard w;
    app.connect(&app, &QtSingleApplication::messageReceived, &w, &Wizard::start);
    if (list.isEmpty())
        w.start(QString());
    else
    {
        QString exe = QFileInfo(list.takeFirst()).absoluteFilePath();
        if (w.start(exe + '\n' + QDir::currentPath() + '\n' + list.join('\n')))
            return 0;
    }
    return app.exec();
}
