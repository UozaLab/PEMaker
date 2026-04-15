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
#ifndef __WIM_H__
#define __WIM_H__

#include "Events.h"
#include "wimlib.h"
#include <wx/wx.h>
#include <vector>

enum class WIMJobStatus
{
    INITIAL = 0,
    WIMFILE_OPEN_ERROR,
    WIMFILE_GETINFO_ERROR,
    WIMFILE_EXPORT_ERROR,
    WIMFILE_EXTRACT_ERROR,
    WIMFILE_UPDATE_ERROR,
    WIMFILE_WRITE_ERROR,
    WRITEFILE_OPEN_ERROR,
    IMAGE_NUMBER_ERROR,
    TERMINATED,
    SUCCESS
};

enum class WIMExtractSubCommands
{
    NORMAL,
    RENAME,
    DROPFILENAME,
};

class WIMCommands
{
  private:
    std::vector<wimlib_update_command*> cmds;
    wimlib_update_command* cmds_array;
  public:
    WIMCommands();
    ~WIMCommands();
    void AppendAdd(wxString src, wxString dst);
    void AppendDelete(wxString target_path, bool recursive);
    int Count() { return cmds.size(); }
    wimlib_update_command* Array();
};

class WIM
{
  private:
    static CustomThread* thread;
    static enum wimlib_progress_status write_progress(enum wimlib_progress_msg msg,
                                                      union wimlib_progress_info *info, void *progctx);

  public:
    WIM(CustomThread* _thread);
    ~WIM();
    WIMJobStatus GetPEImageNumber(wxString wim_file_path, int* image_number);
    WIMJobStatus GetNameInformations(wxString wim_file_path, std::vector<wxString>& infos);
    WIMJobStatus Export(wxString wim_file_path, int _image_number, wxString write_file_path);
    WIMJobStatus Update(wxString wim_file_path, int _image_number, WIMCommands& cmds);
    WIMJobStatus Extract(wxString wim_file_path, int _image_number, wxString target_path, wxString write_file_path, WIMExtractSubCommands sub_cmd = WIMExtractSubCommands::NORMAL);

};

#endif
