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

#include <QStandardPaths>
#include <QJsonDocument>
#include <QMimeDatabase>
#include <QJsonObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QMimeType>
#include <QUuid>
#include <QIcon>

#include "wintypes.h"
#include "utils.h"

namespace Utils
{
    bool warning(const QString &message, QWidget *parent)
    {
        return QMessageBox::warning(parent, QObject::tr("Warning"), message,
                                    QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
    }

    void error(const QString &message, QWidget *parent)
    {
        QMessageBox::critical(parent, QObject::tr("Error"), message);
    }

    QString selectExe(const QString &dir, QWidget *parent)
    {
        QString filter = QObject::tr("Executable files (*.exe)");
        return QFileDialog::getOpenFileName(parent, QObject::tr("Select File"), dir,
                                   QObject::tr("Executable files (*.exe);;All files (*.*)"), &filter);
    }

    QString selectIcon(const QString &dir, QWidget *parent)
    {
        QString filter = QObject::tr("Images (*.png *.ico *.bmp *.jpg *.jpeg *.exe)");
        return QFileDialog::getOpenFileName(parent, QObject::tr("Select Icon"), dir,
                   QObject::tr("Images (*.png *.ico *.bmp *.jpg *.jpeg *.exe);;All files (*.*)"), &filter);
    }

    QString selectDir(const QString &dir, QWidget *parent)
    {
        return QFileDialog::getExistingDirectory(parent, QObject::tr("Select Directory"), dir);
    }

    QDir make(const QString &path)
    {
        QDir res(path);
        if (!res.exists())
            res.mkpath(res.absolutePath());
        return res;
    }

    QDir data()
    {
        return make(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    }

    QDir temp()
    {
        return make(QDir::temp().absoluteFilePath(".winewizard"));
    }

    QByteArray readFile(const QString &path)
    {
        if (QFile::exists(path))
        {
            QFile f(path);
            f.open(QFile::ReadOnly);
            return f.readAll();
        }
        return QByteArray();
    }

    QJsonObject readJson(const QString &path)
    {
        return QJsonDocument::fromJson(readFile(path)).object();
    }

    inline const IMAGE_RESOURCE_DIRECTORY *toIRD(void *p)
    {
        return reinterpret_cast<const IMAGE_RESOURCE_DIRECTORY *>(p);
    }

    inline const IMAGE_RESOURCE_DIRECTORY_ENTRY *toIRDE(void *p)
    {
        return reinterpret_cast<const IMAGE_RESOURCE_DIRECTORY_ENTRY *>(p);
    }

    inline const IMAGE_RESOURCE_DATA_ENTRY *toIRDataE(void *p)
    {
        return reinterpret_cast<const IMAGE_RESOURCE_DATA_ENTRY *>(p);
    }

    inline const GRPICONDIRENTRY *toGDE(void *p)
    {
        return reinterpret_cast<const GRPICONDIRENTRY *>(p);
    }

    inline const GRPICONDIR *toGID(void *p)
    {
        return reinterpret_cast<const GRPICONDIR *>(p);
    }

    QImage extractIcon(const QString &path)
    {
        QImage stdPixmap(path);
        if (!stdPixmap.isNull())
            return stdPixmap.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QMimeType mime = QMimeDatabase().mimeTypeForFile(path);
        QIcon icon = QIcon::fromTheme(mime.iconName());
        stdPixmap = icon.isNull() ? QImage(":/images/app") : icon.pixmap(32, 32).toImage();
        QFile f(path);
        if (!f.open(QFile::ReadOnly))
            return stdPixmap;
        uchar *data = f.map(0, f.size());
        f.close();
        const IMAGE_DOS_HEADER *dh = reinterpret_cast<const IMAGE_DOS_HEADER *>(data);
        if (dh->e_magic != IMAGE_DOS_SIGNATURE)
            return stdPixmap;
        qint64 offset = dh->e_lfanew;
        const DWORD *sign = reinterpret_cast<const DWORD *>(data + offset);
        if (*sign != IMAGE_NT_SIGNATURE)
            return stdPixmap;
        offset += sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER);
        const WORD *magic = reinterpret_cast<const WORD *>(data + offset);
        const IMAGE_DATA_DIRECTORY *dir = nullptr;
        int numOfSections = -1;
        if (*magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        {
            const IMAGE_NT_HEADERS32 *nth = reinterpret_cast<const IMAGE_NT_HEADERS32 *>(data + dh->e_lfanew);
            dir = nth->OptionalHeader.DataDirectory + IMAGE_DIRECTORY_ENTRY_RESOURCE;
            numOfSections = nth->FileHeader.NumberOfSections;
            offset += nth->FileHeader.SizeOfOptionalHeader;
        }
        else if (*magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
        {
            const IMAGE_NT_HEADERS64 *nth = reinterpret_cast<const IMAGE_NT_HEADERS64 *>(data + dh->e_lfanew);
            dir = nth->OptionalHeader.DataDirectory + IMAGE_DIRECTORY_ENTRY_RESOURCE;
            numOfSections = nth->FileHeader.NumberOfSections;
            offset += nth->FileHeader.SizeOfOptionalHeader;
        }
        else
            return stdPixmap;
        const IMAGE_SECTION_HEADER *h = reinterpret_cast<const IMAGE_SECTION_HEADER *>(data + offset);
        if (!dir)
            return stdPixmap;
        for (int i = 0; i < numOfSections; ++i)
        {
            if (h->VirtualAddress == dir->VirtualAddress)
            {
                QMap<DWORD, QPair<uchar *, DWORD>> icons;
                qint64 offset = h->PointerToRawData;
                const IMAGE_RESOURCE_DIRECTORY *ird1 = toIRD(data + offset);
                offset += sizeof(IMAGE_RESOURCE_DIRECTORY);
                const IMAGE_RESOURCE_DIRECTORY_ENTRY *irde1 = toIRDE(data + offset);
                for (int i = 0; i < ird1->NumberOfNamedEntries + ird1->NumberOfIdEntries; ++i)
                {
                    DWORD type = irde1->DUMMYUNIONNAME.Name;
                    if (type == RT_GROUP_ICON || type == RT_ICON)
                    {
                        qint64 offset = h->PointerToRawData;
                        offset += irde1->DUMMYUNIONNAME2.DUMMYSTRUCTNAME2.OffsetToDirectory;
                        const IMAGE_RESOURCE_DIRECTORY *ird2 = toIRD(data + offset);
                        offset += sizeof(IMAGE_RESOURCE_DIRECTORY);
                        const IMAGE_RESOURCE_DIRECTORY_ENTRY *irde2 = toIRDE(data + offset);
                        for (int i = 0; i < ird2->NumberOfIdEntries + ird2->NumberOfNamedEntries; ++i)
                        {
                            DWORD id = irde2->DUMMYUNIONNAME.Name;
                            qint64 offset = h->PointerToRawData;
                            offset += irde2->DUMMYUNIONNAME2.DUMMYSTRUCTNAME2.OffsetToDirectory;
                            const IMAGE_RESOURCE_DIRECTORY *ird3 = toIRD(data + offset);
                            offset += sizeof(IMAGE_RESOURCE_DIRECTORY);
                            const IMAGE_RESOURCE_DIRECTORY_ENTRY *irde3 = toIRDE(data + offset);
                            for (int i = 0; i < ird3->NumberOfNamedEntries + ird3->NumberOfIdEntries; ++i)
                            {
                                qint64 offset = h->PointerToRawData;
                                offset += irde3->DUMMYUNIONNAME2.OffsetToData;
                                const IMAGE_RESOURCE_DATA_ENTRY *de = toIRDataE(data + offset);
                                offset = de->OffsetToData - h->VirtualAddress + h->PointerToRawData;
                                if (type == RT_ICON)
                                    icons.insert(id, qMakePair(data + offset, de->Size));
                                else if (type == RT_GROUP_ICON)
                                {
                                    const GRPICONDIR *gid = toGID(data + offset);
                                    offset += sizeof(GRPICONDIR);
                                    const GRPICONDIRENTRY *gideTmp = toGDE(data + offset);
                                    ICONDIRENTRY entry{ 0, 0, 0, 0, 0, 0, 0, 0 };
                                    WORD nID = -1;
                                    for (int i = 0; i < gid->idCount; ++i)
                                    {
                                        if (gideTmp->bWidth >= entry.bWidth && gideTmp->wBitCount >= entry.wBitCount)
                                        {
                                            entry.bColorCount = gideTmp->bColorCount;
                                            entry.bHeight = gideTmp->bHeight;
                                            entry.bReserved = gideTmp->bReserved;
                                            entry.bWidth = gideTmp->bWidth;
                                            entry.dwBytesInRes = gideTmp->dwBytesInRes;
                                            entry.wBitCount = gideTmp->wBitCount;
                                            entry.wPlanes = gideTmp->wPlanes;
                                            nID = gideTmp->nID;
                                        }
                                        ++gideTmp;
                                    }
                                    if (entry.bWidth > 0)
                                    {
                                        QByteArray data;
                                        ICONDIR icoHdr{ 0, 1, 1 };
                                        data.append(reinterpret_cast<char *>(&icoHdr), sizeof(ICONDIR));
                                        entry.dwImageOffset = sizeof(ICONDIR) + sizeof(ICONDIRENTRY);
                                        data.append(reinterpret_cast<char *>(&entry), sizeof(ICONDIRENTRY));
                                        QPair<uchar *, DWORD> res = icons.value(nID);
                                        data.append(QByteArray(reinterpret_cast<char *>(res.first), res.second));
                                        QImage p;
                                        if (p.loadFromData(data, "ICO"))
                                            return p.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                                    }
                                }
                                ++irde3;
                            }
                            ++irde2;
                        }
                    }
                    ++irde1;
                }
                return stdPixmap;
            }
            ++h;
        }
        return QImage();
    }

    QString genID()
    {
        return QUuid::createUuid().toString();
    }
}
