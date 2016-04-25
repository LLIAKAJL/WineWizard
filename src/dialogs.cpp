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

#include <QFileDialog>
#include <QPushButton>
#include <QSettings>
#include <QCheckBox>

#include "filesystem.h"
#include "dialogs.h"

const QString S_OK = QObject::tr("Is your application working correctly?");
const QString S_FAILED = QObject::tr("The shortcuts directory is empty! Is your application working correctly?");
const QString S_UTIL = QObject::tr(R"(Are you sure you want to execute "%1"? It might be unsafe!)");

namespace Dialogs
{
    MessageDialog::MessageDialog(QMessageBox::Icon icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QWidget *parent) :
        QMessageBox(icon, title, text, buttons, parent),
        SingletonWidget(this)
    {
    }

    //--------------------------------------------------------------------------------------------

    FinishDialog::FinishDialog(bool ok) :
        MessageDialog(ok ? Question : Warning, tr("Finish"), ok ? S_OK : S_FAILED, Yes | No)
    {
        connect(button(Yes), &QPushButton::clicked, this, &QDialog::accept);
        connect(button(No), &QPushButton::clicked, this, &QDialog::reject);
        if (ok)
        {
            auto cb = new QCheckBox(tr("End the debug session"), this);
            connect(cb, &QCheckBox::toggled, button(No), &QPushButton::setDisabled);
            cb->setChecked(!ok);
            cb->setEnabled(ok);
            setCheckBox(cb);
        }
    }

    void FinishDialog::reject()
    {
        if ((checkBox() && !checkBox()->isChecked()) || !checkBox())
            QDialog::reject();
    }

    //--------------------------------------------------------------------------------------------

    void error(const QString &text, QWidget *parent)
    {
        MessageDialog md(QMessageBox::Critical, QObject::tr("Error"), text, QMessageBox::Ok, parent);
        md.connect(md.button(QMessageBox::Ok), &QPushButton::clicked, &md, &QMessageBox::accept);
        md.exec();
    }

    bool confirm(const QString &text, QWidget *parent)
    {
        MessageDialog md(QMessageBox::Warning, QObject::tr("Warning"), text, QMessageBox::Yes | QMessageBox::No, parent);
        md.connect(md.button(QMessageBox::Yes), &QPushButton::clicked, &md, &QMessageBox::accept);
        md.connect(md.button(QMessageBox::No), &QPushButton::clicked, &md, &QMessageBox::reject);
        return md.exec() == QDialog::Accepted;
    }

    bool retry(const QString &text, QWidget *parent)
    {
        MessageDialog md(QMessageBox::Critical, QObject::tr("Error"), text, QMessageBox::Retry | QMessageBox::Cancel, parent);
        md.connect(md.button(QMessageBox::Retry), &QPushButton::clicked, &md, &QMessageBox::accept);
        md.connect(md.button(QMessageBox::Cancel), &QPushButton::clicked, &md, &QMessageBox::reject);
        return md.exec() == QDialog::Accepted;
    }

    FinishStatus finish(const QString &solution)
    {
        FinishDialog fd(!FS::shortcuts(solution).entryList(QDir::Files | QDir::Hidden).isEmpty());
        if (fd.exec() == QDialog::Accepted)
        {
            if (fd.checkBox())
                return (fd.checkBox()->isChecked()) ? Finish : Yes;
            return Finish;
        }
        else
            return FinishStatus::No;
    }

    QString open(const QString &title, const QString &filter, QWidget *parent, const QString &dir)
    {
        return QFileDialog::getOpenFileName(parent, title, dir, filter, nullptr, QFileDialog::DontUseNativeDialog);
    }

    //--------------------------------------------------------------------------------------------

    QString selectDir(const QString &start, QWidget *parent)
    {
        return QFileDialog::getExistingDirectory(parent, QObject::tr("Select Directory"), start, QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
    }

}
