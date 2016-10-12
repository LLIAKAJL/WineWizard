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

#ifndef EDITSOLUTIONDIALOG_H
#define EDITSOLUTIONDIALOG_H

#include <QDialog>

#include "packagemodel.h"

namespace Ui {
class EditSolutionDialog;
}

class EditSolutionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSolutionDialog(const QModelIndex &solution, const QString &out,
                                const QString &err, QWidget *parent = nullptr);
    ~EditSolutionDialog() override;

public slots:
    void accept();

private slots:
    void on_lockBtn_toggled(bool checked);
    void on_bw_clicked();
    void on_aw_clicked();
    void on_buttonBox_helpRequested();
    void on_categories_currentIndexChanged(int index);

private:
    Ui::EditSolutionDialog *ui;
    int mRow;
    QAbstractItemModel *mModel;
    PackageModel *mWineModel;

    void setWine(QPushButton *button, const QVariant &wine);
};

#endif // EDITSOLUTIONDIALOG_H
