/*
 * Copyright (c) 2024-2026, Uoza Lab
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

#ifndef __RegistryHive_H___
#define __RegistryHive_H___

#include <windows.h>
#include <tchar.h>

class PrivilegeControl
{
private:
    HANDLE process_token;
public:
    PrivilegeControl();
    ~PrivilegeControl();
    bool IsValid(){return process_token != NULL;}
    BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
};

class RegistryHive;
class RegistryHiveData
{
private:
    RegistryHive* registry_hive;
    HKEY parent_key;
    TCHAR subkey_name[256];

public:
    HKEY Key;
public:
    RegistryHiveData(RegistryHive* _registry_hive, HKEY _parent_key, HKEY _key, LPCTSTR _subkey_name);
    ~RegistryHiveData();
};

class RegistryHive
{
private:
    PrivilegeControl privilege;
    RegistryHive();
public:
    ~RegistryHive();
    static RegistryHiveData* Load(HKEY key, LPCTSTR registry_file_path, LPCTSTR subkey_name);
    static RegistryHiveData* Load(LPCTSTR registry_file_path, LPCTSTR subkey_name)
    {
        return Load(HKEY_USERS, registry_file_path, subkey_name);
    }
};

#endif
