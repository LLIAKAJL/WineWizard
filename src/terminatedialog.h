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

#ifndef TERMINATEDIALOG_H
#define TERMINATEDIALOG_H

#include <QAbstractItemModel>
#include <QDialog>

namespace Ui {
class TerminateDialog;
}

class TerminateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TerminateDialog(QAbstractItemModel *model, QWidget *parent = nullptr);
    ~TerminateDialog() override;

public slots:
    void accept() override;

private slots:
    void on_prefixes_clicked(const QModelIndex &index);

    void on_buttonBox_helpRequested();

private:
    Ui::TerminateDialog *ui;
};

#endif // TERMINATEDIALOG_H
