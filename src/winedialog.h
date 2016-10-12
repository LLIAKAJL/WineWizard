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

#ifndef WINEDIALOG_H
#define WINEDIALOG_H

#include <QDialog>

#include "winesortmodel.h"
#include "packagemodel.h"

namespace Ui {
class WineDialog;
}

class WineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WineDialog(const QString &current, PackageModel *model, QWidget *parent = nullptr);
    ~WineDialog() override;

    QString wine() const;

private slots:
    void on_list_clicked(const QModelIndex &index);

private:
    Ui::WineDialog *ui;
    WineSortModel *mModel;
};

#endif // WINEDIALOG_H
