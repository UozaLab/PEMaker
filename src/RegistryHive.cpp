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

#include "RegistryHive.h"

PrivilegeControl::PrivilegeControl()
: process_token(NULL)
{
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &process_token);
}

PrivilegeControl::~PrivilegeControl()
{
    if(process_token != NULL)
        CloseHandle(process_token);
}

BOOL PrivilegeControl::SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    if(process_token == NULL) return FALSE;

    TOKEN_PRIVILEGES tp;
    LUID luid;

    if(!LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
    {
        return FALSE; 
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    if(!AdjustTokenPrivileges(process_token, FALSE, &tp, 0, (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL))
    {
        return FALSE; 
    } 

    if(GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        return FALSE;
    } 

    return TRUE;
}

RegistryHiveData::RegistryHiveData(RegistryHive* _registry_hive, HKEY _parent_key, HKEY _key, LPCTSTR _subkey_name)
        : registry_hive(_registry_hive), parent_key(_parent_key), Key(_key)
{
    lstrcpy(subkey_name, _subkey_name);
}


RegistryHiveData::~RegistryHiveData()
{
    if(Key != NULL) RegCloseKey(Key);

    if(parent_key != NULL)
        RegUnLoadKey(parent_key, subkey_name);

    if(parent_key != NULL) RegCloseKey(parent_key);
    if(registry_hive != NULL) delete registry_hive;
}

RegistryHive::RegistryHive()
{
    privilege.SetPrivilege(SE_BACKUP_NAME, TRUE);
    privilege.SetPrivilege(SE_RESTORE_NAME, TRUE);
}

RegistryHive::~RegistryHive()
{
    privilege.SetPrivilege(SE_BACKUP_NAME, FALSE);
    privilege.SetPrivilege(SE_RESTORE_NAME, FALSE);
}

RegistryHiveData* RegistryHive::Load(HKEY key, LPCTSTR registry_file_path, LPCTSTR subkey_name)
{
    RegistryHive* registry_hive = new RegistryHive();

    HKEY hKey = NULL;
    HKEY hKey_hive = NULL;

    if(RegOpenKeyEx(key, L"", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) goto ERROR_END;
    if(RegLoadKey(hKey, subkey_name, registry_file_path) != ERROR_SUCCESS) goto ERROR_END;
    if(RegOpenKeyEx(hKey, subkey_name, 0, KEY_ALL_ACCESS, &hKey_hive) != ERROR_SUCCESS) goto ERROR_END;

    return new RegistryHiveData(registry_hive, hKey, hKey_hive, subkey_name);

ERROR_END:
    delete registry_hive;
    if(hKey != NULL) RegCloseKey(hKey);
    if(hKey_hive != NULL) RegCloseKey(hKey_hive);
    return NULL;
}
