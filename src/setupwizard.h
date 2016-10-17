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

#ifndef SETUPWIZARD_H
#define SETUPWIZARD_H

#include <QAbstractItemModel>
#include <QWizard>

namespace Ui {
class SetupWizard;
}

class SetupWizard : public QWizard
{
    Q_OBJECT

    typedef QList<SetupWizard *> InstanceList;

public:
    enum { PageIntro, PageSolution, PageInstall, PageDebug, PageFinal, PageUpdate };

    explicit SetupWizard(const QString &exe, const QString &args,
                         QAbstractItemModel *model, QWidget *parent = nullptr);
    ~SetupWizard() override;

    static bool running();

public slots:
    void reject() override;
    void setVisible(bool visible) override;

private slots:
    void hideBackButton();
    void on_SetupWizard_helpRequested();

private:
    static InstanceList mInstances;

    Ui::SetupWizard *ui;
    QAbstractItemModel *mModel;
};

#endif // SETUPWIZARD_H
