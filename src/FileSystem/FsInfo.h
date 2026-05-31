/*
 * Copyright (c) 2024-2026, UozaLab
 *
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __FSINFO__H__
#define __FSINFO__H__

#include <windows.h>
#include "tstring.h"

enum PARTITION_TYPE
{
    PART_TYPE_Empty = 0x00,
    PART_TYPE_FAT12,
    PART_TYPE_Extended = 0x05,
    PART_TYPE_FAT16,
    PART_TYPE_NTFS_exFAT,
    PART_TYPE_FAT32 = 0x0B,

    PART_TYPE_BitLocker = 0xFF
};

enum FS_TYPE
{
    FS_TYPE_UNKNOWN,
    FS_TYPE_FAT12,
    FS_TYPE_FAT16,
    FS_TYPE_FAT32,
    FS_TYPE_exFAT,
    FS_TYPE_NTFS,
};


struct VOLUME_TYPE_INFO
{
    UINT8 PartitionType;
    tstring FileSystemName;
    FS_TYPE FileSystemType;
    tstring VolumeName;

    VOLUME_TYPE_INFO()
    {
        PartitionType = 0;
        FileSystemType = FS_TYPE_UNKNOWN;
        FileSystemName = _T("");
        VolumeName = _T("");
    }

};

struct VOLUME_SIZE_INFO
{
    bool SizeCalculated;
    ULONGLONG Total;//Byte
    ULONGLONG Used;//Byte
    ULONGLONG Fixed;//Byte
    double UsedRatio;

    UINT32 SectorsPerCluster;
    UINT32 BytesPerSector;

    VOLUME_SIZE_INFO()
    {
        SizeCalculated = false;
        UsedRatio = 0.0;
        SectorsPerCluster = 0;
        BytesPerSector = 0;
        Total = 0ULL;
        Used = 0ULL;
        Fixed = 0ULL;
    }
};

struct FSInfo
{
    VOLUME_TYPE_INFO TypeInfo;
    VOLUME_SIZE_INFO SizeInfo;
};



#endif
