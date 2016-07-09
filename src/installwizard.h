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

#ifndef INSTALLWIZARD_H
#define INSTALLWIZARD_H

#include <QShowEvent>
#include <QHideEvent>
#include <QWizard>

namespace Ui {
class InstallWizard;
}

class InstallWizard : public QWizard
{
    Q_OBJECT

public:
    enum { PageIntro, PageSolution, PageInstall, PageWin, PageDebug };

    explicit InstallWizard(const QString &exe, QWidget *parent = nullptr);
    ~InstallWizard() override;

    static InstallWizard *instance();

signals:
    void windowShow();
    void windowHide();

public slots:
    void reject() override;

private slots:
    void on_InstallWizard_helpRequested();
    void pageChanged();

private:
    Ui::InstallWizard *ui;
    static InstallWizard *mInstance;
};

#endif // INSTALLWIZARD_H
