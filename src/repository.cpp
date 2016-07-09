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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "repository.h"
#include "filesystem.h"

Repository::Repository(QObject *parent) :
    QObject(parent)
{
}

void Repository::setArch(const QString &arch)
{
    mPackages.clear();
    mWines.clear();
    mPCategories.clear();
    mWCategories.clear();
    mFiles.clear();
    mFunctions.clear();
    QByteArray data = FS::readFile(FS::cache().absoluteFilePath(OS + arch + ".repo"));
    QJsonObject repo = QJsonDocument::fromJson(data).object();
    QJsonArray wines = repo.value("wines").toArray();
    for (QJsonArray::ConstIterator i = wines.begin(); i != wines.end(); ++i)
    {
        QJsonObject wo = (*i).toObject();
        Wine wine;
        wine.id = wo.value("i").toInt();
        wine.version = wo.value("v").toString();
        QJsonArray catArr = wo.value("c").toArray();
        for (QJsonArray::ConstIterator i = catArr.begin(); i != catArr.end(); ++i)
            wine.categories.append((*i).toInt());
        QJsonArray reqArr = wo.value("r").toArray();
        for (QJsonArray::ConstIterator i = reqArr.begin(); i != reqArr.end(); ++i)
            wine.required.append((*i).toInt());
        mWines.append(wine);
    }

    QJsonArray packages = repo.value("packages").toArray();
    for (QJsonArray::ConstIterator i = packages.begin(); i != packages.end(); ++i)
    {
        QJsonObject po = (*i).toObject();
        Package p;
        p.id = po.value("id").toInt();
        p.name = po.value("n").toString();
        p.tooltip = po.value("h").toString();
        QJsonArray catArr = po.value("cat").toArray();
        for (QJsonArray::ConstIterator i = catArr.begin(); i != catArr.end(); ++i)
            p.categories.append((*i).toInt());
        QJsonArray reqArr = po.value("r").toArray();
        for (QJsonArray::ConstIterator i = reqArr.begin(); i != reqArr.end(); ++i)
            p.required.append((*i).toInt());
        p.check = po.value("c").toString();
        p.install = po.value("i").toString();
        mPackages.append(p);
    }

    QJsonArray pCat = repo.value("pcategories").toArray();
    mPCategories.append(Category{ -1, QObject::tr("All Packages") });
    for (QJsonArray::ConstIterator i = pCat.begin(); i != pCat.end(); ++i)
    {
        QJsonObject co = (*i).toObject();
        Category cat;
        cat.id = co.value("id").toInt();
        cat.name = co.value("n").toString();
        mPCategories.append(cat);
    }

    QJsonArray wCat = repo.value("wcategories").toArray();
    mWCategories.append(Category{ -1, QObject::tr("All Versions") });
    for (QJsonArray::ConstIterator i = wCat.begin(); i != wCat.end(); ++i)
    {
        QJsonObject co = (*i).toObject();
        Category cat;
        cat.id = co.value("id").toInt();
        cat.name = co.value("n").toString();
        mWCategories.append(cat);
    }

    QJsonArray files = repo.value("files").toArray();
    for (QJsonArray::ConstIterator i = files.begin(); i != files.end(); ++i)
    {
        QJsonObject fo = (*i).toObject();
        File file;
        file.id = fo.value("id").toInt();
        file.name = fo.value("n").toString();
        file.sum = fo.value("s").toString();
        QJsonArray mArr = fo.value("m").toArray();
        for (QJsonArray::ConstIterator i = mArr.begin(); i != mArr.end(); ++i)
        {
            QJsonObject mo = (*i).toObject();
            Mirror m;
            m.url = mo.value("u").toString();
            m.re = mo.value("r").toString();
            file.mirrors.append(m);
        }
        mFiles.append(file);
    }

    QJsonArray functions = repo.value("functions").toArray();
    for (QJsonArray::ConstIterator i = functions.begin(); i != functions.end(); ++i)
    {
        QJsonObject fo = (*i).toObject();
        Function f;
        f.name = fo.value("n").toString();
        f.body = fo.value("b").toString();
        if (f.name == "init_repo")
            mInit = f;
        else if (f.name == "done_repo")
            mDone = f;
        else
            mFunctions.append(f);
    }

    QJsonArray errors = repo.value("errors").toArray();
    for (QJsonArray::ConstIterator i = errors.begin(); i != errors.end(); ++i)
    {
        QJsonObject eo = (*i).toObject();
        Error e;
        e.re = eo.value("r").toString();
        e.package = eo.value("p").toInt();
        mErrors.append(e);
    }
}

const Repository::WineList &Repository::wines() const
{
    return mWines;
}

const Repository::PackageList &Repository::packages() const
{
    return mPackages;
}

const Repository::CategoryList &Repository::pCategories() const
{
    return mPCategories;
}

const Repository::CategoryList &Repository::wCategories() const
{
    return mWCategories;
}

Repository::Wine Repository::wine(int id) const
{
    for (const Wine &w : wines())
        if (w.id == id)
            return w;
    Wine w;
    w.id = -1;
    return w;
}

const Repository::FileList &Repository::files() const
{
    return mFiles;
}

Repository::File Repository::file(int id) const
{
    for (const File &f : files())
        if (f.id == id)
            return f;
    File f;
    f.id = -1;
    return f;
}

Repository::Package Repository::package(int id) const
{
    for (const Package &p : packages())
        if (p.id == id)
            return p;
    Package p;
    p.id = -1;
    return p;
}

Repository::PackageList Repository::packagesFromArr(const IntList &arr) const
{
    PackageList res;
    for (int i : arr)
        res.append(package(i));
    return res;
}

const Repository::FunctionList &Repository::functions() const
{
    return mFunctions;
}

const Repository::ErrorList &Repository::errors() const
{
    return mErrors;
}

const Repository::Function &Repository::init() const
{
    return mInit;
}

const Repository::Function &Repository::done() const
{
    return mDone;
}

bool Repository::Package::operator ==(const Repository::Package &p) const
{
    return id == p.id;
}

bool Repository::Wine::operator ==(const Repository::Package &p) const
{
    return id == p.id;
}
