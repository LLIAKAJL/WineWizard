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

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QStringList>
#include <QJsonArray>
#include <QObject>
#include <QString>
#include <QList>

typedef QList<int> IntList;

class Repository : public QObject
{
    Q_OBJECT

public:
    struct Category
    {
        int id;
        QString name;
    };
    typedef QList<Category> CategoryList;

    struct Package
    {
        int id;
        QString name, tooltip, check, install;
        QList<int> categories, required;
        bool operator ==(const Package &p) const;
    };
    typedef QList<Package> PackageList;

    struct Wine
    {
        int id;
        QString version;
        QList<int> categories, required;
        bool operator ==(const Package &p) const;
    };
    typedef QList<Wine> WineList;

    struct Mirror
    {
        QString url, re;
    };
    typedef QList<Mirror> MirrorList;

    struct File
    {
        int id;
        QString name, sum;
        MirrorList mirrors;
    };
    typedef QList<File> FileList;

    struct Function
    {
        QString name, body;
    };
    typedef QList<Function> FunctionList;

    struct Error
    {
        QString re;
        int package;
    };
    typedef QList<Error> ErrorList;

    explicit Repository(QObject *parent = nullptr);

    void setArch(const QString &arch);
    const WineList &wines() const;
    const PackageList &packages() const;
    const CategoryList &pCategories() const;
    const CategoryList &wCategories() const;
    Wine wine(int id) const;
    const FileList &files() const;
    File file(int id) const;
    Package package(int id) const;
    PackageList packagesFromArr(const IntList &arr) const;
    const FunctionList &functions() const;
    const ErrorList &errors() const;
    const Function &init() const;
    const Function &done() const;

private:
    PackageList mPackages;
    WineList mWines;
    CategoryList mPCategories, mWCategories;
    FileList mFiles;
    FunctionList mFunctions;
    Function mInit, mDone;
    ErrorList mErrors;
};
Q_DECLARE_METATYPE(Repository::Package)

#endif // REPOSITORY_H
