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

#include <QSortFilterProxyModel>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include <QImage>
#include <QIcon>

#include "ui_editprefixdialog.h"
#include "editprefixdialog.h"
#include "mainmodel.h"
#include "utils.h"

EditPrefixDialog::EditPrefixDialog(QAbstractItemModel *model, const QModelIndex &prefix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditPrefixDialog),
    mModel(static_cast<QSortFilterProxyModel *>(model)),
    mPrefix(prefix)
{
    ui->setupUi(this);
    QDesktopWidget *dw = QApplication::desktop();
    QSettings s("winewizard", "settings");
    s.beginGroup("EditPrefixDialog");
    resize(s.value("Width", dw->width() * 0.6).toInt(), height());
    s.endGroup();
    setFixedHeight(sizeHint().height());
    QAbstractItemModel *m = mModel->sourceModel();
    QMap<int, QVariant> data = m->itemData(mModel->mapToSource(mPrefix));
    ui->name->setText(data.value(Qt::EditRole).toString());
    ui->icon->setIcon(QPixmap::fromImage(data.value(Qt::DecorationRole).value<QImage>()));
}

EditPrefixDialog::~EditPrefixDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("EditPrefixDialog");
    s.setValue("Width", width());
    s.endGroup();
    delete ui;
}

void EditPrefixDialog::accept()
{
    QMap<int, QVariant> request;
    request.insert(Qt::DecorationRole, scaleIcon(ui->icon->icon().pixmap(32, 32)).toImage());
    request.insert(Qt::EditRole, ui->name->text().trimmed());
    mModel->sourceModel()->setItemData(mModel->mapToSource(mPrefix), request);
    QDialog::accept();
}

void EditPrefixDialog::on_icon_clicked()
{
    QDir dir(mPrefix.data(MainModel::PathRole).toString());
    QString path = dir.exists("dosdevices/c:") ? dir.absoluteFilePath("dosdevices/c:") : QDir::homePath();
    path = Utils::selectIcon(path, this);
    if (!path.isEmpty())
        ui->icon->setIcon(QPixmap::fromImage(Utils::extractIcon(path)));
}

QPixmap EditPrefixDialog::scaleIcon(const QPixmap &icon) const
{
    return icon.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

void EditPrefixDialog::on_name_textChanged(const QString &name)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(name.trimmed().isEmpty());
}

void EditPrefixDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help/#edit-prefix"));
}
