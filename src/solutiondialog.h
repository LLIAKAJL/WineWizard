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

#ifndef SOLUTIONDIALOG_H
#define SOLUTIONDIALOG_H

#include "installwizard.h"

namespace Ui {
class SolutionDialog;
}

class SolutionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SolutionDialog(QWizard *wizard, QWidget *parent = nullptr, bool debug = false);
    ~SolutionDialog() override;

public slots:
    void accept() override;

private slots:
    void setCategory(const QModelIndex &index);
    void on_buttonBox_helpRequested();
    void on_lockBtn_toggled(bool checked);
    void on_category_currentIndexChanged(int index);
    void on_solutions_currentIndexChanged(int index);
    void on_editBtn_clicked();
    void on_cancelBtn_clicked();
    void on_saveBtn_clicked();
    void on_aWine_currentIndexChanged(int index);
    void on_bWine_currentIndexChanged(int index);

private:
    Ui::SolutionDialog *ui;
    QWizard *mWizard;
    bool mDebug;

    void setEditable(bool editable);
};

#endif // SOLUTIONDIALOG_H
