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

#ifndef SOLUTIONPAGE_H
#define SOLUTIONPAGE_H

#include <QWizardPage>
#include <QModelIndex>

#include "repository.h"

namespace Ui {
class SolutionPage;
}

class SolutionPage : public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(QString sha READ sha)
    Q_PROPERTY(QModelIndex app READ app)
    Q_PROPERTY(Repository *repository READ repository)

public:
    explicit SolutionPage(QWidget *parent = nullptr);
    ~SolutionPage() override;

    void initializePage();
    bool validatePage() override;
    bool isComplete() const override;
    int nextId() const override;

private slots:
    void updateFinished(int code);
    void repoFinished(int code);
    void searchFinished(int code);
    void solutonsFinished(int code);
    void solutonsEditFinished(int code);
    void addFinished(int code);
    void on_addBtn_clicked();
    void on_leftBtn_clicked();
    void on_rightBtn_clicked();
    void on_solutionsBtn_clicked();
    void on_apps_doubleClicked(const QModelIndex &index);
    void readyOutput(const QByteArray &text);
    void readyError(const QByteArray &text);

private:
    Ui::SolutionPage *ui;
    Repository mRepository;
    QString mSha;
    int mPage, mPageCount;

    void getSearch(int page = 1);
    void getSolutions(bool next = true);
    Repository *repository();
    QModelIndex app() const;
    void clearRepository() const;
    QString sha() const;

    void appendOut(const QString &text);
};

#endif // SOLUTIONPAGE_H
