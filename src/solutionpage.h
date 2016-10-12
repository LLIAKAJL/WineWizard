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

#ifndef SOLUTIONPAGE_H
#define SOLUTIONPAGE_H

#include <QStyledItemDelegate>
#include <QItemSelection>
#include <QWizardPage>

namespace Ui {
class SolutionPage;
}

class SolutionPage : public QWizardPage
{
    Q_OBJECT

    Q_PROPERTY(QModelIndex solution READ solution CONSTANT)
    Q_PROPERTY(QString arch READ arch CONSTANT)

public:
    explicit SolutionPage(const QString &exe, QWidget *parent = nullptr);
    ~SolutionPage() override;

    void initializePage() override;
    bool isComplete() const;
    int nextId() const override;

private slots:
    void updateData();
    void finished();
    void currentChanged(const QModelIndex &index);
    void on_copyBtn_clicked();
    void on_editBtn_clicked();
    void on_result_doubleClicked(const QModelIndex &index);

private:
    Ui::SolutionPage *ui;

    QString arch() const;
    QModelIndex solution() const;
};

#endif // SOLUTIONPAGE_H
