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
#include <QNetworkRequest>
#include <QSettings>
#include <QUrl>

#include "ui_downloaddialog.h"
#include "downloaddialog.h"
#include "filesystem.h"
#include "executor.h"
#include "dialogs.h"

DownloadDialog::DownloadDialog(const QString &url, QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::DownloadDialog)
{
    ui->setupUi(this);
    ui->file->setVisible(false);
    if (url.isEmpty())
        getUpdate();
    else
        getSolution(url);
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
    auto reply = static_cast<QNetworkReply *>(sender());
    auto redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
    if (redirect.isEmpty())
    {
        ui->progressBar->setMaximum(bytesTotal);
        ui->progressBar->setValue(bytesReceived);
        ui->file->setText(tr("Name: %1").arg(mList.first().first));
    }
    else
    {
        ui->progressBar->setRange(0, 0);
        ui->file->setText(tr("Please wait..."));
    }
}

void DownloadDialog::otherProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesReceived);
}

void DownloadDialog::packageFinished()
{
    auto reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError)
    {
        if (reply->error() != QNetworkReply::OperationCanceledError && Dialogs::retry(tr("Network error: %1").arg(reply->errorString()), this))
            getPackage();
        else
            QDialog::reject();
    }
    else
    {
        auto download = mList.first();
        auto redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        if (redirect.isEmpty())
        {
            auto cd = FS::cache();
            if (download.first == "main_package")
            {
                QSettings u(FS::temp().absoluteFilePath("update"), QSettings::IniFormat);
                if (FS::md5(cd.absoluteFilePath("main_package.wwp")) == u.value("MainPackageHash").toString())
                {
                    mList.removeFirst();
                    Ex::wait(FS::readFile(":/decrypt").arg(download.first), QString(), this);
                    Ex::wait(FS::readFile(":/extract-main"), QString(), this);
                    auto conf = FS::config();
                    QSettings s(conf.absoluteFilePath("versions"), QSettings::IniFormat);
                    QSettings v(cd.absoluteFilePath("versions"), QSettings::IniFormat);
                    for (auto g : s.childKeys())
                        if (s.value(g, 0).toInt() < v.value(g, -1).toInt())
                            cd.remove(g + ".7z");
                    conf.remove("versions");
                    QFile::rename(FS::mainPart("versions"), conf.absoluteFilePath("versions"));
                }
            }
            else
            {
                QSettings h(FS::mainPart("hashes"), QSettings::IniFormat);
                if (FS::md5(cd.absoluteFilePath(download.first + ".wwp")) == h.value(download.first).toString())
                {
                    Ex::wait(FS::readFile(":/decrypt").arg(download.first), QString(), this);
                    mList.removeFirst();
                }
            }
            if (mList.isEmpty())
                accept();
            else
                getPackage();
        }
        else
        {
            mList.removeFirst();
            mList.push_front(Download{ download.first, redirect });
            getPackage();
        }
    }
    reply->deleteLater();
}

void DownloadDialog::solutionFinished()
{
    auto reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError)
    {
        if (reply->error() != QNetworkReply::OperationCanceledError && Dialogs::retry(tr("Network error: %1").arg(reply->errorString()), this))
            getSolution(reply->url().toString());
        else
            QDialog::reject();
    }
    else
    {
        makeRequired();
        if (mList.isEmpty())
            accept();
        else
        {
            setWindowTitle(tr("Package Download"));
            ui->progressBar->setRange(0, 0);
            ui->file->setText(tr("Please wait..."));
            ui->file->setVisible(true);
            getPackage();
        }
    }
    reply->deleteLater();
}

void DownloadDialog::updateFinished()
{
    auto reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError)
    {
        if (reply->error() != QNetworkReply::OperationCanceledError && Dialogs::retry(tr("Network error: %1").arg(reply->errorString()), this))
            getUpdate();
        else
            QDialog::reject();
    }
    else
    {
        QSettings u(FS::temp().absoluteFilePath("update"), QSettings::IniFormat);
        auto version = u.value("Version").toString();
        if (version != qApp->applicationVersion())
        {
            QDialog::reject();
            Dialogs::error(tr("Please install a newer version of Wine Wizard.\n\nThe current version is %1.\nThe required version is %2.\n\nWine Wizard will exit.").arg(qApp->applicationVersion()).arg(version));
            QDesktopServices::openUrl(QUrl("http://wwizard.net/download/"));
        }
        else
        {
            QSettings s("winewizard", "settings");
            auto myMainPackageVersion = s.value("MainPackageVersion", 0).toInt();
            auto mainPackageVersion = u.value("MainPackageVersion", 0).toInt();
            if (myMainPackageVersion != mainPackageVersion)
            {
                QDir(FS::cache().absoluteFilePath("main_package")).removeRecursively();
                s.setValue("MainPackageVersion", mainPackageVersion);
            }
            if (!FS::cache().exists("main_package"))
            {
                mList.append(Download{ "main_package", u.value("MainPackageUrl").toString() });
                getPackage();
            }
            else
                accept();
        }
    }
    reply->deleteLater();
}

void DownloadDialog::readyRead()
{
    auto reply = static_cast<QNetworkReply *>(sender());
    QFile f(reply->property("out").toString());
    f.open(QFile::Append);
    f.write(reply->readAll());
}

void DownloadDialog::getSolution(const QString &url)
{
    FS::temp().remove("solution");
    QNetworkRequest request(QNetworkRequest(url + "?strip=true"));
    request.setRawHeader("User-Agent", "Mozilla Firefox");
    auto reply = mNam.get(request);
    reply->setProperty("out", FS::temp().absoluteFilePath("solution"));
    connect(reply, &QNetworkReply::finished, this, &DownloadDialog::solutionFinished);
    connect(reply, &QNetworkReply::readyRead, this, &DownloadDialog::readyRead);
    connect(this, &DownloadDialog::rejected, reply, &QNetworkReply::deleteLater);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadDialog::otherProgress);
}

void DownloadDialog::getUpdate()
{
    setWindowTitle(tr("Check for Updates"));
    FS::temp().remove("update");
    QNetworkRequest request(QNetworkRequest(QUrl("http://wwizard.net/update/")));
    request.setRawHeader("User-Agent", "Mozilla Firefox");
    auto reply = mNam.get(request);
    reply->setProperty("out", FS::temp().absoluteFilePath("update"));
    connect(reply, &QNetworkReply::finished, this, &DownloadDialog::updateFinished);
    connect(reply, &QNetworkReply::readyRead, this, &DownloadDialog::readyRead);
    connect(this, &DownloadDialog::rejected, reply, &QNetworkReply::deleteLater);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadDialog::otherProgress);
}

void DownloadDialog::getPackage()
{
    auto download = mList.first();
    FS::cache().remove(download.first + ".wwp");
    QNetworkRequest request(QNetworkRequest(download.second));
    request.setRawHeader("User-Agent", "Mozilla Firefox");
    auto reply = mNam.get(request);
    reply->setProperty("out", FS::cache().absoluteFilePath(download.first + ".wwp"));
    connect(reply, &QNetworkReply::finished, this, &DownloadDialog::packageFinished);
    connect(reply, &QNetworkReply::readyRead, this, &DownloadDialog::readyRead);
    connect(this, &DownloadDialog::rejected, reply, &QNetworkReply::deleteLater);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadDialog::downloadProgress);
}

void DownloadDialog::makeRequired()
{
    QSettings s(FS::temp().absoluteFilePath("solution"), QSettings::IniFormat);
    s.setIniCodec("UTF-8");
    auto bw = s.value("BWine").toString();
    auto aw = s.value("AWine").toString();
    auto cache = FS::cache();
    QSettings u(FS::mainPart("urls"), QSettings::IniFormat);
    if (!cache.exists(bw + ".7z"))
        mList.append(Download{ bw, u.value(bw).toString() });
    if (!cache.exists(aw + ".7z"))
        mList.append(Download{ aw, u.value(aw).toString() });
    QSettings r(FS::mainPart("required"), QSettings::IniFormat);
    for (auto package : s.value("BPackages").toStringList())
        if (!cache.exists(package + ".7z"))
            for (auto file : r.value(package).toStringList())
                mList.append(Download{ file, u.value(file).toString() });
    for (auto package : s.value("APackages").toStringList())
        if (!cache.exists(package + ".7z"))
            for (auto file : r.value(package).toStringList())
                mList.append(Download{ file, u.value(file).toString() });
    mList = mList.toSet().toList();
}

void DownloadDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
