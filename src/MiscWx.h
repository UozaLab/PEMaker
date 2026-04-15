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

#ifndef __MISC_WX_H___
#define __MISC_WX_H___

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/utils.h>
#include <wx/regex.h>
#include "Widgets/SimpleButton.h"
#include "SimpleIni.h"
#include "RegistryHive.h"

class Utility
{
public:
    void static SetIcon(SimpleButton* button, int iconid, wxString text);
    void static SetIcon(wxBitmapButton* button, int iconid);
    void static SetIcon(wxStaticBitmap* bmp, int iconid);
    wxString static GetVersion();
};

class MultiLanguage
{
private:
    static MultiLanguage* _instance;
    wxString locale_opened;
protected:
    CSimpleIniW ini;
    CSimpleIniW ini_res;
    wxString locale;
    bool damaged;
    bool resource_loaded;

    MultiLanguage();
public:
    static MultiLanguage* Instance();
    void SetLocale(wxString lng)
    {
        locale = lng;
        damaged = true;
    }
    wxString GetString(wxString category, wxString name);
};

wxString ttt(wxString name);

class Convert
{
public:
    wxString static Char2wxString(const char* src);
    std::string static wxString2String(const wxString& src);
};

class WorkDir
{
public:
    static wxFileName GetWorkDir();
    static bool RemoveWorkDir();
    static bool PrepareWorkDir();
};

class Registry
{
public:
    static wxString GetStringValue(RegistryHiveData* hive_data, wxString key_name, wxString value_name);
    static bool SetStringValue(RegistryHiveData* hive_data, wxString key_name, wxString value_name, wxString value);
};

class EnvironmentVariables
{
  private:
    wxEnvVariableHashMap env_map;
    wxRegEx regex;
    EnvironmentVariables();
    static EnvironmentVariables* instance;
    void replace_env_vars(wxString* src);
  public:
    static void ReplaceEnvVars(wxString* src);
};

#endif
