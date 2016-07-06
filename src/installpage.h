/***************************************************************************
 *   Copyright (C) 2016 by Vitalii Kachemtsev <LLIAKAJL@yandex.ru>         *
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

#include <QWizardPage>

#include "repository.h"

namespace Ui {
class InstallPage;
}

class InstallPage : public QWizardPage
{
    Q_OBJECT

    Q_PROPERTY(QString out READ out WRITE setOut)
    Q_PROPERTY(QString err READ err WRITE setErr)

public:
    explicit InstallPage(QWidget *parent = nullptr);
    ~InstallPage() override;

    void initializePage() override;
    int nextId() const override;
    bool isComplete() const override;
    bool validatePage() override;

private slots:
    void bsFinished(int code);
    void createFinished(int code);
    void installFinished(int code);
    void install();
    void setupFinished();
    void readyLogOutput(const QByteArray &text);
    void readyLogError(const QByteArray &text);
    void readyOutput(const QByteArray &text);
    void readyError(const QByteArray &text);

private:
    Ui::InstallPage *ui;
    QString mBs, mAcs, mAs, mOut, mErr;

    const QString &out() const;
    const QString &err() const;
    void setOut(const QString &text);
    void setErr(const QString &text);
    void appendOut(const QString &text);
    void installationFailed();
};

#endif // INSTALLPAGE_H
