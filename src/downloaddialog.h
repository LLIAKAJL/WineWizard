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

#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QNetworkAccessManager>

#include "netdialog.h"

namespace Ui {
class DownloadDialog;
}

class DownloadDialog : public NetDialog
{
    Q_OBJECT

public:
    explicit DownloadDialog(const QStringList &mirrors, const QString &outFile,
                            QWidget *parent = nullptr, const QString &checsum = QString(),
                            const QStringList &reList = QStringList());
    ~DownloadDialog() override;

    void reject() override;

private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void readyRead();
    void on_buttonBox_helpRequested();

private:
    Ui::DownloadDialog *ui;
    QString mOutFile, mCheckSum;
    QStringList mMirrors, mReList;

    void download();
    void retry();
};

#endif // DOWNLOADDIALOG_H
