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

#include "solutionmodel.h"
#include "installwizard.h"
#include "searchmodel.h"
#include "ui_winpage.h"
#include "filesystem.h"
#include "executor.h"
#include "winpage.h"

const QString API_URL = "http://wwizard.net/api2/";

WinPage::WinPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WinPage)
{
    ui->setupUi(this);
}

WinPage::~WinPage()
{
    delete ui;
}

void WinPage::initializePage()
{
    QModelIndex index = field("app").toModelIndex();
    SolutionModel *sm = index.data(SearchModel::ModelRole).value<SolutionModel *>();
    QModelIndex si = sm->index(index.data(SearchModel::SolutionRole).toInt());
    setTitle(tr("Data updating"));
    setSubTitle(tr("This may take several minutes. Please wait..."));
    ui->pages->setCurrentIndex(0);

    QJsonObject jo;
    jo.insert("arch", field("arch").toString());
    jo.insert("os", OS);
    jo.insert("app", index.data(SearchModel::IdRole).toInt());
    jo.insert("bw", si.data(SolutionModel::BWRole).toInt());
    jo.insert("aw", si.data(SolutionModel::BWRole).toInt());
    QJsonArray bp;
    for (int p : si.data(SolutionModel::BPRole).value<QList<int>>())
        bp.append(p);
    QJsonArray ap;
    for (int p : si.data(SolutionModel::APRole).value<QList<int>>())
        ap.append(p);
    jo.insert("bp", bp);
    jo.insert("ap", ap);
    jo.insert("bs", si.data(SolutionModel::BSRole).toString());
    jo.insert("as", si.data(SolutionModel::ASRole).toString());
    jo.insert("id", si.data(SolutionModel::IdRole).toInt());
    jo.insert("c", "fork");
    QString data = QJsonDocument(jo).toJson();
    QString path = FS::temp().absoluteFilePath("response");
    QString script = QString(FS::readFile(":/post")).arg(API_URL).arg(data).arg(path);

    Executor *e = new Executor;
    connect(e, &Executor::finished, this, &WinPage::finished);
    connect(e, &Executor::readyOutput, this, &WinPage::readyOutput);
    connect(e, &Executor::readyError, this, &WinPage::readyError);
    e->start(script);
}

bool WinPage::isComplete() const
{
    return ui->pages->currentIndex() > 0;
}

int WinPage::nextId() const
{
    return -1;
}

void WinPage::finished()
{
    setTitle(tr("Congratulations!"));
    QModelIndex index = field("app").toModelIndex();
    QString name = index.data().toString();
    setSubTitle(tr("Application \"%1\" has been successfully installed.").arg(name));
    ui->pages->setCurrentIndex(1);
    emit completeChanged();
}

void WinPage::readyOutput(const QByteArray &text)
{
    appendOut(text);
}

void WinPage::readyError(const QByteArray &text)
{
    appendOut(QString(R"(<span style=" color:#ff0000;">%1</span>)").arg(QString(text)));
}

void WinPage::appendOut(const QString &text)
{
    ui->out->moveCursor(QTextCursor::End);
    ui->out->insertHtml(QString(text).replace('\n', "<br>"));
}
