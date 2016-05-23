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

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QSettings>
#include <QUrl>

#include "ui_editprefixdialog.h"
#include "editshortcutdialog.h"
#include "editprefixdialog.h"
#include "filesystem.h"
#include "netdialog.h"
#include "dialogs.h"

EditPrefixDialog::EditPrefixDialog(const QString &prefixHash, QWidget *parent) :
    SingletonDialog(parent),
    ui(new Ui::EditPrefixDialog),
    mPrefixHash(prefixHash),
    mPrefixList(FS::data().entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
{
    ui->setupUi(this);

    QSettings s("winewizard", "settings");
    s.beginGroup("EditPrefixDialog");
    resize(s.value("Size", size()).toSize());

    ui->addBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
    ui->editBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    ui->deleteBtn->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    ui->setIconBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));

    QDir pDir = FS::prefix(prefixHash);
    QSettings solSett(pDir.absoluteFilePath(".settings"), QSettings::IniFormat);
    mPrefixName = solSett.value("Name").toString();
    QIcon icon = pDir.exists(".icon") ? QIcon(pDir.absoluteFilePath(".icon")) :
                                        style()->standardIcon(QStyle::SP_DirIcon);;
    ui->icon->setIcon(icon);
    ui->name->setText(mPrefixName);
    QStandardItemModel *model = new QStandardItemModel(this);
    QDir shDir = FS::shortcuts(mPrefixHash);
    QDir iDir = FS::icons(mPrefixHash);
    for (const QString &shortcut : shDir.entryList(QDir::Files | QDir::Hidden))
    {
        QIcon icon = iDir.exists(shortcut) ? QIcon(iDir.absoluteFilePath(shortcut)) :
                                  QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon);
        QSettings s(shDir.absoluteFilePath(shortcut), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        QString name = s.value("Name").toString();
        QString exe = FS::toUnixPath(mPrefixHash, s.value("Exe").toString());
        QString workDir = FS::toUnixPath(mPrefixHash, s.value("WorkDir").toString());
        QString args = s.value("Args").toString();
        bool debug = s.value("Debug", true).toBool();
        QStandardItem *item = new QStandardItem(icon, name);
        item->setData(exe, ExeRole);
        item->setData(workDir, WorkDirRole);
        item->setData(args, ArgsRole);
        item->setData(debug, DebugRole);
        item->setData(shortcut, HashRole);
        model->appendRow(item);
    }
    model->sort(0);
    ui->shortcuts->setModel(model);
    connect(ui->shortcuts->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &EditPrefixDialog::currentChanged);
    if (model->rowCount() > 0)
        ui->shortcuts->setCurrentIndex(model->index(0, 0));
}

EditPrefixDialog::~EditPrefixDialog()
{
    QSettings s("winewizard", "settings");
    s.beginGroup("EditPrefixDialog");
    s.setValue("Size", size());
    delete ui;
}

void EditPrefixDialog::accept()
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui->shortcuts->model());
    QDir iDir = FS::icons(mPrefixHash);
    QDir sDir = FS::shortcuts(mPrefixHash);
    for (int i = model->rowCount() - 1; i >= 0; --i)
    {
        QStandardItem *item = model->item(i);
        QString hash = FS::hash(item->text());
        QString itemHash = item->data(HashRole).toString();
        if (itemHash != hash && !itemHash.isEmpty())
        {
            iDir.rename(itemHash, hash);
            sDir.rename(itemHash, hash);
        }
        item->setData(hash, HashRole);
        QSettings s(sDir.absoluteFilePath(hash), QSettings::IniFormat);
        s.setValue("Name", item->text());
        s.setValue("Exe", FS::toWinPath(mPrefixHash, item->data(ExeRole).toString()));
        s.setValue("WorkDir", FS::toWinPath(mPrefixHash, item->data(WorkDirRole).toString()));
        s.setValue("Args", item->data(ArgsRole));
        s.setValue("Debug", item->data(DebugRole));
        QString iconPath = item->data(IconRole).toString();
        if (!iconPath.isEmpty())
        {
            if (iDir.exists(hash))
                iDir.remove(hash);
            QFile::copy(iconPath, iDir.absoluteFilePath(hash));
        }
    }
    for (const QString &hash : sDir.entryList(QDir::Files))
        if (model->match(model->index(0, 0), HashRole, hash, -1, Qt::MatchCaseSensitive).isEmpty())
        {
            sDir.remove(hash);
            if (iDir.exists(hash))
                iDir.remove(hash);
        }
    QDir solDir = FS::prefix(mPrefixHash);
    if (!mIcon.isEmpty())
    {
        if (solDir.exists(".icon"))
            solDir.remove(".icon");
        QFile::copy(mIcon, solDir.absoluteFilePath(".icon"));
    }
    QString name = ui->name->text().trimmed();
    if (name != mPrefixName)
    {
        QString hash = FS::hash(name);
        FS::data().rename(mPrefixHash, hash);
        QSettings s(solDir.absoluteFilePath(".settings"), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        s.setValue("Name", name);
    }
    QDialog::accept();
}

bool EditPrefixDialog::exists(const QString &name) const
{
    QString tName = name.trimmed();
    if (mPrefixName == tName)
        return false;
    return mPrefixList.contains(FS::hash(tName));
}

void EditPrefixDialog::on_name_textChanged(const QString &name)
{
    QString tName = name.trimmed();
    if (tName.isEmpty())
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->status->setText(tr("Empty name!"));
    }
    else if (exists(tName))
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->status->setText(tr("Name is already used!"));
    }
    else
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->status->setText(tr("OK"));
    }
}

void EditPrefixDialog::on_icon_clicked()
{
    QString path = Dialogs::open(tr("Select Icon"), tr("Icon files (*.png)"), this);
    if (!path.isEmpty())
    {
        mIcon = path;
        ui->icon->setIcon(QIcon(mIcon));
    }
}

void EditPrefixDialog::currentChanged(const QModelIndex &index)
{
    ui->deleteBtn->setEnabled(index.isValid());
    ui->editBtn->setEnabled(index.isValid());
    if (index.isValid())
    {
        QString iconPath = index.data(IconRole).toString();
        if (iconPath.isEmpty())
        {
            QString shortcutHash = index.data(HashRole).toString();
            if (!shortcutHash.isEmpty())
            {
                QDir iDir = FS::icons(mPrefixHash);
                if (iDir.exists(shortcutHash))
                    iconPath = iDir.absoluteFilePath(shortcutHash);
            }
        }
        ui->setIconBtn->setDisabled(iconPath.isEmpty());
    }
    else
        ui->setIconBtn->setDisabled(true);
}

void EditPrefixDialog::on_deleteBtn_clicked()
{
    QModelIndex index = ui->shortcuts->currentIndex();
    QString name = index.data().toString();
    if (Dialogs::confirm(tr(R"(Are you sure you want to delete "%1"?)").arg(name), this))
        ui->shortcuts->model()->removeRow(index.row());
}

void EditPrefixDialog::on_addBtn_clicked()
{
    QString drive = FS::drive(mPrefixHash).absolutePath();
    QString exe = Dialogs::open(tr("Select Application"), tr("Executable files (*.exe *.msi)"), this, drive);
    if (!exe.isEmpty())
    {
        QStandardItemModel *model = static_cast<QStandardItemModel *>(ui->shortcuts->model());
        QFileInfo info(exe);
        QString name = info.completeBaseName();
        while (!model->findItems(name, Qt::MatchCaseSensitive).isEmpty())
            name += '0';
        QIcon icon = QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon);
        QStandardItem *item = new QStandardItem(icon, name);
        item->setData(FS::toUnixPath(mPrefixHash, FS::toWinPath(mPrefixHash, exe)), ExeRole);
        item->setData(FS::toUnixPath(mPrefixHash, FS::toWinPath(mPrefixHash, info.absolutePath())), WorkDirRole);
        item->setData(true, DebugRole);
        model->appendRow(item);
        ui->shortcuts->setCurrentIndex(model->index(model->rowCount() - 1, 0));
        model->sort(0);
        EditShortcutDialog(mPrefixHash, ui->shortcuts->currentIndex(), this).exec();
    }
}

void EditPrefixDialog::on_editBtn_clicked()
{
    EditShortcutDialog(mPrefixHash, ui->shortcuts->currentIndex(), this).exec();
}

void EditPrefixDialog::on_setIconBtn_clicked()
{
    QModelIndex index = ui->shortcuts->currentIndex();
    QString iconPath = index.data(IconRole).toString();
    if (iconPath.isEmpty())
    {
        QString shortcutHash = index.data(HashRole).toString();
        QDir iDir = FS::icons(mPrefixHash);
        if (iDir.exists(shortcutHash))
            iconPath = iDir.absoluteFilePath(shortcutHash);
    }
    mIcon = iconPath;
    ui->icon->setIcon(QIcon(mIcon));
}

void EditPrefixDialog::on_buttonBox_helpRequested()
{
    QDesktopServices::openUrl(QUrl(HELP_URL));
}
