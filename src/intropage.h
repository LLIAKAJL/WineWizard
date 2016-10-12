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

#ifndef INTROPAGE_H
#define INTROPAGE_H

#include <QAbstractItemModel>
#include <QWizardPage>

#include "mainmodel.h"

namespace Ui {
class IntroPage;
}

class IntroPage : public QWizardPage
{
    Q_OBJECT

    Q_PROPERTY(QModelIndex app READ app CONSTANT)
    Q_PROPERTY(QString exe READ exe CONSTANT)
    Q_PROPERTY(QString args READ args CONSTANT)
    Q_PROPERTY(QString workDir READ workDir CONSTANT)
    Q_PROPERTY(QModelIndex prefix READ prefix CONSTANT)

public:
    explicit IntroPage(const QString &exe, QAbstractItemModel *model, QWidget *parent = nullptr);
    ~IntroPage() override;

    bool isComplete() const override;
    int nextId() const override;

signals:
    void modelChanged();

private slots:
    void finished();
    void retry();
    void prefixChanged(const QModelIndex &index);
    void on_installRB_toggled(bool checked);
    void on_workDir_textChanged(const QString &path);
    void on_addBtn_clicked();
    void on_browseBtn_clicked();
    void on_leftBtn_clicked();
    void on_rightBtn_clicked();
    void on_searchBtn_clicked();
    void on_alter_linkActivated(const QString &link);
    void on_search_textChanged(const QString &);

private:
    Ui::IntroPage *ui;
    QString mExe;
    int mPageNum;

    const QString &exe() const;
    QString args() const;
    QString workDir() const;
    QModelIndex app() const;
    QModelIndex prefix() const;
    void search(int pagenum = 1);
    QString prepareSearch() const;
};

#endif // INTROPAGE_H
