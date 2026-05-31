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
#include <shlobj.h>

MakePEImage::MakePEImage(wxEvtHandler *event_handler, DataHolder* _data)
     : CustomThread(event_handler), data(_data)
{
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

FIRST_STEP:
    {
    if(data->ExtractMethod == EXTRACT_BOOT_WIM_FROM_ISO)
    {
        iso = ISO::CreateExtractor(this, data->FilePath.IsoFilePath);
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
    const CSimpleIni::TKeyVal* key_value_pair = ini.GetSection(data->ISOData.Mbr ? _T("ExtractFileFromBootWIM_BIOS") : _T("ExtractFileFromBootWIM_UEFI"));
    if(key_value_pair)
    {
        for(CSimpleIni::TKeyVal::const_iterator itr = key_value_pair->begin(),
            itr_end = key_value_pair->end(); itr != itr_end; itr++)
        {
            wxString key(itr->first.pItem);
            wxString line(itr->second);
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
    }


THIRD_STEP:
    {
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
    if(data->ExtractMethod == EXTRACT_BOOT_WIM_FROM_RE)
      goto FIFTH_STEP;

    iso->EnableReportProgressFile(false);
    const CSimpleIni::TKeyVal* key_value_pair = ini.GetSection(_T("ExtractFolderFromISO"));
    if(key_value_pair)
    {
        for(CSimpleIni::TKeyVal::const_iterator itr = key_value_pair->begin(),
            itr_end = key_value_pair->end(); itr != itr_end; itr++)
        {
            wxString line(itr->second);
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
    }

    key_value_pair = ini.GetSection(_T("ExtractFileFromISO"));
    if(key_value_pair)
    {
        for(CSimpleIni::TKeyVal::const_iterator itr = key_value_pair->begin(),
            itr_end = key_value_pair->end(); itr != itr_end; itr++)
        {
            wxString line(itr->second);
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
    }

FIFTH_STEP:
    {
    wxQueueEvent(event_handler, new MsgEvent("Trying to delete and add files"));
    // Delete
    WIMCommands cmds;
    const CSimpleIni::TKeyVal* key_value_pair = ini.GetSection(_T("Delete"));
    if(key_value_pair)
    {
        for(CSimpleIni::TKeyVal::const_iterator itr = key_value_pair->begin(),
            itr_end = key_value_pair->end(); itr != itr_end; itr++)
        {
            wxString file_to_delete(itr->second);
            cmds.AppendDelete(file_to_delete, false);
        }
    }

    // Add
    key_value_pair = ini.GetSection(data->ISOData.Mbr ? _T("Add_x86") : _T("Add_x64"));
    if(key_value_pair)
    {
        for(CSimpleIni::TKeyVal::const_iterator itr = key_value_pair->begin(),
            itr_end = key_value_pair->end(); itr != itr_end; itr++)
        {
            wxString line_to_add(itr->second);
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
            if(!wxFileName::FileExists(add_from))
            {
                wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Skip adding %s", add_from)));
                continue;
            }
            cmds.AppendAdd(add_from, add_to);
        }
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
    wxQueueEvent(event_handler, new ProgressEvent(0.));
    wxQueueEvent(event_handler, new MsgEvent("Trying to copy files"));
    // CopyFiles
    const CSimpleIni::TKeyVal* key_value_pair = ini.GetSection(_T("CopyFile"));
    if(key_value_pair)
    {
        for(CSimpleIni::TKeyVal::const_iterator itr = key_value_pair->begin(),
            itr_end = key_value_pair->end(); itr != itr_end; itr++)
        {
            wxString line_to_copy(itr->second);
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
    }

SEVENTH_STEP:
    {
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

