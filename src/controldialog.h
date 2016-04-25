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

#ifndef CONTROLDIALOG_H
#define CONTROLDIALOG_H

#include "singletondialog.h"

namespace Ui {
class ControlDialog;
}

class ControlDialog : public SingletonDialog
{
    Q_OBJECT

public:
    explicit ControlDialog(const QStringList &busiList, const QStringList &runList, QWidget *parent = nullptr);
    ~ControlDialog() override;

private slots:
    void shortcutChanged(const QModelIndex &index);
    void on_deleteShortcutBtn_clicked();
    void on_addBtn_clicked();
    void on_deleteSolutionBtn_clicked();
    void on_browseBtn_clicked();
    void on_shortcutSettingsBtn_clicked();
    void on_solutionSettingsBtn_clicked();
    void on_solutions_currentIndexChanged(int index);

    void on_buttonBox_helpRequested();

private:
    Ui::ControlDialog *ui;
    QStringList mBusiList, mRunList;
};

#endif // CONTROLDIALOG_H
