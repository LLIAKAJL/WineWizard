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

#ifndef EDITPREFIXDIALOG_H
#define EDITPREFIXDIALOG_H

#include "singletondialog.h"

namespace Ui {
class EditPrefixDialog;
}

class EditPrefixDialog : public SingletonDialog
{
    Q_OBJECT

public:
    explicit EditPrefixDialog(const QString &prefixHash, QWidget *parent = nullptr);
    ~EditPrefixDialog() override;

public slots:
    void accept() override;

private slots:
    void on_name_textChanged(const QString &name);
    void on_icon_clicked();
    void currentChanged(const QModelIndex &index);
    void on_deleteBtn_clicked();
    void on_addBtn_clicked();
    void on_editBtn_clicked();

    void on_setIconBtn_clicked();

private:
    Ui::EditPrefixDialog *ui;
    QString mPrefixName, mPrefixHash, mIcon;
    QStringList mPrefixList;

    bool exists(const QString &name) const;
};

#endif // EDITPREFIXDIALOG_H
