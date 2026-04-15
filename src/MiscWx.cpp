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

#include "MiscWx.h"
#include "resource.h"
#include "resource_lang.h"
#include "tstring.h"
#include <wx/stdpaths.h>

void Utility::SetIcon(SimpleButton* button, int iconid, wxString text)
{
    HRSRC hbin = FindResource(NULL, MAKEINTRESOURCE(iconid), RT_RCDATA);
    const BYTE *bindata = (const BYTE*)LockResource(LoadResource(0, hbin));
    DWORD binsize = SizeofResource(0, hbin);

    wxBitmap bitmap = wxBitmap::NewFromPNGData(bindata, binsize);
    button->SetBitmap(bitmap);
    button->SetText(text);
}

void Utility::SetIcon(wxBitmapButton* button, int iconid)
{
    HRSRC hbin = FindResource(NULL, MAKEINTRESOURCE(iconid), RT_RCDATA);
    const BYTE *bindata = (const BYTE*)LockResource(LoadResource(0, hbin));
    DWORD binsize = SizeofResource(0, hbin);

    wxBitmap bitmap = wxBitmap::NewFromPNGData(bindata, binsize);
    button->SetBitmap(bitmap);
}

void Utility::SetIcon(wxStaticBitmap* bmp, int iconid)
{
    HRSRC hbin = FindResource(NULL, MAKEINTRESOURCE(iconid), RT_RCDATA);
    const BYTE *bindata = (const BYTE*)LockResource(LoadResource(0, hbin));
    DWORD binsize = SizeofResource(0, hbin);

    wxBitmap bitmap = wxBitmap::NewFromPNGData(bindata, binsize);
    bmp->SetBitmap(bitmap);
}

wxString Utility::GetVersion()
{
    TCHAR exe_name[MAX_PATH + 1];
    ZeroMemory(exe_name, MAX_PATH+1);
    if(!GetModuleFileName(NULL, exe_name, MAX_PATH)) return wxEmptyString;

    DWORD handle;
    DWORD size = GetFileVersionInfoSize(exe_name, &handle);
    if(size == 0) return wxEmptyString;
    if(size >= 2048) return wxEmptyString;

    UINT8 buff[2048];
    ZeroMemory(buff, 2048);
    if(!GetFileVersionInfo(exe_name, (DWORD)NULL, size, buff)) return wxEmptyString;

    UINT length;
    VS_FIXEDFILEINFO* ver_info;
    VerQueryValue(buff, _T("\\"), reinterpret_cast<LPVOID*>(&ver_info), &length);

    WORD v1 = HIWORD(ver_info->dwProductVersionMS);
    WORD v2 = LOWORD(ver_info->dwProductVersionMS);
    WORD v3 = HIWORD(ver_info->dwProductVersionLS);
    WORD v4 = LOWORD(ver_info->dwProductVersionLS);

    return wxString::Format("%d.%d.%d", v1, v2, v3);
}

MultiLanguage* MultiLanguage::_instance = NULL;

MultiLanguage* MultiLanguage::Instance()
{
    if(_instance == NULL)
    {
        _instance = new MultiLanguage();
    }
    return _instance;
}

wxString ttt(wxString name)
{
    return MultiLanguage::Instance()->GetString("StringsPE", name);
}

MultiLanguage::MultiLanguage()
{
    locale="en-us"; 
    ini.SetUnicode();
    ini.SetQuotes();
    ini.SetMultiLine();
    damaged = true;

    ini_res.SetUnicode();
    ini_res.SetQuotes();
    ini_res.SetMultiLine();
    resource_loaded = false;
}

wxString MultiLanguage::GetString(wxString category, wxString name)
{
    if(!resource_loaded)
    {
        HMODULE handle = ::GetModuleHandle(NULL);
        HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(IDR_TEXT1), _T("TEXT"));
        HGLOBAL rcData = ::LoadResource(handle, rc);
        DWORD size = ::SizeofResource(handle, rc);

        char* buffer = new char[size+1];
        ::memcpy(buffer, rcData, size);
        buffer[size] = 0;

        ini_res.LoadData(buffer, size);
        
        delete[] buffer;
        resource_loaded = true;
    }
    if(damaged)
    {
        wxFileName file = wxFileName(wxStandardPaths::Get().GetExecutablePath());
        file.AppendDir("lang");
        wxFileName pe_lang_folder(file);
        pe_lang_folder.AppendDir("PE");
        if(pe_lang_folder.DirExists())
          file.AppendDir("PE");
        file.SetName(locale);
        file.SetExt("txt");
        ini.Reset();
        ini.LoadFile(file.GetFullPath().wc_str());

        damaged = false;
    }
    const wchar_t* val = ini.GetValue(category, name, wxEmptyString);
    wxString result(val);
    if(result == wxEmptyString)
    {
        val = ini_res.GetValue(category, name, wxEmptyString);
        result = wxString(val);
    }
    result.Replace("\\n", "\n", true);
    return result;
}

wxString Convert::Char2wxString(const char* src)
{
    return wxString(string2wstring(std::string(src)));
}

std::string Convert::wxString2String(const wxString& src)
{
    return wstring2string(std::wstring(src.wc_str()));
}

wxFileName WorkDir::GetWorkDir()
{
    wxFileName work_path(wxStandardPaths::Get().GetExecutablePath());
    work_path.SetFullName(wxEmptyString);
    work_path.AppendDir(_T("work"));
    return work_path;
}

bool WorkDir::RemoveWorkDir()
{
    wxFileName work_path = GetWorkDir();
    if(!work_path.DirExists())
    {
        return true;
    }
    if(!work_path.Rmdir(wxPATH_RMDIR_RECURSIVE))
    {
        return false;
    }
    return true;
}

bool WorkDir::PrepareWorkDir()
{
    wxFileName work_path = GetWorkDir();
    if(!work_path.Mkdir())
    {
        return false;
    }

    wxFileName media_path(work_path);
    media_path.AppendDir(_T("media"));
    if(!media_path.Mkdir())
    {
        return false;
    }

    wxFileName tmp_path(work_path);
    tmp_path.AppendDir(_T("tmp"));
    if(!tmp_path.Mkdir())
    {
        return false;
    }

    return true;
}

wxString Registry::GetStringValue(RegistryHiveData* hive_data, wxString key_name, wxString value_name)
{
    wxString result = wxEmptyString;
    TCHAR* buffer = NULL;

    HKEY sub_key;
    if(RegOpenKeyEx(hive_data->Key, key_name.wc_str(), 0, KEY_QUERY_VALUE, &sub_key) != ERROR_SUCCESS)
      return wxEmptyString;

    DWORD size;
    DWORD type;
    int str_len = 0;
    if(RegQueryValueEx(sub_key, value_name.wc_str(), NULL, &type, NULL, &size) != ERROR_SUCCESS)
      goto END;

    str_len = size/sizeof(TCHAR) + 1;
    buffer = new TCHAR[str_len];
    buffer[str_len - 1] = 0;
    if(RegQueryValueEx(sub_key, value_name.wc_str(), NULL, &type, (LPBYTE)buffer, &size) != ERROR_SUCCESS)
      goto END;

    result = wxString(buffer);
    
END:
    if(buffer != NULL) delete[] buffer;
    RegCloseKey(sub_key);
    return result;
}

bool Registry::SetStringValue(RegistryHiveData* hive_data, wxString key_name, wxString value_name, wxString value)
{
    bool result = false;
    HKEY sub_key;
    if(RegOpenKeyEx(hive_data->Key, key_name.wc_str(), 0, KEY_SET_VALUE , &sub_key) != ERROR_SUCCESS)
      return false;

    if(RegSetValueEx(sub_key, value_name.wc_str(), 0, REG_SZ, (const BYTE *) value.wc_str(), (value.length()+1) * sizeof(TCHAR)) != ERROR_SUCCESS)
      goto END;

    result = true;
END:
    RegCloseKey(sub_key);
    return result;
}

EnvironmentVariables* EnvironmentVariables::instance = nullptr;

EnvironmentVariables::EnvironmentVariables()
{
    wxEnvVariableHashMap _env_map;
    wxGetEnvMap(&_env_map);
    for(wxEnvVariableHashMap::iterator itr = _env_map.begin(), itr_end = _env_map.end();
        itr != itr_end; itr++)
    {
        wxString key = itr->first;
        key.MakeUpper();
        env_map[key] = itr->second;
    }
    
    wxFileName current_dir(wxStandardPaths::Get().GetExecutablePath());
    current_dir.SetFullName(wxEmptyString);
    wxString cd = current_dir.GetFullPath();
    if(cd[cd.Length()-1] == '\\')
      cd = cd.SubString(0, cd.Length()-2);
    env_map[_T("CURRENTDIR")] = cd;
    regex.Compile(_T("%.+?%"));
}

void EnvironmentVariables::replace_env_vars(wxString* src)
{
    while(regex.Matches(*src))
    {
        wxString matched_str = regex.GetMatch(*src);
        wxString key = matched_str.SubString(1, matched_str.Length()-2);// remove %
        key.MakeUpper();

        size_t start;
        size_t len;
        regex.GetMatch(&start, &len);

        if(env_map.count(key) != 0)
          src->replace(start, len, env_map[key]);
        else
          src->replace(start, len, wxEmptyString);
    }
}

void EnvironmentVariables::ReplaceEnvVars(wxString* src)
{
    if(instance == nullptr)
      instance = new EnvironmentVariables();
    instance->replace_env_vars(src);
}

