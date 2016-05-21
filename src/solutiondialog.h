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

#ifndef SOLUTIONDIALOG_H
#define SOLUTIONDIALOG_H

#include <QPushButton>
#include <QKeyEvent>

#include "singletondialog.h"

namespace Ui {
class SolutionDialog;
}

class SolutionDialog : public SingletonDialog
{
    Q_OBJECT

public:
    explicit SolutionDialog(const QStringList &runList, QWidget *parent = nullptr);
    ~SolutionDialog() override;

    QString slug() const;

public slots:
    void accept() override;

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void getSearch();
    void currentChanged(const QModelIndex &index);
    void on_buttonBox_helpRequested();
    void on_search_textChanged(const QString &search);
    void on_addBtn_clicked();
    void on_edit32Btn_clicked();
    void on_edit64Btn_clicked();
    void searchExecute();

    void on_viewBtn_clicked();

private:
    Ui::SolutionDialog *ui;
    int mCurrentPage;
    QStringList mRunList;

    bool getSolution(const QString &arch);
};

#endif // SOLUTIONDIALOG_H
