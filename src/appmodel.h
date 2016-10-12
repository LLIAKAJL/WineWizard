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

#ifndef APPMODEL_H
#define APPMODEL_H

#include <QAbstractListModel>

#include "netmanager.h"

class AppModel : public QAbstractListModel
{
    Q_OBJECT

    struct Item
    {
        int id;
        QString name, path;
    };
    typedef QList<Item> ItemList;

public:
    enum
    {
        ResetRole = Qt::UserRole + 1,
        IdRole,
        PathRole,
        PageCountRole,
        AlterRole,
        NewRole
    };

    explicit AppModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

signals:
    void started();
    void finished();
    void error(int code);
    void readyOutput(const QString &data);
    void readyError(const QString &data);

private slots:
    void downloadFinished();
    void addFinished();
    void clear();

private:
    ItemList mItems;
    int mPageCount;
    QString mAlter;
    QString mOut, mSearch;
    NetManager mNetMgr;

    void reset(const QPair<QString, int> &args);
    void newApp(const QString &name);
};

#endif // APPMODEL_H
