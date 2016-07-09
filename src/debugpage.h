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

#ifndef DEBUGPAGE_H
#define DEBUGPAGE_H

#include <QWizardPage>
#include <QModelIndex>

namespace Ui {
class DebugPage;
}

class DebugPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit DebugPage(QWidget *parent = nullptr);
    ~DebugPage() override;

    void initializePage() override;
    int nextId() const override;
    bool isComplete() const override;
    bool validatePage() override;

private slots:
    void finished();
    void readyOutput(const QString &text);
    void readyError(const QString &text);

private:
    Ui::DebugPage *ui;

    void uncheckButtons();
    void appendOut(const QString &text);
};

#endif // DEBUGPAGE_H
