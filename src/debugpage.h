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

#ifndef DEBUGPAGE_H
#define DEBUGPAGE_H

#include <QSortFilterProxyModel>
#include <QWizardPage>

namespace Ui {
class DebugPage;
}

class DebugPage : public QWizardPage
{
    Q_OBJECT

    Q_PROPERTY(QModelIndex shortcut READ shortcut CONSTANT)
    Q_PROPERTY(bool debug READ debug CONSTANT)

public:
    explicit DebugPage(QAbstractItemModel *model, QWidget *parent = nullptr);
    ~DebugPage() override;

    void initializePage() override;
    bool validatePage() override;
    int nextId() const override;

private slots:
    void edit();
    void currentChanged(const QModelIndex &index);
    void on_newBtn_clicked();
    void on_delBtn_clicked();
    void on_shortcuts_doubleClicked(const QModelIndex &index);

private:
    Ui::DebugPage *ui;
    QSortFilterProxyModel *mModel;

    QModelIndex shortcut() const;
    bool debug() const;
};

#endif // DEBUGPAGE_H
