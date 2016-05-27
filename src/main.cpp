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
    QString cmdLine = list.isEmpty() ? QString() : QFileInfo(list.takeFirst()).absoluteFilePath();
    if (!cmdLine.isEmpty())
         cmdLine += '\n' + QDir::currentPath() + '\n' + list.join('\n');
    if (app.isRunning())
    {
        app.sendMessage(cmdLine);
        return 0;
    }
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationDisplayName("Wine Wizard");
    app.setApplicationName("winewizard");
    app.setApplicationVersion(APP_VERSION);
    bool trayVisible, autoclose;
    {
        QSettings s("winewizard", "settings");
        s.beginGroup("Tray");
        trayVisible = s.value("Visible", false).toBool();
        autoclose = s.value("Autoclose", true).toBool();
        s.endGroup();
    }
    Wizard w(trayVisible, autoclose);
    app.connect(&app, &QtSingleApplication::messageReceived, &w, &Wizard::start);
    w.start(cmdLine);
    if (!trayVisible || autoclose || app.property("quit").toBool())
        return 0;
    else
        return app.exec();
}
