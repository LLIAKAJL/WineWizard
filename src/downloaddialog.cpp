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

#include <QDesktopServices>

#include "ui_downloaddialog.h"
#include "downloaddialog.h"
#include "filesystem.h"
#include "dialogs.h"

DownloadDialog::DownloadDialog(const QStringList &mirrors, const QString &outFile,
                               QWidget *parent, const QString &checsum,
                               const QStringList &reList) :
    NetDialog(parent),
    ui(new Ui::DownloadDialog),
    mOutFile(outFile),
    mCheckSum(checsum),
    mMirrors(mirrors),
    mReList(reList)
{
    ui->setupUi(this);
    if (mReList.isEmpty())
        for (int i = mMirrors.count() - 1; i >= 0; --i)
            mReList.append(QString());
    download();
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

void DownloadDialog::reject()
{
    if (Dialogs::confirm(tr("Are you sure you want to cancel all downloads?"), this))
        QDialog::reject();
}

void DownloadDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesReceived);
}

void DownloadDialog::downloadFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() == QNetworkReply::NoError)
    {
        QString redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        if (redirect.isEmpty())
        {
            if (mReList.first().isEmpty())
            {
                if (!mCheckSum.isEmpty() && !FS::checkFileSum(mOutFile, mCheckSum))
                {
                    QString errMsg = tr("Invalid checksum for file!");
                    if (Dialogs::retry(errMsg, this))
                        retry();
                    else
                        QDialog::reject();
                }
                else
                {
                    mMirrors.removeFirst();
                    mReList.removeFirst();
                    accept();
                }
            }
            else
            {
                QString data = FS::readFile(mOutFile);
                QRegExp re(mReList.first());
                re.setMinimal(true);
                re.indexIn(data);
                mMirrors.first() = re.cap(1);
                mReList.first().clear();
                download();
            }
        }
        else
        {
            mMirrors.first() = redirect;
            download();
        }
    }
    else if (reply->error() != QNetworkReply::OperationCanceledError)
    {
        QString errMsg = tr("Network error: %1").arg(reply->errorString());
        if (Dialogs::retry(errMsg, this))
            retry();
        else
            QDialog::reject();
    }
    else
        QDialog::reject();
    reply->deleteLater();
}

void DownloadDialog::readyRead()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() == QNetworkReply::NoError)
    {
        QFile f(mOutFile);
        f.open(QFile::Append);
        f.write(reply->readAll());
    }
}

void DownloadDialog::download()
{
    ui->label->setText(mMirrors.first());
    if (QFile::exists(mOutFile))
        QFile::remove(mOutFile);
    QNetworkRequest request(mMirrors.first());
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);
    request.setRawHeader("User-Agent", "Mozilla Firefox");
    QNetworkReply *reply = mNam.get(request);
    connect(reply, &QNetworkReply::finished, this, &DownloadDialog::downloadFinished);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadDialog::downloadProgress);
    connect(reply, &QNetworkReply::readyRead, this, &DownloadDialog::readyRead);
    connect(this, SIGNAL(rejected()), reply, SLOT(deleteLater()));
}

void DownloadDialog::retry()
{
    mMirrors.append(mMirrors.takeFirst());
    mReList.append(mReList.takeFirst());
    download();
}

void DownloadDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl(HELP_URL));
}
