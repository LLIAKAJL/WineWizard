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

#ifndef EDITSOLUTIONDIALOG_H
#define EDITSOLUTIONDIALOG_H

#include <QDialog>

namespace Ui {
class EditSolutionDialog;
}

enum { PT_PACKAGE = 0, PT_WINE, PT_HIDDEN };

class EditSolutionDialog : public QDialog
{
    Q_OBJECT

    struct SolutionData
    {
        QString bWine, aWine, slug;
        QStringList bPackages, aPackages;
    };

public:
    explicit EditSolutionDialog(const QString &arch, QWidget *parent = nullptr);
    ~EditSolutionDialog() override;

public slots:
    void accept() override;

private slots:
    void on_category_clicked();
    void on_bWine_clicked();
    void on_aWine_clicked();
    void on_bMoveWine_clicked();
    void on_aMoveWine_clicked();

private:
    Ui::EditSolutionDialog *ui;
    QString mArch;
    QStringList mWineList, mCategoryList;
    SolutionData mSolData;
};

#endif // EDITSOLUTIONDIALOG_H
