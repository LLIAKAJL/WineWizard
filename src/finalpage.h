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

#ifndef FINALPAGE_H
#define FINALPAGE_H

#include <QWizardPage>

#include "netmanager.h"

namespace Ui {
class FinalPage;
}

class FinalPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit FinalPage(QWidget *parent = nullptr);
    ~FinalPage() override;

    void initializePage() override;
    bool isComplete() const override;
    int nextId() const override;

private slots:
    void finished();

private:
    Ui::FinalPage *ui;
    bool mFinished;
    NetManager mNetMgr;
};

#endif // FINALPAGE_H
