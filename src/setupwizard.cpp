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

#include <QDesktopServices>
#include <QAbstractButton>
#include <QDesktopWidget>
#include <QSettings>
#include <QUrl>

#include "ui_setupwizard.h"
#include "solutionpage.h"
#include "setupwizard.h"
#include "installpage.h"
#include "updatepage.h"
#include "finalpage.h"
#include "intropage.h"
#include "debugpage.h"
#include "appmodel.h"
#include "utils.h"

SetupWizard::InstanceList SetupWizard::mInstances;

SetupWizard::SetupWizard(const QString &exe, const QString &args,
                         QAbstractItemModel *model, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::SetupWizard),
    mModel(model)
{
    mInstances.append(this);
    ui->setupUi(this);
    setWindowTitle(QFileInfo(exe).fileName());
    setWindowIcon(QPixmap::fromImage(Utils::extractIcon(exe)));
    setAttribute(Qt::WA_DeleteOnClose);
    QSettings s("winewizard", "settings");
    s.beginGroup("InstallWizard");
    QDesktopWidget *dw = QApplication::desktop();
    resize(s.value("Size", QSize(dw->width() * 0.7, dw->height() * 0.6)).toSize());
    s.endGroup();
    setPage(PageIntro, new IntroPage(exe, args, mModel, this));
    setPage(PageSolution, new SolutionPage(exe, this));
    setPage(PageInstall, new InstallPage(mModel, this));
    setPage(PageDebug, new DebugPage(mModel, this));
    setPage(PageFinal, new FinalPage(this));
    setPage(PageUpdate, new UpdatePage(mModel, this));
    for (int id : pageIds())
        connect(page(id), &QWizardPage::completeChanged, button(QWizard::BackButton), &QAbstractButton::hide);
    connect(this, &SetupWizard::currentIdChanged, button(QWizard::BackButton), &QAbstractButton::hide);
}

SetupWizard::~SetupWizard()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("InstallWizard");
    s.setValue("Size", size());
    s.endGroup();
    delete ui;
    mInstances.removeOne(this);
}

bool SetupWizard::running()
{
    return !mInstances.isEmpty();
}

void SetupWizard::reject()
{
    if (Utils::warning(tr("Do you really want to cancel the setup?"), this))
    {
        QModelIndex index = field("app").toModelIndex();
        if (index.isValid())
        {
            QString path = index.data(AppModel::PathRole).toString();
            QModelIndexList l = mModel->match(mModel->index(0, 0), MainModel::PathRole,
                                              path, -1, Qt::MatchFixedString);
            if (!l.isEmpty())
                mModel->removeRow(l.first().row());
        }
        QWizard::reject();
    }
}

void SetupWizard::setVisible(bool visible)
{
    move(QApplication::desktop()->geometry().center() - QPoint(width(), height()) * 0.5);
    QWizard::setVisible(visible);
}

void SetupWizard::hideBackButton()
{
    button(QWizard::BackButton)->hide();
}

void SetupWizard::on_SetupWizard_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help"));
}
