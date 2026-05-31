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

#include "CheckISOImage.h"
#include "ISO.h"
#include "WIM.h"
#include "MiscWx.h"
#include "tstring.h"
#include "FileSystem/DiskInfo.h"
#include <wx/stdpaths.h>
#include <vector>
#include <map>

CheckISOImage::CheckISOImage(wxEvtHandler *event_handler, DataHolder* _data)
     : CustomThread(event_handler), data(_data)
{
}

bool find(const std::vector<wxString>& strings, wxString str)
{
    for(std::vector<wxString>::const_iterator itr = strings.begin(),
        itr_end = strings.end(); itr != itr_end; itr++)
    {
        const wxString string = *itr;
        if(string.Contains(str)) return true;
    }
    return false;
}

bool CheckISOImage::try_copy_winre(tstring src_path, tstring dest_path)
{
    bool result = false;
    HANDLE h_read = INVALID_HANDLE_VALUE;
    HANDLE h_write = INVALID_HANDLE_VALUE;
    LONGLONG sum_bytes_read = 0LL;
    unsigned int progress_pre = 0;

    h_read = CreateFile(src_path.c_str(),
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        0, 0);
    if(h_read == INVALID_HANDLE_VALUE)
      goto END;

    h_write = CreateFile(dest_path.c_str(),
                         GENERIC_WRITE,
                         0,
                         NULL,
                         CREATE_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);
    if(h_write == INVALID_HANDLE_VALUE)
      goto END;

    wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Found %s", src_path)));
    wxQueueEvent(event_handler, new MsgEvent("Start copying..."));
    wxQueueEvent(event_handler, new ProgressEvent(0));
    LARGE_INTEGER file_size;
    GetFileSizeEx(h_read, &file_size);

    char buff[4096];
    DWORD bytes_read;
    DWORD bytes_write;
    while(true)
    {
        if(TerminateRequired()) break;
        if(!ReadFile(h_read, buff, sizeof(buff), &bytes_read, NULL))
        {
            goto END;
        }
        if(!bytes_read)
        {
            wxQueueEvent(event_handler, new ProgressEvent(100));
            result = true;
            break;
        }
        
        if(!WriteFile(h_write, buff, bytes_read, &bytes_write, NULL))
        {
            goto END;
        }
        sum_bytes_read += bytes_read;
        double progress = ((double)sum_bytes_read / (double)file_size.QuadPart) * 100.0;
        if(progress_pre != (unsigned int) progress)
          wxQueueEvent(event_handler, new ProgressEvent((unsigned int) progress));
        progress_pre = (unsigned int) progress;
    }
    
END:
    if(h_read != INVALID_HANDLE_VALUE)
      CloseHandle(h_read);
    if(h_write != INVALID_HANDLE_VALUE)
      CloseHandle(h_write);

    return result;
}

bool CheckISOImage::copy_winre(tstring volume_name, tstring dest_path)
{
    if(try_copy_winre(volume_name + _T("\\Recovery\\WindowsRE\\Winre.wim"), dest_path))
      return true;
    if(TerminateRequired())
      return false;
    if(try_copy_winre(volume_name + _T("\\Windows\\System32\\Recovery\\Winre.wim"), dest_path))
      return true;

    return false;
}

bool CheckISOImage::search_for_bootwim()
{
    wxFileName bootwim_path = WorkDir::GetWorkDir();
    bootwim_path.AppendDir("tmp");
    bootwim_path.SetName("boot");
    bootwim_path.SetExt("wim");

    // map<DiskNumber, map<PartitionNumber, VolumeInfo>>
    shared_ptr<std::map<DWORD, std::map<DWORD, VolumeInfo>>> vis = DiskInfoFactory::GetVolumeInfos();
    for(std::map<DWORD, std::map<DWORD, VolumeInfo>>::iterator itr = vis->begin(), itr_end = vis->end();
        itr != itr_end; itr++)
    {
        DWORD disk_id = itr->first;
        std::map<DWORD, VolumeInfo> volumes = itr->second;
        for(std::map<DWORD, VolumeInfo>::iterator itr_inner = volumes.begin(), itr_inner_end = volumes.end();
            itr_inner != itr_inner_end; itr_inner++)
        {
            if(TerminateRequired()) return false;
            VolumeInfo vi = itr_inner->second;
            DWORD part_id = itr_inner->first;
            tstring fsname = vi.FilesystemInfo.TypeInfo.FileSystemName;
            if(vi.DriveType != 3/*FIXED*/) continue;
            if(fsname != _T("NTFS") && fsname != _T("FAT32")) continue;
            if(copy_winre(vi.VolumeGUIDPath, bootwim_path.GetFullPath().wc_str()))
              return true;
        }
        if(TerminateRequired()) return false;
    }
    
    return false;
}


void* CheckISOImage::Entry()
{
    SetStateStart();
    ISOExtractor* iso = nullptr;
    std::vector<wxString> wim_image_names;
    std::map<tstring, bool> file_exist_map;
    WIM wim(this);

    wxFileName ini_path = wxStandardPaths::Get().GetExecutablePath();
    ini_path.SetFullName("pe_settings.ini");
    CSimpleIniW ini;
    ini.SetUnicode();
    ini.SetQuotes();
    ini.SetMultiLine();
    ini.LoadFile(ini_path.GetFullPath().wc_str());

PREP_STEP:
    if(!WorkDir::RemoveWorkDir())
    {
        wxQueueEvent(event_handler, new ErrorEvent("Cannot remove work folder."));
        goto END;
    }
    if(!WorkDir::PrepareWorkDir())
    {
        wxQueueEvent(event_handler, new ErrorEvent("Cannot create work folders."));
        goto END;
    }

    {
    const TCHAR* winpeshl_ini_value = ini.GetValue(_T("winpeshl.ini"), _T("key"));
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
    wxFileName bootwim_path = WorkDir::GetWorkDir();
    bootwim_path.AppendDir("tmp");
    bootwim_path.SetName("boot");
    bootwim_path.SetExt("wim");

    wxFileName export_target_path = WorkDir::GetWorkDir();
    export_target_path.AppendDir("media");
    export_target_path.AppendDir("sources");
    export_target_path.SetName("boot");
    export_target_path.SetExt("wim");

    {
    if(data->ExtractMethod == EXTRACT_BOOT_WIM_FROM_RE)
    {
        if(!search_for_bootwim())
        {
            wxQueueEvent(event_handler, new ErrorEvent("Cannot find the recovery environment"));
            goto END;
        }
        goto THIRD_STEP;
    }

    iso = ISO::CreateExtractor(this, data->FilePath.IsoFilePath);
    if(iso == nullptr)
    {
        wxQueueEvent(event_handler, new ErrorEvent("Failed to open iso file"));
        goto END;
    }

    wxQueueEvent(event_handler, new MsgEvent("Start checking files"));

    }

SECOND_STEP:
    {
    wxString boot_wim_internal_path("/sources/boot.wim");
    const CSimpleIni::TKeyVal* key_value_pair = ini.GetSection(_T("ExtractBootWIM"));
    if(key_value_pair)
    {
        for(CSimpleIni::TKeyVal::const_iterator itr = key_value_pair->begin(),
            itr_end = key_value_pair->end(); itr != itr_end; itr++)
        {
            wxString line(itr->second);
            line.Trim();
            line.Trim(false);
            boot_wim_internal_path = line;

            if(!iso->FileExists(boot_wim_internal_path))
            {
                wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Cannot find %s", boot_wim_internal_path)));
                continue;
            }
            
            break;
        }
    }

    wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Trying to extract %s", boot_wim_internal_path)));
    iso->EnableReportProgressFile(true);
    ISOJobStatus result = iso->ExtractFile(boot_wim_internal_path, bootwim_path.GetFullPath());
    if(result != ISOJobStatus::SUCCESS)
    {
        wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Warning : Failed to extract boot.wim (%d)", result)));
        goto SIXTH_STEP;
    }
    wxQueueEvent(event_handler, new ProgressEvent(100.));
    }

THIRD_STEP:
    {
    std::vector<wxString> infos;
    WIMJobStatus result = wim.GetNameInformations(bootwim_path.GetFullPath(), infos);
    if(result != WIMJobStatus::SUCCESS)
    {
        wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Failed to get boot.wim information(%d)", result)));
        goto FOURTH_STEP;
    }

    wxQueueEvent(event_handler, new MsgEvent(wxString::Format("boot.wim has %zu image(s)", infos.size())));
    int index = 1;
    for(std::vector<wxString>::iterator itr = infos.begin(), itr_end = infos.end(); itr != itr_end; itr++)
    {
        wim_image_names.push_back((*itr).Upper());
        wxQueueEvent(event_handler, new MsgEvent(wxString::Format("  [%d] %s", index, *itr)));
        index++;
    }

    {
    int image_number = 1;
    WIMJobStatus result = wim.GetPEImageNumber(bootwim_path.GetFullPath(), &image_number);
    if(result != WIMJobStatus::SUCCESS)
    {
        wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Failed to get PE image number(%d)", result)));
        goto FOURTH_STEP;
    }
    wxQueueEvent(event_handler, new MsgEvent("Trying to export an image"));
    result = wim.Export(bootwim_path.GetFullPath(), image_number, export_target_path.GetFullPath());
    if(result != WIMJobStatus::SUCCESS)
    {
        wxQueueEvent(event_handler, new MsgEvent(wxString::Format("Failed to export an image(%d)", result)));
        goto FOURTH_STEP;
    }
    }

    }

FOURTH_STEP:
    {
    const CSimpleIni::TKeyVal* key_value_pair = ini.GetSection(_T("ExtractFileFromBootWIM"));
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
    }

FIFTH_STEP:
    {
    wxFileName tmp_path = WorkDir::GetWorkDir();
    tmp_path.AppendDir("tmp");

    {
        std::vector<tstring> filenames =
        {
            _T("etfsboot.com"), _T("efisys.bin")
        };
        for(std::vector<tstring>::iterator itr = filenames.begin(), itr_end = filenames.end(); itr != itr_end; itr++)
        {
            tstring filename = *itr;
            tmp_path.SetFullName(filename);
            bool file_exists = tmp_path.FileExists();
            file_exist_map[filename] = file_exists;
        }
    }
    
    }

SIXTH_STEP:
    {
    wxQueueEvent(event_handler, new MsgEvent(wxEmptyString));
    wxQueueEvent(event_handler, new MsgEvent(ttt("ISODesc1")));
    wxQueueEvent(event_handler, new MsgEvent(ttt("ISODesc2")));

    wxString str_arc("  ");
    str_arc << ttt("ISODesc_Arch");
    str_arc << " : ";
    wxQueueEvent(event_handler, new MsgEvent(str_arc, true, false));
    str_arc = wxEmptyString;
    if(find(wim_image_names, "X86"))
      str_arc << "[32bit] ";
    if(find(wim_image_names, "64"))
      str_arc << "[64bit] ";
    wxQueueEvent(event_handler, new MsgEvent(str_arc, wxColor(0, 0, 255), false, true));

    wxString str_sys("  ");
    str_sys << ttt("ISODesc_System");
    str_sys << " : ";
    wxQueueEvent(event_handler, new MsgEvent(str_sys, true, false));
    str_sys = wxEmptyString;
    if(file_exist_map[_T("etfsboot.com")])
       str_sys << "[MBR(BIOS)] ";
    if(file_exist_map[_T("efisys.bin")])
       str_sys << "[GPT(UEFI)] ";
    wxQueueEvent(event_handler, new MsgEvent(str_sys, wxColor(0, 0, 255), false, true));

    wxQueueEvent(event_handler, new MsgEvent(wxEmptyString));
    wxQueueEvent(event_handler, new MsgEvent(ttt("ClickNextToProceed")));
    }

END:
    if(iso != nullptr)
      delete iso;

    SetStateComplete();
    return nullptr;
}

