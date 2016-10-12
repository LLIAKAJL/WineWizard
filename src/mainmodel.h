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

#ifndef MAINMODEL_H
#define MAINMODEL_H

#include <QAbstractItemModel>
#include <QFileInfo>
#include <QMimeData>
#include <QProcess>
#include <QPixmap>
#include <QWizard>

class MainModel : public QAbstractItemModel
{
    Q_OBJECT

    typedef QList<QProcess *> ProcessList;
    typedef QMap<int, QVariant> VarMap;

    class Item
    {
    public:
        explicit Item(const QString &hash = QString(), const QImage &icon = QImage(),
                      const QString &name = QString(), Item *parent = nullptr);
        ~Item();

        Item *parent() const;
        Item *child(int row) const;
        int childCount() const;
        int row() const;
        Item *takeAt(int row);

        const QString &name() const;
        bool setName(const QString &value);
        const QString &path() const;
        bool setPath(const QString &value);
        const QImage &icon() const;
        bool setIcon(const QImage &value);
        bool running() const;
        bool setRunning(bool value);

    private:
        Item *mParent;
        QList<Item *> mChildren;
        QString mName, mPath;
        QImage mIcon;
        bool mRunning;
    };

public:
    enum
    {
        TargetRole = Qt::UserRole + 1,
        ArgsRole,
        WorkDirRole,
        IconLocationRole,
        RunningRole,
        PathRole,
        DebugRole,
        ExecuteRole
    };

    explicit MainModel(QObject *parent = nullptr);
    ~MainModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    VarMap itemData(const QModelIndex &index) const override;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;

    QModelIndex newShortcut(const QFileInfo &targetInfo, const QModelIndex &prefix);
    void install(const QByteArray &script, const QString &name, int appID,
                 const QString &path, const QString &exe, const QString &args, const QString &workDir,
                 const QString &bw, const QString &aw, const QStringList &bp, const QStringList &ap);

signals:
    void readyOutput(const QString &path, const QString &data);
    void readyError(const QString &path, const QString &data);

private slots:
    void procFinished();
    void procReadyOutput();
    void procReadyError();

private:
    Item *mRoot;
    ProcessList mProcList;

    QProcess *execute(const QModelIndex &prefix, const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment());
    QProcess *debug(const QString &script, const QModelIndex &prefix, const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment());
    QProcess *release(const QString &script, const QModelIndex &prefix, const QProcessEnvironment &env = QProcessEnvironment::systemEnvironment());
    void runShortcut(const QModelIndex &index, bool debug = false);
    void runInPrefix(const QModelIndex &index, const QVariantMap &params);
    bool isRunning(const QString &path) const;

    static void scanPrefix(Item *prefix);
    static void scanDir(Item *prefix, const QString &dir, const QString &codepage);
    static QString cutQuotes(const QString &str);
    static QString readLnkString(qint64 &offset, uchar *data, const QString &codepage, bool unicode);
    static void saveLink(const QString &path, const QString &name, const QString &winTarget,
                          const QString &winWorkDir, const QString &args, const QString &winIconLocation);
    static void writeLnkString(QFile &f, const QString &str);
    static QString toUnix(const QString &path, const QString &prefixPath);
    static QString toWin(const QString &path, const QString &prefixPath);
    static QString prefixCodepage(const QString &prefixPath);
};

#endif // MAINMODEL_H
