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

#include <QWidgetAction>
#include <QSettings>
#include <QStyle>

#include "filesystem.h"
#include "mainmenu.h"

MainMenu::MainMenu(const QStringList &runList, QWidget *parent) :
    QMenu(parent),
    SingletonWidget(this)
{
    move(QCursor::pos());
    auto pList = FS::data().entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden, QDir::Name);
    if (pList.isEmpty())
        addEmpty(this);
    else
    {
        sortList(pList);
        for (auto solution : pList)
        {
            auto hash = solution.fileName();
            auto sList = FS::shortcuts(hash).entryInfoList(QDir::Files | QDir::Hidden, QDir::Name);
            sortList(sList, false);
            QSettings s(FS::solution(solution.fileName()).absoluteFilePath(".settings"), QSettings::IniFormat);
            s.setIniCodec("UTF-8");
            auto pMenu = addMenu(getSolutionIcon(hash, sList), s.value("Name").toString().replace('&', "&&"));
            if (sList.isEmpty())
                addEmpty(pMenu);
            else
            {
                for (auto shortcut : sList)
                {
                    auto base = shortcut.fileName();
                    QFileInfo iconPath = FS::icons(hash).absoluteFilePath(base);
                    auto icon = iconPath.exists() ? QIcon(iconPath.absoluteFilePath()) : style()->standardIcon(QStyle::SP_FileLinkIcon);
                    QSettings s(shortcut.absoluteFilePath(), QSettings::IniFormat);
                    s.setIniCodec("UTF-8");
                    auto act = pMenu->addAction(icon, s.value("Name").toString().replace('&', "&&"));
                    act->setProperty("Solution", hash);
                    act->setProperty("Exe", s.value("Exe").toString());
                    act->setData(Shortcut);
                }
            }
            pMenu->addSeparator();
            auto act = pMenu->addAction(style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Terminate"));
            act->setData(Terminate);
            act->setProperty("Solution", hash);
            act->setEnabled(runList.contains(hash));
        }
    }
    addSeparator();
    auto act = addAction(style()->standardIcon(QStyle::SP_ComputerIcon), tr("Control Center"));
    act->setData(Control);
    addSeparator();
    act = addAction(QIcon::fromTheme("winewizard"), tr("About"));
    act->setData(About);
    act = addAction(style()->standardIcon(QStyle::SP_DialogHelpButton), tr("Help"));
    act->setData(Help);
}

void MainMenu::addEmpty(QMenu *menu)
{
    auto act = new QWidgetAction(menu);
    act->setData(Empty);
    act->setText(tr("*** Empty ***"));
    menu->addAction(act);
}

QIcon MainMenu::getSolutionIcon(const QString &solution, const QFileInfoList &list) const
{
    auto iconPath = FS::solution(solution).absoluteFilePath(".icon");
    if (!QFile::exists(iconPath))
    {
        auto iDir = FS::icons(solution);
        for (auto shortcut : list)
        {
            if (iDir.exists(shortcut.fileName()))
            {
                QFile::copy(iDir.absoluteFilePath(shortcut.fileName()), iconPath);
                return QIcon(iconPath);
            }
        }
        return style()->standardIcon(QStyle::SP_DirIcon);
    }
    return QIcon(iconPath);
}

void MainMenu::sortList(QFileInfoList &list, bool solution)
{
    QMap<QString, QString> sortList;
    for (auto item : list)
    {
        auto hash = item.fileName();
        auto path = solution ? FS::solution(hash).absoluteFilePath(".settings") : item.absoluteFilePath();
        QSettings s(path, QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        sortList.insert(hash, s.value("Name").toString().replace('&', "&&"));
    }
    qSort(list.begin(), list.end(), [&sortList](const QFileInfo &l, const QFileInfo &r)
    {
        return sortList.value(l.fileName()) < sortList.value(r.fileName());
    });
}
