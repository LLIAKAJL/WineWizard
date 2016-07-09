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

#ifndef INTROPAGE_H
#define INTROPAGE_H

#include <QWizardPage>

namespace Ui {
class IntroPage;
}

class IntroPage : public QWizardPage
{
    Q_OBJECT

    Q_PROPERTY(QString arch READ arch)
    Q_PROPERTY(QString exe READ exe)
    Q_PROPERTY(QString search READ search)

public:
    explicit IntroPage(const QString &exe, QWidget *parent = nullptr);
    ~IntroPage() override;

    int nextId() const override;
    bool isComplete() const override;
    bool validatePage() override;

private slots:
    void on_browseBtn_clicked();
    void on_newRB_toggled(bool checked);

private:
    Ui::IntroPage *ui;
    QString mExe;

    QString arch() const;
    const QString &exe() const;
    QString search() const;
};

#endif // INTROPAGE_H
