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

#ifndef EDITSHORTCUTDIALOG_H
#define EDITSHORTCUTDIALOG_H

#include <QSortFilterProxyModel>
#include <QDialog>
#include <QIcon>

#include "mainmodel.h"

namespace Ui {
class EditShortcutDialog;
}

class EditShortcutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditShortcutDialog(QAbstractItemModel *model, const QModelIndex &shortcut, QWidget *parent = nullptr);
    ~EditShortcutDialog() override;

public slots:
    void accept() override;

private slots:
    void on_icon_clicked();
    void on_workDirBrowseBtn_clicked();
    void on_targetBrowseBtn_clicked();
    void on_target_textChanged(const QString &target);
    void on_workDir_textChanged(const QString &workDir);
    void on_name_textChanged(const QString &name);
    void on_buttonBox_helpRequested();

private:
    Ui::EditShortcutDialog *ui;
    QSortFilterProxyModel *mModel;
    QModelIndex mShortcut;
    QString mIcon;

    QString preparePath(const QString &path) const;
};

#endif // EDITSHORTCUTDIALOG_H
