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

#include "ui_postdialog.h"
#include "postdialog.h"
#include "dialogs.h"

PostDialog::PostDialog(const QString &url, const QJsonDocument &data, QWidget *parent) :
    NetDialog(parent),
    ui(new Ui::PostDialog),
    mUrl(url),
    mData(data)
{
    ui->setupUi(this);
    post();
}

PostDialog::~PostDialog()
{
    delete ui;
}

void PostDialog::reject()
{
}

void PostDialog::postFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError)
    {
        QString errMsg = tr("Network error: %1").arg(reply->errorString());
        if (reply->error() != QNetworkReply::OperationCanceledError && Dialogs::retry(errMsg, this))
            post();
        else
            QDialog::reject();
    }
    else
    {
        QStringList r = QString(reply->readAll()).split('\n', QString::SkipEmptyParts);
        if (r.isEmpty())
        {
            Dialogs::error(tr("Unknown error!"), this);
            QDialog::reject();
        }
        else if (r.first() != "OK")
        {
            Dialogs::error(tr("Server error: %1").arg(r.last()), this);
            QDialog::reject();
        }
        else
            accept();
    }
    reply->deleteLater();
}

void PostDialog::post()
{
    QNetworkRequest request(mUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla Firefox");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);
    QNetworkReply *reply = mNam.post(request, mData.toJson()/*.query().toUtf8()*/);
    connect(reply, &QNetworkReply::finished, this, &PostDialog::postFinished);
    connect(this, &PostDialog::rejected, reply, &QNetworkReply::abort);
}
