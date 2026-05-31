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

#include "DiskInfo.h"
#include <setupapi.h>

shared_ptr<std::map<DWORD, std::map<DWORD, VolumeInfo>>> DiskInfoFactory::GetVolumeInfos()
{
    // map<DiskNumber, map<PartitionNumber, VolumeName>>
    shared_ptr<std::map<DWORD, std::map<DWORD, VolumeInfo>>> result(new std::map<DWORD, std::map<DWORD, VolumeInfo>>());

    TCHAR VolName[MAX_PATH+1];
    TCHAR VolNameNoBSlash[MAX_PATH+1];
    
    HANDLE hVolume = FindFirstVolume(VolName, MAX_PATH);
    if(hVolume == INVALID_HANDLE_VALUE) return result;

    do
    {
        VolumeInfo vi;
        _tcsncpy_s(VolNameNoBSlash, MAX_PATH+1, VolName, MAX_PATH+1);
        VolNameNoBSlash[_tcslen(VolNameNoBSlash) - 1] = _T('\0');

        HANDLE h = CreateFile(VolNameNoBSlash,
                              FILE_READ_ATTRIBUTES | SYNCHRONIZE | FILE_TRAVERSE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, 0, 0);
        if(h == INVALID_HANDLE_VALUE)
          continue;


        DWORD bytesReturned;
        STORAGE_DEVICE_NUMBER StorageDeviceNumber;
        if(!DeviceIoControl(h,
                            IOCTL_STORAGE_GET_DEVICE_NUMBER,
                            NULL,
                            0,
                            &StorageDeviceNumber,
                            sizeof(STORAGE_DEVICE_NUMBER),
                            &bytesReturned,
                            NULL))
        {
            CloseHandle(h);
            continue;
        }
        CloseHandle(h);

        vi.VolumeGUIDPath = tstring(VolNameNoBSlash);
		vi.StorageDeviceNumber = StorageDeviceNumber;
		(*result)[StorageDeviceNumber.DeviceNumber][StorageDeviceNumber.PartitionNumber] = vi;

        TCHAR VolumeName[MAX_PATH + 1];// UNICODE
        TCHAR FileSystemName[MAX_PATH + 1];
        DWORD SerialNumber;
        DWORD MaxFileNameLength;
        DWORD FileSystemFlags;
        if(!GetVolumeInformation(VolName,
                                 VolumeName,
                                 _countof(VolumeName),
                                 &SerialNumber,
                                 &MaxFileNameLength,
                                 &FileSystemFlags,
                                 FileSystemName,
                                 _countof(FileSystemName)))
        {
            if(GetLastError()!=ERROR_NOT_READY)
			{
				continue;
			}
        }

        vi.FilesystemInfo.TypeInfo.FileSystemName = tstring(FileSystemName);
        vi.FilesystemInfo.TypeInfo.VolumeName = tstring(VolumeName);
        (*result)[StorageDeviceNumber.DeviceNumber][StorageDeviceNumber.PartitionNumber] = vi;

        vi.DriveType = GetDriveType(VolName);

        DWORD length = 0;
        TCHAR PathNames[MAX_PATH + 1];
        if(!GetVolumePathNamesForVolumeName(VolName, PathNames, MAX_PATH, &length))
          continue;

        vi.PathNames = tstring(PathNames);
        (*result)[StorageDeviceNumber.DeviceNumber][StorageDeviceNumber.PartitionNumber] = vi;

    }while(FindNextVolume(hVolume, VolName, MAX_PATH));

    FindVolumeClose(hVolume);

    return result;
}
