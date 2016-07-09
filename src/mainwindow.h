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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QKeyEvent>
#include <QFileInfo>
#include <QLabel>

namespace Ui {
class MainWindow;
}

const QString DOWNLOAD_URL = "http://wwizard.net/download/";

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    static MainWindow *instance();

    static void update();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void changeEvent(QEvent *e) override;
    bool eventFilter(QObject *target, QEvent *event) override;

private slots:
    void prefixChanged(const QModelIndex &index);
    void shortcutChanged(const QModelIndex &index);
    void winecfg();
    void regedit();
    void setIconFromShortcut();
    void runShortcut();
    void setIconFromFile();
    void deleteApp();
    void renameApp();
    void renameShortcut();
    void deleteShortcut();
    void editShortcutIcon();
    void editArgs();
    void editWorkDir();
    void addShortcut();
    void on_actionInstall_triggered();
    void on_prefixes_customContextMenuRequested(const QPoint &pos);
    void on_shortcuts_customContextMenuRequested(const QPoint &pos);
    void on_actionSettings_triggered();
    void on_actionBrowse_triggered();
    void on_actionTerminate_triggered();
    void on_actionTerminateAll_triggered();
    void on_actionQuit_triggered();
    void updateStatusBar();

private:
    Ui::MainWindow *ui;
    QMenu *mMenu;
    static MainWindow *mInstance;
    QLabel *mNewVersion;
//    void addMenuEmpty(QMenu *menu);
};

#endif // MAINWINDOW_H
