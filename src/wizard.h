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

#ifndef WIZARD_H
#define WIZARD_H

#include <QSystemTrayIcon>
#include <QTranslator>
#include <QFileInfo>
#include <QObject>

class Wizard : public QObject
{
    Q_OBJECT

public:
    enum { LANG_EN, LANG_RU };

    explicit Wizard(QObject *parent = nullptr);

    static void update();
    static bool autoquit();
    static void setAutoquit(bool autoquit);
    static void retranslate(int lang);
    static Wizard *instance();

public slots:
    void start(const QString &cmdLine = QString());

private slots:
    void trayActivated(QSystemTrayIcon::ActivationReason reason);

private:
    static Wizard *mInstance;
    static bool mAutoquit;
    static QTranslator *mStdTrans, *mTrans;

    bool testSuffix(const QFileInfo &path) const;
    void executeShortcut(const QString &prefix, const QString &shortcut);
    static QString langSuffix(int lang);
};

#endif // WIZARD_H
