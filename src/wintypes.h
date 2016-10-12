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

#ifndef WINTYPES_H
#define WINTYPES_H

typedef void                                   *LPVOID;
typedef const void                             *LPCVOID;
typedef int             BOOL,       *PBOOL,    *LPBOOL;
typedef unsigned char   BYTE,       *PBYTE,    *LPBYTE;
typedef unsigned char   UCHAR,      *PUCHAR;
typedef unsigned short  WORD,       *PWORD,    *LPWORD;
typedef unsigned short  USHORT,     *PUSHORT;
typedef int             INT,        *PINT,     *LPINT;
typedef unsigned int    UINT,       *PUINT;
typedef float           FLOAT,      *PFLOAT;
typedef char                        *PSZ;
typedef int                                    *LPLONG;
typedef unsigned int    DWORD,      *PDWORD,   *LPDWORD;
typedef unsigned int    ULONG,      *PULONG;
typedef qint64          ULONGLONG;

typedef ULONG           UCSCHAR;
#define MIN_UCSCHAR                 (0)
#define MAX_UCSCHAR                 (0x0010ffff)
#define UCSCHAR_INVALID_CHARACTER   (0xffffffff)

#ifndef VOID
#define VOID void
#endif
typedef VOID           *PVOID;
typedef VOID           *PVOID64;
typedef BYTE            BOOLEAN,    *PBOOLEAN;
typedef char            CHAR,       *PCHAR;
typedef short           SHORT,      *PSHORT;
typedef int             LONG,       *PLONG;

/* ANSI string types */
typedef CHAR           *PCH,        *LPCH,      *PNZCH;
typedef const CHAR     *PCCH,       *LPCCH,     *PCNZCH;
typedef CHAR           *PSTR,       *LPSTR,     *NPSTR;
typedef const CHAR     *PCSTR,      *LPCSTR;
typedef CHAR           *PZZSTR;
typedef const CHAR     *PCZZSTR;

typedef unsigned short  WCHAR,      *PWCHAR;

/* Unicode string types */
typedef const WCHAR    *PCWCHAR,    *LPCUWCHAR, *PCUWCHAR;
typedef WCHAR          *PWCH,       *LPWCH;
typedef const WCHAR    *PCWCH,      *LPCWCH;
typedef WCHAR          *PNZWCH,     *PUNZWCH;
typedef const WCHAR    *PCNZWCH,    *PCUNZWCH;
typedef WCHAR          *PWSTR,      *LPWSTR,    *NWPSTR;
typedef const WCHAR    *PCWSTR,     *LPCWSTR;
typedef WCHAR          *PZZWSTR,    *PUZZWSTR;
typedef const WCHAR    *PCZZWSTR,   *PCUZZWSTR;
typedef PWSTR          *PZPWSTR;
typedef PCWSTR         *PZPCWSTR;

typedef long          INT_PTR, *PINT_PTR;
typedef unsigned long UINT_PTR, *PUINT_PTR;
typedef long          LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR     DWORD_PTR, *PDWORD_PTR;

typedef struct _GUID
{
    DWORD Data1;
    WORD  Data2;
    WORD  Data3;
    BYTE  Data4[8];
} GUID;

#define MAX_PATH 0x00000104

#define ALIGN_DOWN(x, align) (x & ~(align - 1))
#define ALIGN_UP(x, align) ((x & (align - 1)) ? ALIGN_DOWN(x, align) + align : x)

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

#define IMAGE_DOS_SIGNATURE    0x5A4D             /* MZ   */
#define IMAGE_OS2_SIGNATURE    0x454E             /* NE   */
#define IMAGE_OS2_SIGNATURE_LE 0x454C             /* LE   */
#define IMAGE_OS2_SIGNATURE_LX 0x584C             /* LX */
#define IMAGE_VXD_SIGNATURE    0x454C             /* LE   */
#define IMAGE_NT_SIGNATURE     0x00004550         /* PE00 */

#define IMAGE_MSI_SIGNATUE     0xE11AB1A1E011CFD0

/* Possible Magic values */
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC      0x10b
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC      0x20b
#define IMAGE_ROM_OPTIONAL_HDR_MAGIC       0x107

/* These defines are for the Characteristics bitfield. */

#define IMAGE_SCN_CNT_INITIALIZED_DATA   0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA 0x00000080
#define IMAGE_SCN_LNK_NRELOC_OVFL        0x01000000
#define IMAGE_SCN_MEM_DISCARDABLE	     0x02000000
#define IMAGE_SCN_MEM_NOT_CACHED	     0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED		     0x08000000
#define IMAGE_SCN_MEM_SHARED		     0x10000000
#define IMAGE_SCN_MEM_EXECUTE		     0x20000000
#define IMAGE_SCN_MEM_READ			     0x40000000
#define IMAGE_SCN_MEM_WRITE			     0x80000000

/* Predefined resource types */
#define RT_CURSOR         1
#define RT_BITMAP         2
#define RT_ICON           3
#define RT_MENU           4
#define RT_DIALOG         5
#define RT_STRING         6
#define RT_FONTDIR        7
#define RT_FONT           8
#define RT_ACCELERATOR    9
#define RT_RCDATA         10
#define RT_MESSAGETABLE   11
#define RT_GROUP_CURSOR   12
#define RT_GROUP_ICON     14
#define RT_VERSION        16
#define RT_DLGINCLUDE     17
#define RT_PLUGPLAY       19
#define RT_VXD            20
#define RT_ANICURSOR      21
#define RT_ANIICON        22
#define RT_HTML           23

/* Directory Entries, indices into the DataDirectory array */
#define	IMAGE_DIRECTORY_ENTRY_EXPORT		 0
#define	IMAGE_DIRECTORY_ENTRY_IMPORT		 1
#define	IMAGE_DIRECTORY_ENTRY_RESOURCE		 2
#define	IMAGE_DIRECTORY_ENTRY_EXCEPTION		 3
#define	IMAGE_DIRECTORY_ENTRY_SECURITY		 4
#define	IMAGE_DIRECTORY_ENTRY_BASERELOC		 5
#define	IMAGE_DIRECTORY_ENTRY_DEBUG		     6
#define	IMAGE_DIRECTORY_ENTRY_COPYRIGHT		 7
#define	IMAGE_DIRECTORY_ENTRY_GLOBALPTR		 8
#define	IMAGE_DIRECTORY_ENTRY_TLS		     9
#define	IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG	 10
#define	IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT	 11
#define	IMAGE_DIRECTORY_ENTRY_IAT		     12
#define	IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT	 13
#define	IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14

typedef struct _IMAGE_DOS_HEADER
{
    WORD  e_magic;
    WORD  e_cblp;
    WORD  e_cp;
    WORD  e_crlc;
    WORD  e_cparhdr;
    WORD  e_minalloc;
    WORD  e_maxalloc;
    WORD  e_ss;
    WORD  e_sp;
    WORD  e_csum;
    WORD  e_ip;
    WORD  e_cs;
    WORD  e_lfarlc;
    WORD  e_ovno;
    WORD  e_res[4];
    WORD  e_oemid;
    WORD  e_oeminfo;
    WORD  e_res2[10];
    DWORD e_lfanew;
}  IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER
{
    WORD  Machine;
    WORD  NumberOfSections;
    DWORD TimeDateStamp;
    DWORD PointerToSymbolTable;
    DWORD NumberOfSymbols;
    WORD  SizeOfOptionalHeader;
    WORD  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY
{
    DWORD VirtualAddress;
    DWORD Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

typedef struct _IMAGE_OPTIONAL_HEADER64
{
    WORD  Magic;
    BYTE MajorLinkerVersion;
    BYTE MinorLinkerVersion;
    DWORD SizeOfCode;
    DWORD SizeOfInitializedData;
    DWORD SizeOfUninitializedData;
    DWORD AddressOfEntryPoint;
    DWORD BaseOfCode;
    ULONGLONG ImageBase;
    DWORD SectionAlignment;
    DWORD FileAlignment;
    WORD MajorOperatingSystemVersion;
    WORD MinorOperatingSystemVersion;
    WORD MajorImageVersion;
    WORD MinorImageVersion;
    WORD MajorSubsystemVersion;
    WORD MinorSubsystemVersion;
    DWORD Win32VersionValue;
    DWORD SizeOfImage;
    DWORD SizeOfHeaders;
    DWORD CheckSum;
    WORD Subsystem;
    WORD DllCharacteristics;
    ULONGLONG SizeOfStackReserve;
    ULONGLONG SizeOfStackCommit;
    ULONGLONG SizeOfHeapReserve;
    ULONGLONG SizeOfHeapCommit;
    DWORD LoaderFlags;
    DWORD NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS64
{
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

typedef struct _IMAGE_OPTIONAL_HEADER
{
    WORD  Magic;
    BYTE  MajorLinkerVersion;
    BYTE  MinorLinkerVersion;
    DWORD SizeOfCode;
    DWORD SizeOfInitializedData;
    DWORD SizeOfUninitializedData;
    DWORD AddressOfEntryPoint;
    DWORD BaseOfCode;
    DWORD BaseOfData;
    DWORD ImageBase;
    DWORD SectionAlignment;
    DWORD FileAlignment;
    WORD  MajorOperatingSystemVersion;
    WORD  MinorOperatingSystemVersion;
    WORD  MajorImageVersion;
    WORD  MinorImageVersion;
    WORD  MajorSubsystemVersion;
    WORD  MinorSubsystemVersion;
    DWORD Win32VersionValue;
    DWORD SizeOfImage;
    DWORD SizeOfHeaders;
    DWORD CheckSum;
    WORD  Subsystem;
    WORD  DllCharacteristics;
    DWORD SizeOfStackReserve;
    DWORD SizeOfStackCommit;
    DWORD SizeOfHeapReserve;
    DWORD SizeOfHeapCommit;
    DWORD LoaderFlags;
    DWORD NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_NT_HEADERS
{
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

#ifdef __x86_64__
typedef IMAGE_NT_HEADERS64  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS64 PIMAGE_NT_HEADERS;
typedef IMAGE_OPTIONAL_HEADER64 IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER64 PIMAGE_OPTIONAL_HEADER;
#else
typedef IMAGE_NT_HEADERS32  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS32 PIMAGE_NT_HEADERS;
typedef IMAGE_OPTIONAL_HEADER32 IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER32 PIMAGE_OPTIONAL_HEADER;
#endif

#define IMAGE_SIZEOF_SHORT_NAME 8

typedef struct _IMAGE_SECTION_HEADER
{
    BYTE  Name[IMAGE_SIZEOF_SHORT_NAME];
    union
    {
        DWORD PhysicalAddress;
        DWORD VirtualSize;
    } Misc;
    DWORD VirtualAddress;
    DWORD SizeOfRawData;
    DWORD PointerToRawData;
    DWORD PointerToRelocations;
    DWORD PointerToLinenumbers;
    WORD  NumberOfRelocations;
    WORD  NumberOfLinenumbers;
    DWORD Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

#define	IMAGE_SIZEOF_SECTION_HEADER 40

typedef struct _IMAGE_RESOURCE_DIRECTORY
{
    DWORD	Characteristics;
    DWORD	TimeDateStamp;
    WORD	MajorVersion;
    WORD	MinorVersion;
    WORD	NumberOfNamedEntries;
    WORD	NumberOfIdEntries;
} IMAGE_RESOURCE_DIRECTORY,*PIMAGE_RESOURCE_DIRECTORY;

#define	IMAGE_RESOURCE_NAME_IS_STRING		0x80000000
#define	IMAGE_RESOURCE_DATA_IS_DIRECTORY	0x80000000

typedef struct _IMAGE_RESOURCE_DIRECTORY_ENTRY
{
    union
    {
        struct
        {
#ifdef BITFIELDS_BIGENDIAN
            unsigned NameIsString:1;
            unsigned NameOffset:31;
#else
            unsigned NameOffset:31;
            unsigned NameIsString:1;
#endif
        } DUMMYSTRUCTNAME;
        DWORD   Name;
#ifdef WORDS_BIGENDIAN
        WORD    __pad;
        WORD    Id;
#else
        WORD    Id;
        WORD    __pad;
#endif
    } DUMMYUNIONNAME;
    union
    {
        DWORD   OffsetToData;
        struct
        {
#ifdef BITFIELDS_BIGENDIAN
            unsigned DataIsDirectory:1;
            unsigned OffsetToDirectory:31;
#else
            unsigned OffsetToDirectory:31;
            unsigned DataIsDirectory:1;
#endif
        } DUMMYSTRUCTNAME2;
    } DUMMYUNIONNAME2;
} IMAGE_RESOURCE_DIRECTORY_ENTRY,*PIMAGE_RESOURCE_DIRECTORY_ENTRY;

typedef struct _IMAGE_RESOURCE_DIRECTORY_STRING
{
    WORD	Length;
    CHAR	NameString[1];
} IMAGE_RESOURCE_DIRECTORY_STRING,*PIMAGE_RESOURCE_DIRECTORY_STRING;

typedef struct _IMAGE_RESOURCE_DIR_STRING_U
{
    WORD	Length;
    WCHAR	NameString[1];
} IMAGE_RESOURCE_DIR_STRING_U,*PIMAGE_RESOURCE_DIR_STRING_U;

typedef struct _IMAGE_RESOURCE_DATA_ENTRY
{
    DWORD	OffsetToData;
    DWORD	Size;
    DWORD	CodePage;
    DWORD	Reserved;
} IMAGE_RESOURCE_DATA_ENTRY,*PIMAGE_RESOURCE_DATA_ENTRY;

#pragma pack(push)
#pragma pack(2)
typedef struct
{
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    WORD nID;
} GRPICONDIRENTRY;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(2)
typedef struct
{
    WORD idReserved;
    WORD idType;
    WORD idCount;
} GRPICONDIR;
#pragma pack(pop)

typedef struct
{
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
} ICONDIRENTRY;

typedef struct
{
    WORD idReserved;
    WORD idType;
    WORD idCount;
} ICONDIR;

enum SHELL_LINK_DATA_FLAGS
{
    SLDF_HAS_ID_LIST                           = 0x00000001,
    SLDF_HAS_LINK_INFO                         = 0x00000002,
    SLDF_HAS_NAME                              = 0x00000004,
    SLDF_HAS_RELPATH                           = 0x00000008,
    SLDF_HAS_WORKINGDIR                        = 0x00000010,
    SLDF_HAS_ARGS                              = 0x00000020,
    SLDF_HAS_ICONLOCATION                      = 0x00000040,
    SLDF_UNICODE                               = 0x00000080,
    SLDF_FORCE_NO_LINKINFO                     = 0x00000100,
    SLDF_HAS_EXP_SZ                            = 0x00000200,
    SLDF_RUN_IN_SEPARATE                       = 0x00000400,
    SLDF_HAS_LOGO3ID                           = 0x00000800,
    SLDF_HAS_DARWINID                          = 0x00001000,
    SLDF_RUNAS_USER                            = 0x00002000,
    SLDF_HAS_EXP_ICON_SZ                       = 0x00004000,
    SLDF_NO_PIDL_ALIAS                         = 0x00008000,
    SLDF_FORCE_UNCNAME                         = 0x00010000,
    SLDF_RUN_WITH_SHIMLAYER                    = 0x00020000,
    SLDF_FORCE_NO_LINKTRACK                    = 0x00040000,
    SLDF_ENABLE_TARGET_METADATA                = 0x00080000,
    SLDF_DISABLE_KNOWNFOLDER_RELATIVE_TRACKING = 0x00200000,
    SLDF_RESERVED                              = 0x80000000,
};

#define EXP_SZ_LINK_SIG         0xa0000001
#define EXP_SPECIAL_FOLDER_SIG  0xa0000005
#define EXP_DARWIN_ID_SIG       0xa0000006
#define EXP_SZ_ICON_SIG         0xa0000007
#define EXP_PROPERTYSTORAGE_SIG 0xa0000009

struct FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};

struct LinkHeader
{
    DWORD    dwSize;
    GUID     MagicGuid;
    DWORD    dwFlags;
    DWORD    dwFileAttr;
    FILETIME Time1;
    FILETIME Time2;
    FILETIME Time3;
    DWORD    dwFileLength;
    DWORD    nIcon;
    DWORD    fStartup;
    DWORD    wHotKey;
    DWORD    Unknown5;
    DWORD    Unknown6;
};

#define SW_SHOWNORMAL      0x00000001
#define SW_SHOWMAXIMIZED   0x00000003
#define SW_SHOWMINNOACTIVE 0x00000007

static const GUID CLSID_ShellLink = {0x00021401L, 0, 0, {0xC0,0,0,0,0,0,0,0x46}};

struct LocationInfo
{
    DWORD  dwTotalSize;
    DWORD  dwHeaderSize;
    DWORD  dwFlags;
    DWORD  dwVolTableOfs;
    DWORD  dwLocalPathOfs;
    DWORD  dwNetworkVolTableOfs;
    DWORD  dwFinalPathOfs;
};

struct LocalVolumeInfo
{
    DWORD dwSize;
    DWORD dwType;
    DWORD dwVolSerial;
    DWORD dwVolLabelOfs;
};

struct NetworkVolumeInfo
{
    DWORD dwSize;
    DWORD dwUnknown1;
    DWORD dwShareNameOfs;
    DWORD dwReserved;
    DWORD dwUnknown2;
};

struct LnkString
{
    WORD size;
    union
    {
        WCHAR w[1];
        UCHAR a[1];
    } str;
};

#endif // WINTYPES_H
