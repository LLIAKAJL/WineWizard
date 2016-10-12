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

#include <QDirIterator>
#include <QApplication>
#include <QTextCodec>
#include <QSettings>
#include <QIcon>
#include <QUrl>
#include <QDir>

#include "solutionmodel.h"
#include "mainmodel.h"
#include "appmodel.h"
#include "wintypes.h"
#include "utils.h"

MainModel::MainModel(QObject *parent) :
    QAbstractItemModel(parent),
    mRoot(new Item)
{
    for (const QFileInfo &pi : Utils::data().entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir pd(pi.absoluteFilePath());
        QSettings settings(pd.absoluteFilePath(".settings"), QSettings::IniFormat);
        QString name = settings.value("Name", tr("* Noname *")).toString();
        QImage icon = settings.value("Icon", QImage(":/images/app")).value<QImage>();
        scanPrefix(new Item(pi.absoluteFilePath(), icon, name, mRoot));
    }
}

MainModel::~MainModel()
{
    delete mRoot;
    for (QProcess *p : mProcList)
    {
        if (p->state() == QProcess::Running)
        {
            p->disconnect();
            QProcess killProc;
            killProc.setProcessEnvironment(p->processEnvironment());
            killProc.setProcessChannelMode(QProcess::ForwardedChannels);
            QString script = Utils::readFile(":/scripts/terminate");
            killProc.start("sh", QStringList("-c") << script, QProcess::NotOpen);
            killProc.waitForStarted();
            killProc.waitForFinished();
            if (p->state() == QProcess::Running)
            {
                p->terminate();
                p->waitForFinished();
            }
        }
    }
}

int MainModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<Item *>(parent.internalPointer())->childCount();
    return mRoot->childCount();
}

int MainModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QModelIndex MainModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
    {
        Item *p = parent.isValid() ? static_cast<Item *>(parent.internalPointer()) : mRoot;
        return createIndex(row, column, p->child(row));
    }
    return QModelIndex();
}

QModelIndex MainModel::parent(const QModelIndex &child) const
{
    if (child.isValid())
    {
        Item *c = static_cast<Item *>(child.internalPointer());
        Item *p = c->parent();
        if (p && p->parent())
            return createIndex(p->row(), 0, p);
    }
    return QModelIndex();
}

bool MainModel::removeRows(int row, int, const QModelIndex &parent)
{
    if (row >= 0 && row < rowCount(parent))
    {
        beginRemoveRows(parent, row, row);
        Item *p = parent.isValid() ? static_cast<Item *>(parent.internalPointer()) : mRoot;
        QModelIndex i = index(row, 0, parent);
        QFileInfo info = i.data(PathRole).toString();
        if (info.exists())
        {
            if (info.isDir() && !info.isSymLink())
            {
                if (i.data(RunningRole).toBool())
                {
                    QString script = Utils::readFile(":/scripts/terminate");
                    release(script, i)->waitForFinished();
                }
                QDir(info.absoluteFilePath()).removeRecursively();
            }
            else
                QFile::remove(info.absoluteFilePath());
        }
        delete p->takeAt(row);
        endRemoveRows();
        return true;
    }
    return false;
}

QVariant MainModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        Item *i = static_cast<Item *>(index.internalPointer());
        switch (role)
        {
        case Qt::ForegroundRole:
            if (index.data(RunningRole).toBool())
                return QColor(Qt::red);
            return QVariant();
        case Qt::DisplayRole:
            if (i->running())
                return tr("%1 (* Running *)").arg(index.data(Qt::EditRole).toString());
            return index.data(Qt::EditRole);
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return i->name();
        case PathRole:
            return i->path();
        case Qt::DecorationRole:
            return i->icon();
        case RunningRole:
            return i->running();
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool MainModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        switch (role)
        {
        case RunningRole:
            if (static_cast<Item *>(index.internalPointer())->setRunning(value.toBool()))
            {
                emit dataChanged(index, index);
                return true;
            }
            return false;
        case ExecuteRole:
            if (value.toBool())
            {
                runShortcut(index);
                return true;
            }
            else if (index.data(RunningRole).toBool())
            {
                QString script = Utils::readFile(":/scripts/terminate");
                release(script, index);
                return true;
            }
            return false;
        case DebugRole:
            if (index.parent().isValid())
                runShortcut(index, true);
            else
                runInPrefix(index, value.value<QVariantMap>());
            return true;
        default:
            return false;
        }
    }
    return false;
}

MainModel::VarMap MainModel::itemData(const QModelIndex &index) const
{
    VarMap res;
    if (index.isValid())
    {
        if (index.parent().isValid())
        {
            Item *s = static_cast<Item *>(index.internalPointer());
            QString codepage = prefixCodepage(s->parent()->path());
            QFile f(s->path());
            f.open(QFile::ReadOnly);
            uchar *data = f.map(0, f.size());
            f.close();
            const LinkHeader *h = reinterpret_cast<LinkHeader *>(data);
            qint64 offset = sizeof(LinkHeader);
            if (h->dwFlags & SLDF_HAS_ID_LIST)
            {
                const LnkString *pidl = reinterpret_cast<LnkString *>(data + offset);
                offset += pidl->size + sizeof(WORD);
            }
            QString target;
            if (h->dwFlags & SLDF_HAS_LINK_INFO)
            {
                const LocationInfo *loc = reinterpret_cast<LocationInfo *>(data + offset);
                const char *p = reinterpret_cast<char *>(const_cast<LocationInfo *>(loc));
                if (loc->dwVolTableOfs && loc->dwVolTableOfs + sizeof(LocalVolumeInfo) < loc->dwTotalSize)
                {
                    if (loc->dwLocalPathOfs && (loc->dwLocalPathOfs < loc->dwTotalSize))
                    {
                        QTextCodec *codec = QTextCodec::codecForName("Windows-" + codepage.toUtf8());
                        if (codec)
                            target = cutQuotes(codec->toUnicode(&p[loc->dwLocalPathOfs]));
                    }
                }
                offset += loc->dwTotalSize;
            }
            target = toUnix(target, s->parent()->path());
            if (h->dwFlags & SLDF_HAS_NAME)
                readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE);
            if (h->dwFlags & SLDF_HAS_RELPATH)
                readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE);
            QString workDir;
            if (h->dwFlags & SLDF_HAS_WORKINGDIR)
                workDir = cutQuotes(readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE));
            if (workDir.isEmpty() && !target.isEmpty())
                workDir = QFileInfo(target).absolutePath();
            else
                workDir = toUnix(workDir, s->parent()->path());
            QString args;
            if (h->dwFlags & SLDF_HAS_ARGS)
                args = readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE);
            QString iconLoc;
            if (h->dwFlags & SLDF_HAS_ICONLOCATION)
                iconLoc = readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE);
            if (iconLoc.isEmpty() && !target.isEmpty())
                iconLoc = target;
            else
                iconLoc = toUnix(iconLoc, s->parent()->path());
            res.insert(Qt::EditRole, s->name());
            res.insert(Qt::DecorationRole, s->icon());
            res.insert(TargetRole, target);
            res.insert(ArgsRole, args);
            res.insert(WorkDirRole, workDir);
            res.insert(IconLocationRole, iconLoc);
        }
        else
        {
            Item *p = static_cast<Item *>(index.internalPointer());
            res.insert(Qt::EditRole, p->name());
            res.insert(Qt::DecorationRole, p->icon());
        }
    }
    return res;
}

bool MainModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    if (index.isValid())
    {
        if (index.parent().isValid())
        {
            if (data(index, TargetRole)         != roles.value(TargetRole)   ||
                data(index, ArgsRole)           != roles.value(ArgsRole)     ||
                data(index, WorkDirRole)        != roles.value(WorkDirRole)  ||
                data(index, Qt::EditRole)       != roles.value(Qt::EditRole) ||
                data(index, IconLocationRole) != roles.value(IconLocationRole))
            {
                Item *s = static_cast<Item *>(index.internalPointer());
                QString prefix = s->parent()->path();
                QString target = toWin(roles.value(TargetRole).toString(), prefix);
                QString workDir = toWin(roles.value(WorkDirRole).toString(), prefix);
                QString args = roles.value(ArgsRole).toString();
                QString name = roles.value(Qt::EditRole).toString();
                QString iconLocation = toWin(roles.value(IconLocationRole).toString(), prefix);
                s->setName(name);
                s->setIcon(roles.value(Qt::DecorationRole).value<QImage>());
                saveLink(s->path(), name, target, workDir, args, iconLocation);
                emit dataChanged(index, index);
                return true;
            }
        }
        else if (data(index, Qt::EditRole) != roles.value(Qt::EditRole) ||
                data(index, Qt::DecorationRole) != roles.value(Qt::DecorationRole))
        {
            Item *p = static_cast<Item *>(index.internalPointer());
            QString name = roles.value(Qt::EditRole).toString();
            QImage icon = roles.value(Qt::DecorationRole).value<QImage>();
            p->setName(name);
            p->setIcon(icon);
            QDir pd(p->path());
            QSettings s(pd.absoluteFilePath(".settings"), QSettings::IniFormat);
            s.setValue("Name", name);
            s.setValue("Icon", icon);
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

QModelIndex MainModel::newShortcut(const QFileInfo &targetInfo, const QModelIndex &prefix)
{
    Item *p = static_cast<Item *>(prefix.internalPointer());
    QString target = toWin(targetInfo.absoluteFilePath(), p->path());
    QString workDir = toWin(targetInfo.absolutePath(), p->path());
    QImage icon = Utils::extractIcon(targetInfo.absoluteFilePath());
    QString name = targetInfo.completeBaseName();
    QDir pd(p->path());
    if (!pd.exists(".links"))
        pd.mkdir(".links");
    QString path = pd.absoluteFilePath(".links/" + Utils::genID() + ".lnk");
    saveLink(path, name, target, workDir, QString(), target);
    int row = p->childCount();
    beginInsertRows(prefix, row, row);
    new Item(path, icon, name, p);
    endInsertRows();
    return index(row, 0, prefix);
}

void MainModel::procFinished()
{
    QProcess *proc = static_cast<QProcess *>(sender());
    mProcList.removeOne(proc);
    QString path = proc->property("path").toString();
    proc->deleteLater();
    if (!isRunning(path))
    {
        QModelIndexList l = match(index(0, 0), PathRole, path, -1, Qt::MatchFixedString);
        if (!l.isEmpty())
        {
            Item *p = static_cast<Item *>(l.first().internalPointer());
            scanPrefix(p);
            beginInsertRows(l.first(), 0, p->childCount() - 1);
            endInsertRows();
            setData(l.first(), false, RunningRole);
        }
    }
}

void MainModel::procReadyOutput()
{
    QProcess *proc = static_cast<QProcess *>(sender());
    QString path = proc->property("path").toString();
    emit readyOutput(path, proc->readAllStandardOutput());
}

void MainModel::procReadyError()
{
    QProcess *proc = static_cast<QProcess *>(sender());
    QString path = proc->property("path").toString();
    emit readyError(path, proc->readAllStandardError());
}

void MainModel::scanPrefix(Item *prefix)
{
    for (int i = prefix->childCount() - 1; i >= 0; --i)
        delete prefix->takeAt(i);
    QDir pd(prefix->path());
    QString codepage = prefixCodepage(prefix->path());
    scanDir(prefix, pd.absoluteFilePath(".links"), codepage);
    scanDir(prefix, pd.absoluteFilePath(".documents"), codepage);

    QDir users(pd.absoluteFilePath("dosdevices/c:/users"));
    if (users.exists())
    {
        for (const QFileInfo &i : users.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot))
        {
            QDir ud(i.absoluteFilePath());
            if (ud.exists("Start Menu"))
                scanDir(prefix, ud.absoluteFilePath("Start Menu"), codepage);
        }
    }
    if (prefix->icon().isNull())
        prefix->setIcon(QImage(":/images/app"));
}

QProcess *MainModel::execute(const QModelIndex &prefix, const QProcessEnvironment &env)
{
    setData(prefix, true, RunningRole);
    QProcess *proc = new QProcess(this);
    proc->setProperty("path", prefix.data(PathRole));
    mProcList.append(proc);
    QProcessEnvironment e = env;
    QString path = prefix.data(PathRole).toString();
    e.insert("WINEPREFIX", path);
    QString winePath = QDir(path).absoluteFilePath(".wine");
    e.insert("WINEVERPATH", winePath);
    e.insert("PATH", winePath + "/bin:" + e.value("PATH"));
    e.insert("WINESERVER", winePath + "/bin/wineserver");
    e.insert("WINELOADER", winePath + "/bin/wine");
    e.insert("WINEDLLPATH", winePath + "/lib/wine/fakedlls");
    e.insert("LD_LIBRARY_PATH", winePath + "/lib:" + e.value("LD_LIBRARY_PATH"));
    e.insert("WINEDLLOVERRIDES", "winemenubuilder.exe=n");
    proc->setProcessEnvironment(e);
    connect(proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &MainModel::procFinished);
    return proc;
}

QProcess *MainModel::debug(const QString &script, const QModelIndex &prefix, const QProcessEnvironment &env)
{
    QProcess *proc = execute(prefix, env);
    connect(proc, &QProcess::readyReadStandardOutput, this, &MainModel::procReadyOutput);
    connect(proc, &QProcess::readyReadStandardError, this, &MainModel::procReadyError);
    proc->start("sh", QStringList{ "-c", script }, QProcess::ReadOnly);
    proc->waitForStarted();
    return proc;
}

QProcess *MainModel::release(const QString &script, const QModelIndex &prefix, const QProcessEnvironment &env)
{
    QProcessEnvironment e = env;
    e.insert("WINEDEBUG", "-all");
    QProcess *proc = execute(prefix, e);
    proc->setProcessChannelMode(QProcess::ForwardedChannels);
    proc->start("sh", QStringList{ "-c", script }, QProcess::NotOpen);
    proc->waitForStarted();
    return proc;
}

void MainModel::install(const QByteArray &script, const QString &name, int appID,
                        const QString &path, const QString &exe, const QString &args,
                        const QString &workDir, const QString &bw, const QString &aw,
                        const QStringList &bp, const QStringList &ap)
{
    QModelIndexList l = match(index(0), PathRole, path, -1, Qt::MatchFixedString);
    if (!l.isEmpty())
        removeRow(l.first().row());
    QDir dir(path);
    dir.mkpath(path);
    QFile f(dir.absoluteFilePath("ww_install"));
    f.open(QFile::WriteOnly);
    f.write(script);
    f.close();
    QSettings(dir.absoluteFilePath(".settings"), QSettings::IniFormat).setValue("Name", name);
    int row = mRoot->childCount();
    beginInsertRows(QModelIndex(), row, row);
    new Item(path, QImage(":/images/app"), name, mRoot);
    endInsertRows();
    QProcessEnvironment e = QProcessEnvironment::systemEnvironment();
    e.insert("WW_EXE", exe);
    e.insert("WW_ARGS", args);
    e.insert("WW_WORK_DIR", workDir);
    e.insert("WW_BW", bw);
    e.insert("WW_AW", aw);
    e.insert("WW_APP_ID", QString::number(appID));
    if (!bp.isEmpty())
        e.insert("WW_BP", bp.join(QChar::Space));
    if (!ap.isEmpty())
        e.insert("WW_AP", ap.join(QChar::Space));
    debug(Utils::readFile(":/scripts/install"), index(row, 0), e);
}

void MainModel::runShortcut(const QModelIndex &index, bool debug)
{
    Item *s = static_cast<Item *>(index.internalPointer());
    QString exe = s->path();
    QString wd = QFileInfo(exe).absolutePath();
    QString script = QString(Utils::readFile(":/scripts/execute")).arg(exe).arg(wd).arg(QString());
    if (debug)
        this->debug(script, index.parent());
    else
        release(script, index.parent());
}

void MainModel::runInPrefix(const QModelIndex &index, const QVariantMap &params)
{
    QString exe = params.value("exe").toString();
    QString args = params.value("args").toString();
    QString wd = params.value("workDir").toString();
    QString script = QString(Utils::readFile(":/scripts/execute")).arg(exe).arg(wd).arg(args);
    release(script, index);
}

void MainModel::scanDir(Item *prefix, const QString &dir, const QString &codepage)
{
    if (!QFileInfo::exists(dir))
        return;
    QDirIterator i(dir, QStringList("*.lnk"), QDir::Files | QDir::Hidden, QDirIterator::Subdirectories);
    while (i.hasNext())
    {
        QString path = i.next();
        QFile f(path);
        f.open(QFile::ReadOnly);
        uchar *data = f.map(0, f.size());
        f.close();
        const LinkHeader *h = reinterpret_cast<LinkHeader *>(data);
        qint64 offset = sizeof(LinkHeader);
        if (h->dwFlags & SLDF_HAS_ID_LIST)
        {
            const LnkString *pidl = reinterpret_cast<LnkString *>(data + offset);
            offset += pidl->size + sizeof(WORD);
        }
        QString target, name, iconLoc;
        if (h->dwFlags & SLDF_HAS_LINK_INFO)
        {
            const LocationInfo *loc = reinterpret_cast<LocationInfo *>(data + offset);
            const char *p = reinterpret_cast<char *>(const_cast<LocationInfo *>(loc));
            if (loc->dwVolTableOfs && loc->dwVolTableOfs + sizeof(LocalVolumeInfo) < loc->dwTotalSize)
            {
                if (loc->dwLocalPathOfs && (loc->dwLocalPathOfs < loc->dwTotalSize))
                {
                    QTextCodec *codec = QTextCodec::codecForName("Windows-" + codepage.toUtf8());
                    if (codec)
                        target = cutQuotes(codec->toUnicode(&p[loc->dwLocalPathOfs]));
                }
            }
            offset += loc->dwTotalSize;
        }
        target = toUnix(target, prefix->path());
        if (h->dwFlags & SLDF_HAS_NAME)
            name = readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE);
        if (name.isEmpty())
            name = QFileInfo(path).completeBaseName();
        if (h->dwFlags & SLDF_HAS_RELPATH)
            readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE);
        if (h->dwFlags & SLDF_HAS_WORKINGDIR)
            readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE);
        if (h->dwFlags & SLDF_HAS_ARGS)
            readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE);
        if (h->dwFlags & SLDF_HAS_ICONLOCATION)
            iconLoc = readLnkString(offset, data, codepage, h->dwFlags & SLDF_UNICODE);
        if (iconLoc.isEmpty() && !target.isEmpty())
            iconLoc = target;
        if (iconLoc.isEmpty())
            new Item(path, QImage(":/images/app"), name, prefix);
        else
        {
            Item *s = new Item(path, Utils::extractIcon(toUnix(iconLoc, prefix->path())), name, prefix);
            if (prefix->icon() == QImage(":/images/app"))
                prefix->setIcon(s->icon());
        }
    }
}

QString MainModel::cutQuotes(const QString &str)
{
    if ((str.startsWith('\'') && str.endsWith('\'')) || (str.startsWith('"') && str.endsWith('"')))
        return str.mid(1, str.length() - 2);
    return str;
}

QString MainModel::readLnkString(qint64 &offset, uchar *data, const QString &codepage, bool unicode)
{
    const LnkString *str = reinterpret_cast<LnkString *>(data + offset);
    if (str)
    {
        WORD sz = str->size;
        QString res;
        if (unicode)
        {
            while (sz > 1)
                res += QChar(str->str.w[str->size - sz--]);
            offset += str->size * sizeof(WORD) + sizeof(WORD);
        }
        else
        {
            QByteArray arr;
            while (sz)
                arr += str->str.a[str->size - sz--];
            QTextCodec *codec = QTextCodec::codecForName("Windows-" + codepage.toUtf8());
            if (codec)
                res = codec->toUnicode(arr);
            else
                res = arr;
            offset += str->size + sizeof(WORD);
        }
        return res;
    }
    return QString();
}

void MainModel::writeLnkString(QFile &f, const QString &str)
{
    WORD sz = str.length() + 1;
    f.write(reinterpret_cast<char *>(&sz), sizeof(WORD));
    for (const QChar &c : str + '\0')
    {
        WORD wc = c.unicode();
        f.write(reinterpret_cast<char *>(&wc), sizeof(WORD));
    }
}

bool MainModel::isRunning(const QString &path) const
{
    for (const QProcess *p : mProcList)
        if (p->property("path") == path)
            return true;
    return false;
}

void MainModel::saveLink(const QString &path, const QString &name, const QString &winTarget,
                         const QString &winWorkDir, const QString &args, const QString &winIconLocation)
{
    QFile f(path);
    f.open(QFile::WriteOnly);
    LinkHeader hdr;
    hdr.dwFileAttr = 0;
    hdr.dwFileLength = 0;
    hdr.dwFlags = args.isEmpty() ? 0b11010110 : 0b11110110;
    hdr.dwSize = 0x0000004C;
    hdr.fStartup = SW_SHOWNORMAL;
    hdr.MagicGuid = CLSID_ShellLink;
    hdr.nIcon = 0;
    hdr.Time1 = { 0, 0 };
    hdr.Time2 = { 0, 0 };
    hdr.Time3 = { 0, 0 };
    hdr.Unknown5 = 0;
    hdr.Unknown6 = 0;
    hdr.wHotKey = 0;
    f.write(reinterpret_cast<char *>(&hdr), sizeof(hdr));
    LocationInfo loc;
    loc.dwHeaderSize = 0x0000001C;
    loc.dwFlags = 1;
    loc.dwVolTableOfs = loc.dwHeaderSize;
    loc.dwNetworkVolTableOfs = 0;
    loc.dwLocalPathOfs = loc.dwHeaderSize;
    loc.dwFinalPathOfs = winTarget.length() + loc.dwLocalPathOfs + 1;
    loc.dwTotalSize = loc.dwFinalPathOfs + 1;
    f.write(reinterpret_cast<char *>(&loc), sizeof(loc));
    f.write(winTarget.toUtf8() + '\0' + '\0');
    writeLnkString(f, name);
    writeLnkString(f, winWorkDir);
    if (!args.isEmpty())
        writeLnkString(f, args);
    writeLnkString(f, winIconLocation);
    f.close();
}

QString MainModel::toUnix(const QString &path, const QString &prefixPath)
{
    if (!path.isEmpty())
    {
        QString tmpPath = path;
        tmpPath.replace('\\', QDir::separator());
        QDir dev = QDir(prefixPath).absoluteFilePath("dosdevices");
        QString fullPath = dev.absoluteFilePath(tmpPath);
        if (QFile::exists(fullPath))
            return fullPath;
        QStringList items = tmpPath.split(QDir::separator(), QString::SkipEmptyParts);
        QString file = items.takeLast();
        for (const QString &item : items)
        {
            if (dev.exists(item))
                dev = dev.absoluteFilePath(item);
            else
            {
                QStringList entries = dev.entryList(QStringList(item), QDir::AllEntries);
                if (entries.isEmpty())
                    return path;
                dev = dev.absoluteFilePath(entries.first());
            }
        }
        if (dev.exists(file))
            return dev.absoluteFilePath(file);
        else
        {
            QStringList files = dev.entryList(QStringList(file));
            if (!files.isEmpty())
                return dev.absoluteFilePath(files.first());
        }
    }
    return path;
}

QString MainModel::toWin(const QString &path, const QString &prefixPath)
{
    if (!path.isEmpty())
    {
        QString res = path;
        QString drive;
        QDir dev = QDir(prefixPath).absoluteFilePath("dosdevices");
        for (const QFileInfo &d : dev.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System))
        {
            bool startWithName = res.startsWith(d.absoluteFilePath());
            bool startWithTarget = res.startsWith(d.symLinkTarget());
            if (d.isSymLink() && (startWithName || startWithTarget))
            {
                if (QDir(d.symLinkTarget()).isRoot())
                    drive = d.fileName();
                else
                {
                    if (startWithName)
                        res.replace(d.absoluteFilePath(), d.fileName().toUpper());
                    else if (startWithTarget)
                        res.replace(d.symLinkTarget(), d.fileName().toUpper());
                    return res.replace(QDir::separator(), '\\');
                }
            }
        }
        if (!drive.isEmpty())
        {
           res = drive.toUpper() + QDir::separator() + res;
           return res.replace(QDir::separator(), '\\');
        }
    }
    return path;
}

QString MainModel::prefixCodepage(const QString &prefixPath)
{
    QDir d(prefixPath);
    if (d.exists("system.reg"))
    {
        QRegExp re("\"ACP\"=\"(.*)\"");
        re.setMinimal(true);
        re.indexIn(Utils::readFile(d.absoluteFilePath("system.reg")));
        return re.cap(1);
    }
    return QString();
}

//------------------------------------------------------------------------------------------

MainModel::Item::Item(const QString &path, const QImage &icon, const QString &name, Item *parent) :
    mParent(parent),
    mName(name),
    mPath(path),
    mIcon(icon),
    mRunning(false)
{
    if (mParent)
        mParent->mChildren.append(this);
}

MainModel::Item::~Item()
{
    qDeleteAll(mChildren);
}

MainModel::Item *MainModel::Item::parent() const
{
    return mParent;
}

MainModel::Item *MainModel::Item::child(int row) const
{
    if (row >= 0 && row < childCount())
        return mChildren.at(row);
    return nullptr;
}

int MainModel::Item::childCount() const
{
    return mChildren.count();
}

int MainModel::Item::row() const
{
    if (mParent)
        return mParent->mChildren.indexOf(const_cast<Item *>(this));
    return 0;
}

MainModel::Item *MainModel::Item::takeAt(int row)
{
    return mChildren.takeAt(row);
}

const QString &MainModel::Item::name() const
{
    return mName;
}

bool MainModel::Item::setName(const QString &value)
{
    if (mName != value)
    {
        mName = value;
        return true;
    }
    return false;
}

const QString &MainModel::Item::path() const
{
    return mPath;
}

bool MainModel::Item::setPath(const QString &value)
{
    if (mPath != value)
    {
        mPath = value;
        return true;
    }
    return false;
}

const QImage &MainModel::Item::icon() const
{
    return mIcon;
}

bool MainModel::Item::setIcon(const QImage &value)
{
    if (mIcon != value)
    {
        mIcon = value;
        return true;
    }
    return false;
}

bool MainModel::Item::running() const
{
    return mRunning;
}

bool MainModel::Item::setRunning(bool value)
{
    if (mRunning != value)
    {
        mRunning = value;
        return true;
    }
    return false;
}
