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
#include <QFileDialog>
#include <QSettings>

#include "ui_editshortcutdialog.h"
#include "editshortcutdialog.h"
#include "mainmodel.h"
#include "utils.h"

EditShortcutDialog::EditShortcutDialog(QAbstractItemModel *model, const QModelIndex &shortcut, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditShortcutDialog),
    mModel(static_cast<QSortFilterProxyModel *>(model)),
    mShortcut(shortcut)
{
    ui->setupUi(this);
    ui->targetBrowseBtn->setIcon(QIcon::fromTheme("folder-open", style()->standardIcon(QStyle::SP_DirOpenIcon)));
    ui->workDirBrowseBtn->setIcon(QIcon::fromTheme("folder-open", style()->standardIcon(QStyle::SP_DirOpenIcon)));
    QMap<int, QVariant> data = mModel->sourceModel()->itemData(mModel->mapToSource(mShortcut));
    ui->name->setText(data.value(Qt::EditRole).toString());
    ui->icon->setIcon(QPixmap::fromImage(data.value(Qt::DecorationRole).value<QImage>()));
    mIcon = data.value(MainModel::IconLocationRole).toString();
    ui->target->setText(data.value(MainModel::TargetRole).toString());
    ui->args->setText(data.value(MainModel::ArgsRole).toString());
    ui->workDir->setText(data.value(MainModel::WorkDirRole).toString());
    QDesktopWidget *dw = QApplication::desktop();
    QSettings s("winewizard", "settings");
    s.beginGroup("EditShortcutDialog");
    resize(s.value("Width", dw->width() * 0.6).toInt(), height());
    s.endGroup();
    setFixedHeight(sizeHint().height());
}

EditShortcutDialog::~EditShortcutDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("EditShortcutDialog");
    s.setValue("Width", width());
    s.endGroup();
    delete ui;
}

void EditShortcutDialog::accept()
{
    QMap<int, QVariant> data;
    data.insert(Qt::EditRole, ui->name->text().trimmed());
    data.insert(Qt::DecorationRole, ui->icon->icon().pixmap(32, 32).
                scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage());
    data.insert(MainModel::IconLocationRole, mIcon);
    data.insert(MainModel::TargetRole, ui->target->text().trimmed());
    data.insert(MainModel::ArgsRole, ui->args->text().trimmed());
    data.insert(MainModel::WorkDirRole, ui->workDir->text().trimmed());
    mModel->sourceModel()->setItemData(mModel->mapToSource(mShortcut), data);
    QDialog::accept();
}

void EditShortcutDialog::on_icon_clicked()
{
    QString path = Utils::selectIcon(preparePath(mIcon), this);
    if (!path.isEmpty())
    {
        ui->icon->setIcon(QPixmap::fromImage(Utils::extractIcon(path)));
        mIcon = path;
    }
}

void EditShortcutDialog::on_workDirBrowseBtn_clicked()
{
    QString path = Utils::selectDir(preparePath(ui->workDir->text()), this);
    if (!path.isEmpty())
        ui->workDir->setText(path);
}

void EditShortcutDialog::on_targetBrowseBtn_clicked()
{
    QString path = Utils::selectExe(preparePath(ui->target->text()), this);
    if (!path.isEmpty())
        ui->target->setText(path);
}

void EditShortcutDialog::on_target_textChanged(const QString &target)
{
    QPalette p = palette();
    bool exists = !target.isEmpty() && QFile::exists(target.trimmed());
    if (!exists)
        p.setColor(QPalette::Text, QColor(Qt::red));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(exists);
    ui->target->setPalette(p);
}

void EditShortcutDialog::on_workDir_textChanged(const QString &workDir)
{
    QPalette p = QApplication::palette();
    bool exists = !workDir.isEmpty() && QFile::exists(workDir.trimmed());
    if (!exists)
        p.setColor(QPalette::Text, QColor(Qt::red));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(exists);
    ui->workDir->setPalette(p);
}

QString EditShortcutDialog::preparePath(const QString &path) const
{
    if (QFile::exists(path.trimmed()))
        return path;
    else
    {
        QDir dir(mShortcut.parent().data(MainModel::PathRole).toString());
        if (dir.exists("dosdevices/c:"))
            return dir.absoluteFilePath("dosdevices/c:");
        if (dir.exists())
            return dir.absolutePath();
    }
    return QDir::homePath();
}

void EditShortcutDialog::on_name_textChanged(const QString &name)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(name.trimmed().isEmpty());
}

void EditShortcutDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl("http://wwizard.net/help/#edit-shortcut"));
}
