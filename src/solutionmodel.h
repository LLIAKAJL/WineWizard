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

#ifndef SOLUTIONMODEL_H
#define SOLUTIONMODEL_H

#include <QAbstractTableModel>
#include <QJsonObject>
#include <QColor>

#include "netmanager.h"

class SolutionModel : public QAbstractTableModel
{
    Q_OBJECT

    struct Item
    {
        int id;
        QString date, rating;
        int bw, aw;
        QList<int> bp, ap;
    };
    typedef QList<Item> ItemList;

public:
    typedef QList<int> IntList;
    struct Package
    {
        QString name, tooltip;
        IntList categories;
    };
    typedef QMap<int, Package> PackageList;
    typedef QMap<QString, int> ErrorList;
    typedef QMap<int, QString> CategoryList;

    enum
    {
        ResetRole = Qt::UserRole + 1,
        ScriptRole,
        PackagesRole,
        WinesRole,
        ErrorsRole,
        CatRole,
        CloneRole,
        IdRole,
        BWRole,
        AWRole,
        BPRole,
        APRole
    };

    explicit SolutionModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;

signals:
    void started();
    void finished();
    void error(int code);
    void readyOutput(const QString &data);
    void readyError(const QString &data);

private slots:
    void downloadFinished();
    void clear();

private:
    ItemList mItems;
    NetManager mNetMgr;
    QString mOut, mScriptOut, mRepo;
    QByteArray mScript;
    PackageList mPackages, mWines;
    ErrorList mErrors;
    CategoryList mCategories;

    QColor ratingToColor(const QString &rating) const;
    void reset(const QPair<QString, int> &args);
    void cloneItem(const QModelIndex &index);
    QString displayWines(const QModelIndex &index) const;
    QString displayPackages(const QModelIndex &index) const;
};

Q_DECLARE_METATYPE(SolutionModel::Package)

#endif // SOLUTIONMODEL_H
