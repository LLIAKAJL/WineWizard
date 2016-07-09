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

#include <QInputDialog>
#include <QFileDialog>
#include <QPushButton>
#include <QSettings>
#include <QCheckBox>
#include <QLayout>

#include "filesystem.h"
#include "dialogs.h"

namespace Dialogs
{
    void error(const QString &text, QWidget *parent)
    {
        QMessageBox::critical(parent, QObject::tr("Error"), text, QMessageBox::Ok);
    }

    bool confirm(const QString &text, QWidget *parent)
    {
        return QMessageBox::warning(parent, QObject::tr("Warning"), text,
                                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
    }

    bool question(const QString &text, QWidget *parent)
    {
        return QMessageBox::question(parent, QObject::tr("Confirm"), text,
                                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
    }

    bool retry(const QString &text, QWidget *parent)
    {
        return QMessageBox::critical(parent, QObject::tr("Error"), text,
                                    QMessageBox::Retry | QMessageBox::Cancel) == QMessageBox::Retry;
    }

    QString open(const QString &title, const QString &filter, QWidget *parent, const QString &dir)
    {
        return QFileDialog::getOpenFileName(parent, title, dir, filter);
    }

    QString dir(const QString &start, QWidget *parent)
    {
        return QFileDialog::getExistingDirectory(parent,
                   QObject::tr("Select Directory"), start, QFileDialog::ShowDirsOnly);
    }

    bool getText(const QString &title, const QString &label, QString &text, QWidget *parent)
    {
        QInputDialog id(parent);
        id.setInputMode(QInputDialog::TextInput);
        id.setWindowTitle(title);
        id.resize(550, id.height());
        id.setLabelText(label);
        id.setTextValue(text);
        if (id.exec() == QInputDialog::Accepted)
        {
            text = id.textValue();
            return true;
        }
        return false;
    }
}
