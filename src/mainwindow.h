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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QPushButton>
#include <QMainWindow>
#include <QJsonObject>
#include <QHideEvent>
#include <QKeyEvent>
#include <QStyle>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void start(const QString &cmdLine);
    void setVisible(bool visible) override;

protected:
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    void updateFinished();
    void finished();
    void error();
    void checkState();
    void trayActivated();
    void rowsInserted(const QModelIndex &parent, int row);
    void on_actionQuit_triggered();
    void on_actionSettings_triggered();
    void on_deleteShortcutBtn_clicked();
    void on_deletePrefixBtn_clicked();
    void on_editShortcutBtn_clicked();
    void on_editPrefixBtn_clicked();
    void on_actionBrowse_triggered();
    void on_actionExecute_triggered();
    void on_actionTerminate_triggered();
    void on_actionControl_toggled(bool checked);
    void on_newShortcutBtn_clicked();
    void on_shortcuts_doubleClicked(const QModelIndex &index);
    void on_prefixes_doubleClicked(const QModelIndex &index);
    void on_actionInstall_triggered();
    void on_actionAbout_triggered();
    void on_actionHelp_triggered();

private:
    Ui::MainWindow *ui;

    void setActionIcon(QAction *action, const QString &name, QStyle::StandardPixmap alter);
    void setBtnIcon(QPushButton *button, const QString &iconName, QStyle::StandardPixmap alter);
    void getUpdate();
};

#endif // MAINWINDOW_H
