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

#ifndef CREATESHORTCUTDIALOG_H
#define CREATESHORTCUTDIALOG_H

#include <QAbstractItemModel>
#include <QFileInfo>
#include <QDialog>

namespace Ui {
class CreateShortcutDialog;
}

class CreateShortcutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateShortcutDialog(QAbstractItemModel *model, const QFileInfo &exe, QWidget *parent = nullptr);
    ~CreateShortcutDialog() override;

    QString name() const;
    QString workDir() const;
    QString args() const;

private slots:
    void on_browseBtn_clicked();
    void on_name_textChanged(const QString &name);
    void on_buttonBox_helpRequested();

private:
    Ui::CreateShortcutDialog *ui;
    QAbstractItemModel *mModel;

    bool exists(const QString &name) const;
};

#endif // CREATESHORTCUTDIALOG_H
