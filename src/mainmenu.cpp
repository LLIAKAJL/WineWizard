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

MainMenu::MainMenu(const QStringList &runList, const QStringList &busyList, QWidget *parent) :
    QMenu(parent),
    SingletonWidget(this)
{
    move(QCursor::pos());
    QAction *act = addAction(style()->standardIcon(QStyle::SP_DriveCDIcon), tr("Install Application"));
    act->setData(Install);
    act->setEnabled(busyList.isEmpty());
    addSeparator();
    QFileInfoList pList = FS::data().entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    if (pList.isEmpty())
        addEmpty(this);
    else
    {
        sortList(pList);
        for (const QFileInfo &pInfo : pList)
        {
            QString hash = pInfo.fileName();
            QFileInfoList sList = FS::shortcuts(hash).entryInfoList(QDir::Files);
            sortList(sList, false);
            QSettings s(FS::prefix(hash).absoluteFilePath(".settings"), QSettings::IniFormat);
            s.setIniCodec("UTF-8");
            QString name = s.value("Name").toString().replace('&', "&&");
            QMenu *pMenu = addMenu(getPrefixIcon(hash), name);
            bool run = runList.contains(hash);
            bool busyOrRun = busyList.contains(hash) || run;
            if (sList.isEmpty())
                addEmpty(pMenu);
            else
            {
                QDir iDir = FS::icons(hash);
                for (const QFileInfo &shortcut : sList)
                {
                    QString base = shortcut.fileName();
                    QIcon icon = iDir.exists(base) ? QIcon(iDir.absoluteFilePath(base)) :
                                                     style()->standardIcon(QStyle::SP_FileLinkIcon);
                    QSettings s(shortcut.absoluteFilePath(), QSettings::IniFormat);
                    s.setIniCodec("UTF-8");
                    QString name = s.value("Name").toString().replace('&', "&&");
                    act = pMenu->addAction(icon, name);
                    act->setProperty("PrefixHash", hash);
                    act->setProperty("Exe", FS::toUnixPath(hash, s.value("Exe").toString()));
                    act->setProperty("WorkDir", FS::toUnixPath(hash, s.value("WorkDir").toString()));
                    act->setProperty("Args", s.value("Args").toString());
                    if (s.value("Debug", true).toBool())
                    {
                        act->setData(Debug);
                        act->setProperty("Shortcut", shortcut.absoluteFilePath());
                    }
                    else
                        act->setData(Run);
                }
            }
            pMenu->addSeparator();
            QMenu *ccMenu = pMenu->addMenu(style()->standardIcon(QStyle::SP_ComputerIcon), tr("Control Center"));
            act = ccMenu->addAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Run File"));
            act->setProperty("PrefixHash", hash);
            act->setData(RunFile);
            act = ccMenu->addAction(style()->standardIcon(QStyle::SP_DirOpenIcon), tr("Browse"));
            act->setProperty("PrefixHash", hash);
            act->setData(Browse);
            ccMenu->addSeparator();
            act = ccMenu->addAction(style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Edit"));
            act->setProperty("PrefixHash", hash);
            act->setData(Edit);
            act->setDisabled(busyOrRun);
            ccMenu->addSeparator();
            act = ccMenu->addAction(style()->standardIcon(QStyle::SP_TrashIcon), tr("Delete"));
            act->setProperty("PrefixName", name);
            act->setProperty("PrefixHash", hash);
            act->setData(Delete);
            act->setDisabled(busyOrRun);
            pMenu->addSeparator();
            act = pMenu->addAction(style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Terminate"));
            act->setProperty("PrefixName", name);
            act->setProperty("PrefixHash", hash);
            act->setData(Terminate);
            act->setEnabled(run);
        }
    }
    addSeparator();
    act = addAction(style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Settings"));
    act->setData(Settings);
    act->setEnabled(busyList.isEmpty() && runList.isEmpty());
    addSeparator();
    act = addAction(QIcon::fromTheme("winewizard"), tr("About"));
    act->setData(About);
    act = addAction(style()->standardIcon(QStyle::SP_DialogHelpButton), tr("Help"));
    act->setData(Help);
    addSeparator();
    act = addAction(style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Quit"));
    act->setData(Quit);
}

void MainMenu::addEmpty(QMenu *menu)
{
    auto act = new QWidgetAction(menu);
    act->setText(tr("*** Empty ***"));
    menu->addAction(act);
}

QIcon MainMenu::getPrefixIcon(const QString &prefixHash) const
{
    QString iconPath = FS::prefix(prefixHash).absoluteFilePath(".icon");
    if (!QFile::exists(iconPath))
    {
        QFileInfoList iList = FS::icons(prefixHash).entryInfoList(QDir::Files);
        if (!iList.isEmpty())
        {
            QFile::copy(iList.first().absoluteFilePath(), iconPath);
            return QIcon(iconPath);
        }
        return style()->standardIcon(QStyle::SP_DirIcon);
    }
    return QIcon(iconPath);
}

void MainMenu::sortList(QFileInfoList &list, bool prefix)
{
    QMap<QString, QString> sortList;
    for (auto item : list)
    {
        auto hash = item.fileName();
        auto path = prefix ? FS::prefix(hash).absoluteFilePath(".settings") : item.absoluteFilePath();
        QSettings s(path, QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        sortList.insert(hash, s.value("Name").toString().replace('&', "&&"));
    }
    qSort(list.begin(), list.end(), [&sortList](const QFileInfo &l, const QFileInfo &r)
    {
        return sortList.value(l.fileName()) < sortList.value(r.fileName());
    });
}
