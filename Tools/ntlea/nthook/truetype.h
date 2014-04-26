
#ifndef __TRUETYPE_H__
#define __TRUETYPE_H__

// see: https://www.microsoft.com/typography/otspec/name.htm
// see: http://hi.baidu.com/fanr520/item/6461128f03201d874514cf2e
// see: http://www.codeproject.com/Articles/2293/Retrieving-font-name-from-TTF-file

#include <Windows.h>

#pragma pack(1)

typedef struct
{
	char		tag[4];
	ULONG		checkSum;
	ULONG		offset;
	ULONG		length;
} TableEntry, TT_OFFSET_TABLE;

typedef struct
{
	USHORT		jajorVersion;		// i.e. sfntversion, 0x10000 for version 1.0
	USHORT		minorVersion;
	USHORT		numTables;
	USHORT		searchRange;
	USHORT		entrySelector;
	USHORT		rangeShift;
	TableEntry	entries[1];
} TableDirectory, TT_TABLE_DIRECTORY;

typedef struct
{
	USHORT		uFSelector;			// format selector. Always 0
	USHORT		uNRCount;			// Name Records count
	USHORT		uStorageOffset;		// Offset for strings storage, 
	// from start of the table
} NameTableHeader, TT_NAME_TABLE_HEADER;

typedef struct
{
	USHORT		uPlatformID;
	USHORT		uEncodingID;
	USHORT		uLanguageID;		// 
	USHORT		uNameID;			// 1 says that this is font name. 0 for example determines copyright info. 4 for fullname
	USHORT		uStringLength;
	USHORT		uStringOffset;
	// from start of strings storage area
} NameRecord, TT_NAME_RECORD;

#pragma pack()

#endif // __TRUETYPE_H__
