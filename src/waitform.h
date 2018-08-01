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

#ifndef WAITFORM_H
#define WAITFORM_H

#include <QWidget>
#include <QStyle>

namespace Ui {
class WaitForm;
}

class WaitForm : public QWidget
{
    Q_OBJECT

public:
    explicit WaitForm(QWidget *parent = nullptr);
    ~WaitForm() override;

signals:
    void retry();

public slots:
    void start();
    void error(int code);
    void readyOutput(const QString &data);
    void readyError(const QString &data);

private:
    Ui::WaitForm *ui;

    void appendOut(const QString &text);
};

#endif // WAITFORM_H
