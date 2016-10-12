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

#include <QVariant>

#include "updatepage.h"
#include "mainmodel.h"

UpdatePage::UpdatePage(QAbstractItemModel *model, QWidget *parent) :
    QWizardPage(parent),
    mModel(model)
{
}

void UpdatePage::initializePage()
{
    QVariantMap data;
    data.insert("exe", field("exe"));
    data.insert("args", field("args"));
    data.insert("workDir", field("workDir"));
    mModel->setData(field("prefix").toModelIndex(), data, MainModel::DebugRole);
    wizard()->accept();
}

int UpdatePage::nextId() const
{
    return -1;
}
