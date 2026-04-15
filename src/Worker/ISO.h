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
#ifndef __ISO_H__
#define __ISO_H__

#include <wx/wx.h>
#include <cdio/cdio.h>
#include <cdio/iso9660.h>
#include <cdio/udf.h>
#include <map>
#include "Events.h"

enum class ISOJobStatus
{
    INITIAL = 0,
    ISOFILE_OPEN_ERROR,
    TARGETFILE_OPEN_ERROR,
    TARGETFILE_NOTFILE_ERROR,
    TARGETFILE_READ_ERROR,
    WRITEFILE_OPEN_ERROR,
    WRITEFILE_WRITE_ERROR,
    WRITEFILE_MKDIR_ERROR,
    TERMINATED,
    SUCCESS
};

class ISOExtractor
{
  protected:
    CustomThread* thread;
    bool report_progress_file;
    std::map<wxString, std::pair<wxString, iso9660_stat_t*> > file_list;
    wxString get_correct_filename(const wxString& src);
    std::pair<wxString, iso9660_stat_t*> get_correct_data(const wxString& src);
  public:
    ISOExtractor(CustomThread* _thread)
         : thread(_thread), report_progress_file(false) {}
    virtual ~ISOExtractor();
    void EnableReportProgressFile(bool tf){ report_progress_file = tf; }
    
    virtual ISOJobStatus ExtractFile(const wxString& target_file_path, const wxString& write_file_path) = 0;
    virtual ISOJobStatus ExtractFolder(const wxString& target_dir_path, const wxString& write_folder_path) = 0;
    virtual bool Valid() = 0;
    virtual bool FileExists(const wxString& target_file_path);
};

class ISOExtractorUdf : public ISOExtractor
{
  private:
    udf_t* udf;
    ISOJobStatus extract_folder(udf_dirent_t* target_dirent, const wxString& target_dir_path, const wxString& write_folder_path);
    void create_file_list(udf_dirent_t* udf_dirent, const wxString& dir);
  public:
    ISOExtractorUdf(CustomThread* thread, const wxString& iso_file_path);
    ~ISOExtractorUdf();
    ISOJobStatus ExtractFile(const wxString& target_file_path, const wxString& write_file_path);
    ISOJobStatus ExtractFolder(const wxString& target_dir_path, const wxString& write_folder_path);
    bool Valid() { return udf != nullptr; }
};

class ISOExtractorIso9660 : public ISOExtractor
{
  private:
    iso9660_t* iso;
    uint8_t joliet_level;
    ISOJobStatus extract_folder(CdioISO9660FileList_t* target_entlist, const wxString& target_dir, const wxString& new_dirname, const wxString& write_folder_path);
    void create_file_list(CdioISO9660FileList_t* entlist, const wxString& dir);
  public:
    ISOExtractorIso9660(CustomThread* thread, const wxString& iso_file_path);
    ~ISOExtractorIso9660();
    ISOJobStatus ExtractFile(const wxString& target_file_path, const wxString& write_file_path);
    ISOJobStatus ExtractFolder(const wxString& target_dir_path, const wxString& write_folder_path);
    bool Valid() { return iso != nullptr; }
};

class ISO
{
  public:
    static ISOExtractor* CreateExtractor(CustomThread* thread, const wxString& iso_file_path);
};

#endif
