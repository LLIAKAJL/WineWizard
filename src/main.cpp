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

#include <QFileInfo>

#include "qtsingleapplication/QtSingleApplication"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QtSingleApplication a(argc, argv);
    QString cmdLine;
    QStringList args = a.arguments();
    args.removeFirst();
    if (!args.isEmpty())
    {
        QString target = args.takeFirst();
        QFileInfo info(target);
        cmdLine += (info.exists() ? info.absoluteFilePath() : target) + '\n';
    }
    if (!args.isEmpty())
        cmdLine += args.join(QChar::Space);
    if (a.isRunning())
        a.sendMessage(cmdLine);
    else
    {
        a.setQuitOnLastWindowClosed(false);
        a.setWindowIcon(QIcon(":/images/main"));
        a.setApplicationDisplayName("Wine Wizard");
        a.setApplicationName("winewizard");
        a.setApplicationVersion(VERSION);
        MainWindow w;
        a.connect(&a, &QtSingleApplication::messageReceived, &w, &MainWindow::start);
        w.start(cmdLine);
        return a.exec();
    }
    return 0;
}
