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

#include "WIM.h"
#include "tstring.h"
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/dir.h>

CustomThread* WIM::thread = nullptr;


WIMCommands::WIMCommands()
     : cmds_array(nullptr)
{
}
WIMCommands::~WIMCommands()
{
    for(std::vector<wimlib_update_command*>::iterator itr = cmds.begin(), itr_end = cmds.end(); itr != itr_end; itr++)
    {
        wimlib_update_command* cmd = *itr;
        if(cmd->op == WIMLIB_UPDATE_OP_ADD)
        {
            free(cmd->add.wim_target_path);
            free(cmd->add.fs_source_path);
        }
        delete cmd;
    }
    if(cmds_array != nullptr) delete [] cmds_array;
}

void WIMCommands::AppendAdd(wxString src, wxString dst)
{
    wimlib_update_command* cmd = new wimlib_update_command();
    cmd->op = WIMLIB_UPDATE_OP_ADD;
    cmd->add.fs_source_path = _wcsdup(src.wc_str());
    cmd->add.wim_target_path = _wcsdup(dst.wc_str());
    cmd->add.config_file = nullptr;
    cmd->add.add_flags = 0;
    cmds.push_back(cmd);
}

void WIMCommands::AppendDelete(wxString target_path, bool recursive)
{
    wimlib_update_command* cmd = new wimlib_update_command();
    cmd->op = WIMLIB_UPDATE_OP_DELETE;
    cmd->delete_.wim_path = _wcsdup(target_path.wc_str());
    if(recursive)
      cmd->delete_.delete_flags = WIMLIB_DELETE_FLAG_FORCE | WIMLIB_DELETE_FLAG_RECURSIVE;
    else
      cmd->delete_.delete_flags = WIMLIB_DELETE_FLAG_FORCE;
    cmds.push_back(cmd);
}

wimlib_update_command* WIMCommands::Array()
{
    if(cmds_array != nullptr) delete [] cmds_array;
    if(cmds.size() == 0)
    {
        cmds_array = nullptr;
        return nullptr;
    }

    cmds_array = new wimlib_update_command[cmds.size()];
    int index = 0;
    for(std::vector<wimlib_update_command*>::iterator itr = cmds.begin(), itr_end = cmds.end(); itr != itr_end; itr++)
    {
        wimlib_update_command* cmd = *itr;
        cmds_array[index] = *cmd;
        index++;
    }
    return cmds_array;
}


WIM::WIM(CustomThread* _thread)
{
    thread = _thread;
}

WIM::~WIM()
{
}

WIMJobStatus WIM::GetPEImageNumber(wxString wim_file_path, int* image_number)
{
    *image_number = 0;
    wxFileName wim_filename(wim_file_path);
    WIMJobStatus ret = WIMJobStatus::SUCCESS;
    WIMStruct *wim_src = nullptr;
    struct wimlib_wim_info info;

    int result = wimlib_open_wim(wim_file_path.wc_str(), 0, &wim_src);
    if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_OPEN_ERROR;
        goto END;
    }

    result = wimlib_get_wim_info(wim_src, &info);
    if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_GETINFO_ERROR;
        goto END;
    }

    if(info.image_count == 1)
    {
        *image_number = 1;
        goto END;
    }

    for(int i=1; i<=info.image_count; i++)
    {
        wxString name(wimlib_get_image_name(wim_src, i));
        if(name.Lower().Contains("windows pe"))
        {
            wxQueueEvent(thread->EventHandler(), new MsgEvent(wxString::Format("Found PE image: %s", name)));
            *image_number = i;
            break;
        }
    }
    
END:
    if(wim_src != nullptr)
      wimlib_free(wim_src);

    if(ret == WIMJobStatus::SUCCESS && (*image_number) == 0)
    {
        ret = WIMJobStatus::IMAGE_NUMBER_ERROR;
    }
    else
    {
        wxQueueEvent(thread->EventHandler(), new MsgEvent(wxString::Format("Use the image with index %d", *image_number)));
    }
    return ret;
}

WIMJobStatus WIM::GetNameInformations(wxString wim_file_path, std::vector<wxString>& infos)
{
    wxFileName wim_filename(wim_file_path);
    WIMJobStatus ret = WIMJobStatus::SUCCESS;
    WIMStruct *wim_src = nullptr;
    struct wimlib_wim_info info;

	int result = wimlib_open_wim(wim_file_path.wc_str(), 0, &wim_src);
	if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_OPEN_ERROR;
        goto END;
    }

    result = wimlib_get_wim_info(wim_src, &info);
	if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_GETINFO_ERROR;
        goto END;
    }

    for(int i=1; i<=info.image_count; i++)
    {
        wxString name(wimlib_get_image_name(wim_src, i));
        infos.push_back(name);
    }
    
END:
    if(wim_src != nullptr)
      wimlib_free(wim_src);

    return ret;
}

#define TO_PERCENT(numerator, denominator) \
	(((denominator) == 0) ? 0 : ((numerator) * 100 / (denominator)))

enum wimlib_progress_status
WIM::write_progress(enum wimlib_progress_msg msg,
		 union wimlib_progress_info *info, void *progctx)
{
    static unsigned int percent_done_pre = 0;
    if(msg != WIMLIB_PROGRESS_MSG_WRITE_STREAMS)
      return WIMLIB_PROGRESS_STATUS_CONTINUE;

    if(thread->TerminateRequired())
    {
        return WIMLIB_PROGRESS_STATUS_ABORT;
    }

    if(info->write_streams.completed_bytes == 0)
    {
        percent_done_pre = 0;
    }
    unsigned int percent_done = TO_PERCENT(info->write_streams.completed_bytes,
                                           info->write_streams.total_bytes);
    if(percent_done != percent_done_pre)
        wxQueueEvent(thread->EventHandler(), new ProgressEvent(percent_done));
    percent_done_pre = percent_done;

    return WIMLIB_PROGRESS_STATUS_CONTINUE;
}

WIMJobStatus WIM::Export(wxString wim_file_path, int _image_number, wxString write_file_path)
{
    wxFileName fn_writer(write_file_path);
    if(!fn_writer.DirExists())
    {
        fn_writer.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        if(!fn_writer.DirExists())
          return WIMJobStatus::WRITEFILE_OPEN_ERROR;
    }

    WIMStruct *wim_src = nullptr;
    WIMStruct *wim_dst = nullptr;
    WIMJobStatus ret = WIMJobStatus::SUCCESS;
    wxString image_number_wxstring;
    image_number_wxstring << _image_number;
    int image_number = WIMLIB_NO_IMAGE;
    
	int result = wimlib_open_wim(wim_file_path.wc_str(), 0, &wim_src);
	if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_OPEN_ERROR;
        goto END;
    }

    result = wimlib_create_new_wim(WIMLIB_COMPRESSION_TYPE_LZX, &wim_dst);
	if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_EXPORT_ERROR;
        goto END;
    }

    image_number = wimlib_resolve_image(wim_src, image_number_wxstring.wc_str());
    if (image_number == WIMLIB_NO_IMAGE)
    {
        ret = WIMJobStatus::WIMFILE_OPEN_ERROR;
        goto END;
    }

	result = wimlib_export_image(wim_src, image_number, wim_dst, nullptr, nullptr,
                                 WIMLIB_EXPORT_FLAG_GIFT | WIMLIB_EXPORT_FLAG_BOOT);
	if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_EXPORT_ERROR;
        goto END;
    }

    wimlib_register_progress_function(wim_dst, write_progress, NULL);
    result = wimlib_write(wim_dst, write_file_path.wc_str(), WIMLIB_ALL_IMAGES, 0, 0);
	if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_WRITE_ERROR;
        goto END;
    }

    
END:
    if(wim_src != nullptr)
      wimlib_free(wim_src);
    if(wim_dst != nullptr)
      wimlib_free(wim_dst);
    
    return ret;
}

WIMJobStatus WIM::Update(wxString wim_file_path, int _image_number, WIMCommands& cmds)
{
    WIMJobStatus ret = WIMJobStatus::SUCCESS;
    WIMStruct *wim_src = nullptr;

	int result = wimlib_open_wim(wim_file_path.wc_str(), 0, &wim_src);
	if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_OPEN_ERROR;
        goto END;
    }

    result = wimlib_update_image(wim_src, _image_number, cmds.Array(), cmds.Count(), 0);
    if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_UPDATE_ERROR;
        goto END;
    }

    wxQueueEvent(thread->EventHandler(), new MsgEvent(_T("Trying to commit updates")));
    wimlib_register_progress_function(wim_src, write_progress, NULL);
    result = wimlib_overwrite(wim_src, 0, 0);
    if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_WRITE_ERROR;
        goto END;
    }

END:
    if(wim_src != nullptr)
      wimlib_free(wim_src);
    return ret;
}

WIMJobStatus WIM::Extract(wxString wim_file_path, int _image_number, wxString target_path, wxString write_file_path, WIMExtractSubCommands sub_cmd)
{
    bool rename = (sub_cmd == WIMExtractSubCommands::RENAME);
    bool drop_filename = (sub_cmd == WIMExtractSubCommands::DROPFILENAME);

    wxFileName fn_writer(write_file_path, wxEmptyString);
    wxString rename_filename = wxEmptyString;
    if(rename || drop_filename)
    {
        fn_writer = wxFileName(write_file_path);
        rename_filename = fn_writer.GetFullName();
        fn_writer = wxFileName(fn_writer.GetPath(), wxEmptyString);
    }
    if(!fn_writer.DirExists())
    {
        fn_writer.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        if(!fn_writer.DirExists())
          return WIMJobStatus::WRITEFILE_OPEN_ERROR;
    }

    WIMStruct *wim_src = nullptr;
    WIMJobStatus ret = WIMJobStatus::SUCCESS;
    wxString image_number_wxstring;
    image_number_wxstring << _image_number;
    int image_number = WIMLIB_NO_IMAGE;
    const wimlib_tchar* path = target_path.wc_str();
    const wimlib_tchar** paths = &path;
    
	int result = wimlib_open_wim(wim_file_path.wc_str(), 0, &wim_src);
	if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_OPEN_ERROR;
        goto END;
    }

    image_number = wimlib_resolve_image(wim_src, image_number_wxstring.wc_str());
    if (image_number == WIMLIB_NO_IMAGE)
    {
        ret = WIMJobStatus::WIMFILE_OPEN_ERROR;
        goto END;
    }

    result = wimlib_extract_paths(wim_src, image_number, fn_writer.GetFullPath().wc_str(), paths, 1,
                                  WIMLIB_EXTRACT_FLAG_NO_ATTRIBUTES | WIMLIB_EXTRACT_FLAG_GLOB_PATHS |
                                  WIMLIB_EXTRACT_FLAG_NO_PRESERVE_DIR_STRUCTURE | WIMLIB_EXTRACT_FLAG_NO_ACLS);
	if (result != 0)
    {
        ret = WIMJobStatus::WIMFILE_EXTRACT_ERROR;
        goto END;
    }
    if(rename)
    {
        wxFileName rename_from(fn_writer);
        rename_from.SetFullName(target_path.AfterLast(_T('/')));

        fn_writer.SetFullName(rename_filename);

        wxString hoge = rename_from.GetFullPath();
        wxString uho = fn_writer.GetFullPath();
        if(rename_from.FileExists())
        {
            if(!wxRenameFile(rename_from.GetFullPath(), fn_writer.GetFullPath()))
            {
                wxQueueEvent(thread->EventHandler(), new MsgEvent(wxString::Format("Cannot rename file : %s", rename_from.GetFullPath())));
            }
        }
    }

END:
    if(wim_src != nullptr)
      wimlib_free(wim_src);
    
    return ret;
}
