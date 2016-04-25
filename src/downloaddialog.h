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
#include <QNetworkReply>
#include <QDir>

#include "singletondialog.h"

namespace Ui {
class DownloadDialog;
}

class DownloadDialog : public SingletonDialog
{
    Q_OBJECT

    typedef QPair<QString, QString> Download;
    typedef QList<Download> DownloadList;

public:
    explicit DownloadDialog(const QString &url, QWidget *parent = nullptr);
    ~DownloadDialog() override;

public slots:
    void reject() override;

private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void otherProgress(qint64 bytesReceived, qint64 bytesTotal);
    void packageFinished();
    void solutionFinished();
    void updateFinished();
    void readyRead();

    void on_buttonBox_helpRequested();

private:
    Ui::DownloadDialog *ui;
    QNetworkAccessManager mNam;
    DownloadList mList;

    void getSolution(const QString &url);
    void getUpdate();
    void getPackage();
    void makeRequired();
};

#endif // DOWNLOADDIALOG_H
