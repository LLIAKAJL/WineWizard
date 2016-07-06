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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QtGlobal>
#include <QThread>

#include "ui_installpage.h"
#include "solutiondialog.h"
#include "installwizard.h"
#include "solutionmodel.h"
#include "searchmodel.h"
#include "installpage.h"
#include "repository.h"
#include "filesystem.h"
#include "executor.h"
#include "dialogs.h"

const QString PREPARE_PACKAGES = "ww_installed_%1()\n{\n%2\n}\nww_install_%1()\n{\n%3\n}\n";
const QString PREPARE_FUNCTIONS = "ww_%1()\n{\n%2\n}\n";

InstallPage::InstallPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallPage)
{
    ui->setupUi(this);
    registerField("out", this, "out");
    registerField("err", this, "err");
    registerField("i_out", this, "iOut");
    registerField("i_err", this, "iErr");
}

InstallPage::~InstallPage()
{
    delete ui;
}

void InstallPage::initializePage()
{
    setProperty("completed", false);
    setProperty("failed", false);
    mBs.clear();
    mAcs.clear();
    mAs.clear();
    mOut.clear();
    mErr.clear();
    ui->out->clear();
    QModelIndex ai = field("app").toModelIndex();
    QString prefix = ai.data(SearchModel::PrefixRole).toString();
    QString name = ai.data().toString();
    setSubTitle(tr("Please wait while \"%1\" is being installed.").arg(name));
    SolutionModel *sm = ai.data(SearchModel::ModelRole).value<SolutionModel *>();
    QModelIndex si = sm->index(ai.data(SearchModel::SolutionRole).toInt());
    Repository *repo = field("repository").value<Repository *>();

    QString cs;
    for (const Repository::Function f : repo->functions())
        cs += PREPARE_FUNCTIONS.arg(f.name).arg(f.body);
    for (const Repository::Package p : repo->packages())
        cs += PREPARE_PACKAGES.arg(p.name).arg(p.check).arg(p.install);
    Repository::Wine bw = repo->wine(si.data(SolutionModel::BWRole).toInt());
    Repository::Wine aw = repo->wine(si.data(SolutionModel::AWRole).toInt());
    QString bs = si.data(SolutionModel::BSRole).toString();
    QString as = si.data(SolutionModel::ASRole).toString();
    QSettings s("winewizard", "settings");
    s.beginGroup("VideoSettings");
    QString scrW = s.value("ScreenWidth").toString();
    QString scrH = s.value("ScreenHeight").toString();
    QString vmSize = s.value("VideoMemorySize").toString();
    s.endGroup();
    mBs = cs + repo->init().body.arg(bw.version).arg(scrW + 'x' + scrH).arg(vmSize) + '\n';

    IntList required;
    required.append(bw.required);
    required.append(aw.required);

    IntList bp = si.data(SolutionModel::BPRole).value<IntList>();
    for (int id : bp)
        required.append(repo->package(id).required);

    IntList ap = si.data(SolutionModel::APRole).value<IntList>();
    for (int id : ap)
        required.append(repo->package(id).required);

    if (!bp.isEmpty() || !bs.isEmpty())
    {
        mAcs = mBs;
        for (const Repository::Package &p : repo->packagesFromArr(bp))
            mAcs += "ww_install " + p.name + '\n';
        mAcs += bs + '\n';
    }

    QString requiredScript;
    QDir cache = FS::cache();
    for (int id : required.toSet())
    {
        Repository::File f = repo->file(id);
        requiredScript += "ww_download_package " + f.sum + ' ' + cache.absoluteFilePath(f.name);
        for (const Repository::Mirror &m : f.mirrors)
            requiredScript += ' ' + m.url + " \'" + m.re + '\'';
        requiredScript += '\n';
    }

    mBs += requiredScript;
    mBs += "ww_install_wine " + bw.version + '\n';

    if (aw.version != bw.version || !ap.isEmpty() || !as.isEmpty())
    {
        mAs = cs + repo->init().body.arg(aw.version).arg(scrW + 'x' + scrH).arg(vmSize) + "\n";
        if (aw.version != bw.version)
            mAs += "ww_install_wine " + aw.version + '\n';
        for (const Repository::Package &p : repo->packagesFromArr(ap))
            mAs += "ww_install " + p.name + '\n';
        if (!as.isEmpty())
            mAs += as + '\n';
    }
    mAs += repo->done().body;
    if (!prefix.isEmpty() && FS::prefix(prefix).exists())
        FS::prefix(prefix).removeRecursively();
    Executor *e = new Executor(prefix);
    connect(e, &Executor::finished, this, &InstallPage::bsFinished);
    connect(e, &Executor::readyOutput, this, &InstallPage::readyOutput);
    connect(e, &Executor::readyError, this, &InstallPage::readyError);
    e->start(mBs);
}

int InstallPage::nextId() const
{
    return InstallWizard::PageDebug;
}

bool InstallPage::isComplete() const
{
    return property("completed").toBool() || property("failed").toBool();
}

bool InstallPage::validatePage()
{
    if (property("failed").toBool())
    {
        if (SolutionDialog(wizard(), wizard()).exec() == SolutionDialog::Accepted)
        {
            wizard()->back();
            wizard()->next();
        }
        return false;
    }
    return true;
}

void InstallPage::bsFinished(int code)
{
    if (code != 0)
        installationFailed();
    else
    {
        QModelIndex index = field("app").toModelIndex();
        QString prefix = index.data(SearchModel::PrefixRole).toString();
        QString name = index.data().toString();
        QSettings s(FS::prefix(prefix).absoluteFilePath(".settings"), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        s.setValue("Name", name);
        Executor *e = new Executor(prefix);
        connect(e, &Executor::finished, this, &InstallPage::createFinished);
        connect(e, &Executor::readyOutput, this, &InstallPage::readyOutput);
        connect(e, &Executor::readyError, this, &InstallPage::readyError);
        e->start(FS::readFile(":/create"));
    }
}

void InstallPage::createFinished(int code)
{
    if (code != 0)
        installationFailed();
    else
    {
        QString arch = field("arch").toString();
        QString prefix = field("app").toModelIndex().data(SearchModel::PrefixRole).toString();
        QString wmbPath = FS::sys32(prefix, arch).absoluteFilePath("winemenubuilder.exe");
        QFile::remove(wmbPath);
        QFile::copy(":/winemenubuilder.exe", wmbPath);
        if (arch == "64")
        {
            wmbPath = FS::sys64(prefix).absoluteFilePath("winemenubuilder.exe");
            QFile::remove(wmbPath);
            QFile::copy(":/winemenubuilder64.exe", wmbPath);
        }
        if (mAcs.isEmpty())
            install();
        else
        {
            Executor *e = new Executor(prefix);
            connect(e, &Executor::finished, this, &InstallPage::install);
            connect(e, &Executor::readyOutput, this, &InstallPage::readyOutput);
            connect(e, &Executor::readyError, this, &InstallPage::readyError);
            e->start(mAcs);
        }
    }
}

void InstallPage::installFinished(int code)
{
    if (code != 0)
        installationFailed();
    else
    {
        if (mAs.isEmpty())
            setupFinished();
        else
        {
            QString prefix = field("app").toModelIndex().data(SearchModel::PrefixRole).toString();
            Executor *e = new Executor(prefix);
            connect(e, &Executor::finished, this, &InstallPage::setupFinished);
            connect(e, &Executor::readyOutput, this, &InstallPage::readyOutput);
            connect(e, &Executor::readyError, this, &InstallPage::readyError);
            e->start(mAs);
        }
    }
}

void InstallPage::install()
{
    QString prefix = field("app").toModelIndex().data(SearchModel::PrefixRole).toString();
    QString exe = field("exe").toString();
    QString workDir = field("work_dir").toString();
    QString args = field("args").toString();
    QString script = QString(FS::readFile(":/run")).arg(exe).arg(workDir).arg(args);
    Executor *e = new Executor(prefix);
    connect(e, &Executor::finished, this, &InstallPage::installFinished);
    connect(e, &Executor::readyOutput, this, &InstallPage::readyLogOutput);
    connect(e, &Executor::readyError, this, &InstallPage::readyLogError);
    e->start(script);
}

void InstallPage::setupFinished()
{
    setProperty("completed", true);
    emit completeChanged();
    wizard()->next();
}

void InstallPage::readyLogOutput(const QByteArray &text)
{
    mOut += text;
    appendOut(text);
}

void InstallPage::readyLogError(const QByteArray &text)
{
    mErr += text;
    appendOut(QString(R"(<span style=" color:#ff0000;">%1</span>)").arg(QString(text)));
}

void InstallPage::readyOutput(const QByteArray &text)
{
    appendOut(text);
}

void InstallPage::readyError(const QByteArray &text)
{
    appendOut(QString(R"(<span style=" color:#ff0000;">%1</span>)").arg(QString(text)));
}

const QString &InstallPage::out() const
{
    return mOut;
}

const QString &InstallPage::err() const
{
    return mErr;
}

void InstallPage::setOut(const QString &text)
{
    mOut = text;
}

void InstallPage::setErr(const QString &text)
{
    mErr = text;
}

void InstallPage::appendOut(const QString &text)
{
    ui->out->moveCursor(QTextCursor::End);
    ui->out->insertHtml(QString(text).replace('\n', "<br>"));
}

void InstallPage::installationFailed()
{
    Dialogs::error(tr("Installation failed!"));
    setProperty("failed", true);
    emit completeChanged();
}
