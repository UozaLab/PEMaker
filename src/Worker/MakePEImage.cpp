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

#include "MakePEImage.h"
#include "ISO.h"
#include "WIM.h"
#include "tstring.h"
#include "SimpleIni.h"
#include "MakeISO.h"
#include "MiscWx.h"
#include "RegistryHive.h"
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/regex.h>
#include <shlobj.h>

MakePEImage::MakePEImage(wxEvtHandler *event_handler, DataHolder* _data)
     : CustomThread(event_handler), data(_data)
{
}

wxString MakePEImage::execute_cmd(const wxString& cmd)
{
    HANDLE handle_rd, handle_wr;
    SECURITY_ATTRIBUTES security;
    ZeroMemory(&security, sizeof(SECURITY_ATTRIBUTES));
    security.nLength = sizeof(SECURITY_ATTRIBUTES);
    security.lpSecurityDescriptor = NULL;
    security.bInheritHandle = TRUE;
    if ( !CreatePipe(&handle_rd, &handle_wr, &security, 0) )
      return wxEmptyString;

    if ( !SetHandleInformation(handle_rd, HANDLE_FLAG_INHERIT, 0) )
      return wxEmptyString;

    
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(si);
    si.hStdOutput = handle_wr;
    si.hStdError = handle_wr;
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    if(!CreateProcess(nullptr, (LPWSTR) cmd.t_str(), nullptr, nullptr, true, 0, nullptr, nullptr, &si, &pi))
    {
        CloseHandle(handle_rd);
        CloseHandle(handle_wr);
        return wxEmptyString;
    }

    CloseHandle(handle_wr);

    wxQueueEvent(event_handler, new ProgressEvent(0));
    unsigned int progress_pre = 0;
    wxRegEx r(_T("[ \\[=]+[0-9]+\.[0-9]+%[ \\]=]+[\\r]*"));
    wxRegEx r_number(_T("[0-9]+\.[0-9]+"));
    char buff[256];
    DWORD bytes_read;
    wxString buffer = wxEmptyString;
    wxString result = wxEmptyString;
    while(true)
    {
        if(TerminateRequired())
        {
            TerminateProcess(pi.hProcess, 0);
            break;
        }

        bool ret = ReadFile(handle_rd, buff, sizeof(buff)-1, &bytes_read, NULL);
        if(!ret) break;
        if(bytes_read == 0) break;

        buff[bytes_read] = '\0';
        buffer += wxString(buff);

        while(true)
        {
            int index = buffer.Find('\n');
            if(index == -1) break;

            wxString line = buffer.Left(index);

            if(r.Matches(line))
            {
                double max_value = 0;
                for(int i=0;i<r.GetMatchCount();i++)
                {
                    wxString matched = r.GetMatch(line, i);
                    if(!r_number.Matches(matched)) continue;
                    wxString matched_number = r_number.GetMatch(matched);
                    double value;
                    matched_number.ToDouble(&value);
                    if(max_value < value) max_value = value;

                    double progress = max_value;
                    if(progress_pre != (unsigned int) progress)
                    wxQueueEvent(event_handler, new ProgressEvent((unsigned int) progress));
                    progress_pre = (unsigned int) progress;
                }
                r.Replace(&line, wxEmptyString);
            }
            
            if(line == _T("\r")) line = wxEmptyString;
            if(line.Length() != 0)
            {
                result += line;
                result += _T("\n");
                wxQueueEvent(event_handler, new MsgEvent(wxString::Format("%s\n", line), false, false));
            }
            buffer = buffer.Mid(index+1);
        }
        result += buffer;
        wxQueueEvent(event_handler, new MsgEvent(buffer, false, false));
    }

    CloseHandle(handle_rd);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return result;
}

void* MakePEImage::Entry()
{
    SetStateStart();
    ISOExtractor* iso = nullptr;
    WIM wim(this);

    wxFileName ini_path = wxStandardPaths::Get().GetExecutablePath();
    ini_path.SetFullName("pe_settings.ini");
    CSimpleIniW ini;
    ini.SetUnicode();
    ini.SetQuotes();
    ini.SetMultiLine();
    ini.LoadFile(ini_path.GetFullPath().wc_str());

    wxFileName export_target_path = WorkDir::GetWorkDir();
    export_target_path.AppendDir("media");
    export_target_path.AppendDir("sources");
    export_target_path.SetName("boot");
    export_target_path.SetExt("wim");

PREP_STEP:
    wxFileName bootwim_path = WorkDir::GetWorkDir();
    bootwim_path.AppendDir("tmp");
    bootwim_path.SetName("boot");
    bootwim_path.SetExt("wim");
    if(!bootwim_path.FileExists())
    {
        wxQueueEvent(event_handler, new ErrorEvent("Cannot find a boot.wim file"));
        goto END;
    }

    {
    const TCHAR* winpeshl_ini_value = ini.GetValue(data->ISOData.Arch32bit ? _T("winpeshl.ini_x86") : _T("winpeshl.ini_x64"), _T("key"));
    if(winpeshl_ini_value != nullptr)
    {
        wxFileName tmp_path = WorkDir::GetWorkDir();
        tmp_path.AppendDir("tmp");
        tmp_path.SetFullName("winpeshl.ini");
        wxFile ini_file(tmp_path.GetFullPath(), wxFile::write);
        if(ini_file.IsOpened())
        {
            ini_file.Write(winpeshl_ini_value);
        }
    }
    }

FIRST_STEP:
    {
    if(TerminateRequired()) goto END;
    if(data->ExtractMethod == EXTRACT_BOOT_WIM_FROM_ISO)
    {
        iso = ISO::CreateExtractor(this, data->FilePath.FilePath);
        if(iso == nullptr)
        {
            wxQueueEvent(event_handler, new ErrorEvent("Failed to open iso file"));
            goto END;
        }
    }

    wxFileName catalog_file_path = WorkDir::GetWorkDir();
    catalog_file_path.AppendDir("media");
    catalog_file_path.SetName("boot");
    catalog_file_path.SetExt("catalog");
    {
    wxFile catalog_file(catalog_file_path.GetFullPath(), wxFile::write);
    unsigned char tmp[2048];
    if(catalog_file.Write(tmp, 2048) != 2048)
    {
        wxQueueEvent(event_handler, new ErrorEvent("Failed to create boot catalog file"));
        goto END;
    }
    }

SECOND_STEP:
    {
    if(TerminateRequired()) goto END;

    const TCHAR* section = data->ISOData.Mbr ? _T("ExtractFileFromBootWIM_BIOS") : _T("ExtractFileFromBootWIM_UEFI");
    CSimpleIni::TNamesDepend keys;
    ini.GetAllKeys(section, keys);
    keys.sort(CSimpleIni::Entry::LoadOrder());

    for(CSimpleIni::TNamesDepend::const_iterator itr = keys.begin(), itr_end = keys.end();
        itr != itr_end; itr++)
    {
        wxString key(itr->pItem);
        wxString line(ini.GetValue(section, itr->pItem));
        int c_index = line.Find(',');
        if(c_index == -1) continue;
        wxString from(line.SubString(0, c_index-1));
        wxString to(line.SubString(c_index+1, line.Length()-1));
        from.Trim();
        from.Trim(false);
        to.Trim();
        to.Trim(false);
        EnvironmentVariables::ReplaceEnvVars(&from);
        EnvironmentVariables::ReplaceEnvVars(&to);

        WIMExtractSubCommands sub_cmd = WIMExtractSubCommands::NORMAL;
        if(key.Upper().StartsWith("FILENOTEXIST"))
        {
            sub_cmd = WIMExtractSubCommands::DROPFILENAME;
            if(wxFileExists(to))
              continue;
        }
            
        if(key.Upper().StartsWith("RENAME"))
          sub_cmd = WIMExtractSubCommands::RENAME;

        WIMJobStatus result = wim.Extract(export_target_path.GetFullPath(), 1, from, to, sub_cmd);
        if(result != WIMJobStatus::SUCCESS)
        {
            wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Failed to extract files(%d)", result)));
        }
    }
    }


THIRD_STEP:
    {
    if(TerminateRequired()) goto END;
    const wxString subkey_name("Microsoft\\Windows NT\\CurrentVersion\\WinPE");
    const wxString value_name("InstRoot");
    wxFileName tmp_path = WorkDir::GetWorkDir();
    tmp_path.AppendDir("tmp");
    tmp_path.SetFullName("SOFTWARE");
    if(!tmp_path.FileExists()) goto FOURTH_STEP;

    wxQueueEvent(event_handler, new MsgEvent("Trying to update registry on WinPE"));

    RegistryHiveData* hive_data = RegistryHive::Load(tmp_path.GetFullPath(), _T("pemaker_hive"));
    if(hive_data == NULL) goto FOURTH_STEP;

    wxString InstRoot = Registry::GetStringValue(hive_data, subkey_name, value_name);
    if(InstRoot == wxEmptyString) goto THIRD_END;
    if(InstRoot.Upper() == "X:\\") goto THIRD_END;
    wxQueueEvent(event_handler, new MsgEvent(wxString::Format("InstRoot : %s", InstRoot)));
    if(!Registry::SetStringValue(hive_data, subkey_name, value_name, "X:\\"))
      goto THIRD_END;
    delete hive_data;
    hive_data = nullptr;

    {
    WIMCommands cmds;
    cmds.AppendAdd(tmp_path.GetFullPath(), "/windows/system32/config/SOFTWARE");
    WIMJobStatus result = wim.Update(export_target_path.GetFullPath(), 1, cmds);
    if(result != WIMJobStatus::SUCCESS)
    {
        wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Failed to update registry value(InstRoot) (%d)", result)));
    }
    }

THIRD_END:
    if(hive_data != nullptr)
      delete hive_data;
    }

FOURTH_STEP:
    {
    if(TerminateRequired()) goto END;
    if(data->ExtractMethod != EXTRACT_BOOT_WIM_FROM_ISO)
      goto FIFTH_STEP;

    iso->EnableReportProgressFile(false);

    const TCHAR* section = _T("ExtractFolderFromISO");
    CSimpleIni::TNamesDepend keys;
    ini.GetAllKeys(section, keys);
    keys.sort(CSimpleIni::Entry::LoadOrder());

    for(CSimpleIni::TNamesDepend::const_iterator itr = keys.begin(), itr_end = keys.end();
        itr != itr_end; itr++)
    {
        wxString line(ini.GetValue(section, itr->pItem));
        int c_index = line.Find(',');
        if(c_index == -1) continue;
        wxString from(line.SubString(0, c_index-1));
        wxString to(line.SubString(c_index+1, line.Length()-1));
        from.Trim();
        from.Trim(false);
        to.Trim();
        to.Trim(false);
        EnvironmentVariables::ReplaceEnvVars(&from);
        EnvironmentVariables::ReplaceEnvVars(&to);
        ISOJobStatus result = iso->ExtractFolder(from, to);
        if(result != ISOJobStatus::SUCCESS)
        {
            wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Failed to extract %s(%d)", from, result)));
        }
    }

    const TCHAR* section2 = _T("ExtractFileFromISO");
    CSimpleIni::TNamesDepend keys2;
    ini.GetAllKeys(section2, keys2);
    keys2.sort(CSimpleIni::Entry::LoadOrder());

    for(CSimpleIni::TNamesDepend::const_iterator itr = keys2.begin(), itr_end = keys2.end();
        itr != itr_end; itr++)
    {
        wxString line(ini.GetValue(section2, itr->pItem));
        int c_index = line.Find(',');
        if(c_index == -1) continue;
        wxString from(line.SubString(0, c_index-1));
        wxString to(line.SubString(c_index+1, line.Length()-1));
        from.Trim();
        from.Trim(false);
        to.Trim();
        to.Trim(false);
        EnvironmentVariables::ReplaceEnvVars(&from);
        EnvironmentVariables::ReplaceEnvVars(&to);
        ISOJobStatus result = iso->ExtractFile(from, to);
        if(result != ISOJobStatus::SUCCESS)
        {
            wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Failed to extract %s(%d)", from, result)));
        }
    }
    }

FIFTH_STEP:
    {
    if(TerminateRequired()) goto END;
    wxQueueEvent(event_handler, new MsgEvent("Trying to delete and add files"));
    // Delete
    WIMCommands cmds;

    const TCHAR* section = _T("Delete");
    CSimpleIni::TNamesDepend keys;
    ini.GetAllKeys(section, keys);
    keys.sort(CSimpleIni::Entry::LoadOrder());

    for(CSimpleIni::TNamesDepend::const_iterator itr = keys.begin(), itr_end = keys.end();
        itr != itr_end; itr++)
    {
        wxString file_to_delete(ini.GetValue(section, itr->pItem));
        cmds.AppendDelete(file_to_delete, false);
    }

    // Add
    const TCHAR* section2 = data->ISOData.Arch32bit ? _T("Add_x86") : _T("Add_x64");
    CSimpleIni::TNamesDepend keys2;
    ini.GetAllKeys(section2, keys2);
    keys2.sort(CSimpleIni::Entry::LoadOrder());

    for(CSimpleIni::TNamesDepend::const_iterator itr = keys2.begin(), itr_end = keys2.end();
        itr != itr_end; itr++)
    {
        wxString line_to_add(ini.GetValue(section2, itr->pItem));
        int c_index = line_to_add.Find(',');
        if(c_index == -1) continue;
        wxString add_from(line_to_add.SubString(0, c_index-1));
        wxString add_to(line_to_add.SubString(c_index+1, line_to_add.Length()-1));
        add_from.Trim();
        add_from.Trim(false);
        add_to.Trim();
        add_to.Trim(false);
        EnvironmentVariables::ReplaceEnvVars(&add_from);
        EnvironmentVariables::ReplaceEnvVars(&add_to);
        if(!wxFileName::FileExists(add_from) && !wxFileName::DirExists(add_from))
        {
            wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Skip adding %s", add_from)));
            continue;
        }
        cmds.AppendAdd(add_from, add_to);
    }

    WIMJobStatus result = wim.Update(export_target_path.GetFullPath(), 1, cmds);
    if(result != WIMJobStatus::SUCCESS)
    {
        wxQueueEvent(event_handler, new ErrorEvent(wxString::Format("Failed to update image(%d)", result)));
        goto END;
    }
    }

SIXTH_STEP:
    {
    if(TerminateRequired()) goto END;
    wxQueueEvent(event_handler, new ProgressEvent(0.));
    wxQueueEvent(event_handler, new MsgEvent("Trying to copy files"));
    // CopyFiles
    const TCHAR* section = _T("CopyFile");
    CSimpleIni::TNamesDepend keys;
    ini.GetAllKeys(section, keys);
    keys.sort(CSimpleIni::Entry::LoadOrder());

    for(CSimpleIni::TNamesDepend::const_iterator itr = keys.begin(), itr_end = keys.end();
        itr != itr_end; itr++)
    {
        wxString line_to_copy(ini.GetValue(section, itr->pItem));
        int c_index = line_to_copy.Find(',');
        if(c_index == -1) continue;
        wxString copy_from(line_to_copy.SubString(0, c_index-1));
        wxString copy_to(line_to_copy.SubString(c_index+1, line_to_copy.Length()-1));
        copy_from.Trim();
        copy_from.Trim(false);
        copy_to.Trim();
        copy_to.Trim(false);
        EnvironmentVariables::ReplaceEnvVars(&copy_from);
        EnvironmentVariables::ReplaceEnvVars(&copy_to);
        if(!wxFileName::FileExists(copy_from))
        {
            wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Skip copying %s", copy_from)));
            continue;
        }
        if(!wxCopyFile(copy_from, copy_to))
        {
            wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Failed to copy %s", copy_from)));
            continue;
        }
    }
    }

SEVENTH_STEP:
    {
    if(TerminateRequired()) goto END;
    bool dism_enabled = false;
    wxString value(ini.GetValue(_T("Options"), _T("ExecuteDISM")));
    if(value.Upper() == _T("1") || value.Upper() == _T("TRUE"))
    {
        wxQueueEvent(event_handler, new MsgEvent("DISM commands execution enabled"));
        dism_enabled = true;
    }

    if(dism_enabled)
    {
        const TCHAR* section = _T("DISMCmds");
        CSimpleIni::TNamesDepend keys;
        ini.GetAllKeys(section, keys);
        keys.sort(CSimpleIni::Entry::LoadOrder());
        for(CSimpleIni::TNamesDepend::const_iterator itr = keys.begin(), itr_end = keys.end();
            itr != itr_end; itr++)
        {
            if(TerminateRequired()) break;
            wxString line_to_execute(ini.GetValue(section, itr->pItem));
            line_to_execute.Trim();
            line_to_execute.Trim(false);
            EnvironmentVariables::ReplaceEnvVars(&line_to_execute);
            wxString cmd = wxString::Format("dism.exe %s", line_to_execute);
            wxQueueEvent(event_handler, new MsgEvent(cmd));
            wxString result = execute_cmd(cmd);
        }
    }

    }
EIGHTH_STEP:
    {
    if(TerminateRequired()) goto END;

    wxQueueEvent(event_handler, new MsgEvent("Trying to make an iso image"));
    wxFileName iso_in = WorkDir::GetWorkDir();
    wxFileName iso_out = WorkDir::GetWorkDir();
    wxFileName boot_file = WorkDir::GetWorkDir();
    wxFileName catalog_file = WorkDir::GetWorkDir();
    boot_file.AppendDir("media");
    boot_file.AppendDir("boot");
    boot_file.SetFullName(data->ISOData.Mbr ? "etfsboot.com" : "efisys.bin");
    if(!boot_file.FileExists())
    {
        wxQueueEvent(event_handler, new ErrorEvent(wxString::Format("Cannot find boot image : %s", boot_file.GetFullPath())));;
        goto END;
    }
    catalog_file.AppendDir("media");
    catalog_file.SetFullName("boot.catalog");
    iso_in.AppendDir("media");
    iso_out.SetFullName("WinPE.iso");
    MakeISO m_iso(this, iso_out.GetFullPath(), iso_in.GetFullPath(), !data->ISOData.Mbr);
    m_iso.SetCatalogFilepath(catalog_file.GetFullPath(), "/boot.catalog");
    m_iso.SetBootable(boot_file.GetFullPath(), (data->ISOData.Mbr) ? "/boot/etfsboot.com" : "/boot/efisys.bin");
    if(!m_iso.Make())
    {
        wxQueueEvent(event_handler, new ErrorEvent("Failed to make an iso image"));
        goto END;
    }
    wxQueueEvent(event_handler, new ProgressEvent(100.));
    wxQueueEvent(event_handler, new MsgEvent(ttt("FinishISOFileCreation"), wxColour(0, 0, 255)));
    }

FINAL_STEP:
    wxFileName tmp_path = WorkDir::GetWorkDir();
    tmp_path.AppendDir("tmp");
    if(!tmp_path.DirExists())
      goto END;
    tmp_path.Rmdir(wxPATH_RMDIR_RECURSIVE);

    }
END:
    if(iso != nullptr)
      delete iso;

    SetStateComplete();
    return nullptr;
}

