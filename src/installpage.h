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

#ifndef INSTALLPAGE_H
#define INSTALLPAGE_H

#include <QSortFilterProxyModel>
#include <QWizardPage>

#include "solutionpage.h"

namespace Ui {
class InstallPage;
}

class InstallPage : public QWizardPage
{
    Q_OBJECT

    Q_PROPERTY(QString out READ out CONSTANT)
    Q_PROPERTY(QString err READ err CONSTANT)

public:
    explicit InstallPage(QAbstractItemModel *model, QWidget *parent = nullptr);
    ~InstallPage() override;

    void initializePage() override;
    bool isComplete() const override;
    int nextId() const override;

private slots:
    void dataChanged(const QModelIndex &index);
    void readyOutput(const QString &path, const QString &data);
    void readyError(const QString &path, const QString &data);

private:
    Ui::InstallPage *ui;
    QSortFilterProxyModel *mModel;
    QString mOut, mErr;

    void appendOut(const QString &text);
    const QString &out() const;
    const QString &err() const;
};

#endif // INSTALLPAGE_H
