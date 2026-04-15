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

#include "ISO.h"
#include "tstring.h"
#include "MiscWx.h"
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/dir.h>

#define CEILING(x, y) ((x+(y-1))/y)

wxString Join(const wxString& path, const wxString& path_or_file)
{
    wxString path_or_file_trim = path_or_file;
    if(path.EndsWith(_T("/")) && path_or_file_trim.StartsWith(_T("/")))
        path_or_file_trim = path_or_file_trim.Right(path_or_file_trim.Length() - 1);

    if(path_or_file_trim == wxEmptyString)
    {
        if(path.EndsWith(_T("/")))
        {
            return path.Left(path.Length() - 1);
        }
        return path;
    }
    return (path.EndsWith(_T("/")))
           ? path + path_or_file_trim
           : path + _T("/") + path_or_file_trim;
}

wxString ISOExtractor::get_correct_filename(const wxString& src)
{
    const std::map<wxString, std::pair<wxString, iso9660_stat_t*> >::iterator itr = file_list.find(src.Upper());
    if(itr == file_list.end())
        return src;
    return (itr->second).first;
}

bool ISOExtractor::FileExists(const wxString& target_file_path)
{
    const std::map<wxString, std::pair<wxString, iso9660_stat_t*> >::iterator itr = file_list.find(target_file_path.Upper());
    if(itr == file_list.end())
        return false;
    return true;
}


std::pair<wxString, iso9660_stat_t*> ISOExtractor::get_correct_data(const wxString& src)
{
    const std::map<wxString, std::pair<wxString, iso9660_stat_t*> >::iterator itr = file_list.find(src.Upper());
    if(itr == file_list.end())
      return std::make_pair(src, (iso9660_stat_t*)nullptr);
    return itr->second;
}

ISOExtractor::~ISOExtractor()
{
    for(std::map<wxString, std::pair<wxString, iso9660_stat_t*> >::iterator itr = file_list.begin(),
        itr_end = file_list.end(); itr != itr_end; itr++)
    {
        std::pair<wxString, iso9660_stat_t*> data = itr->second;
        if(data.second != nullptr)
          iso9660_stat_free(data.second);
    }
}


ISOExtractorUdf::ISOExtractorUdf(CustomThread* thread, const wxString& iso_file_path)
     : ISOExtractor(thread)
{
    udf = udf_open(Convert::wxString2String(iso_file_path).c_str());
    if(udf)
    {
        udf_dirent_t* udf_root = udf_get_root(udf, true, 0);
        if(udf_root != nullptr)
        {
            create_file_list(udf_root, wxString(_T("/")));
        }
    }
}

ISOExtractorUdf::~ISOExtractorUdf()
{
    if(udf != nullptr)
    {
        udf_close(udf);
    }
}

void ISOExtractorUdf::create_file_list(udf_dirent_t* udf_dirent, const wxString& dir)
{
    if(!udf_dirent) return;
    file_list[dir.Upper()] = std::make_pair(dir, nullptr);

    while (udf_readdir(udf_dirent))
    {
        if (udf_is_dir(udf_dirent))
        {
            udf_dirent_t *udf_dirent2 = udf_opendir(udf_dirent);
            if (udf_dirent2)
            {
                create_file_list(udf_dirent2, Join(dir, Convert::Char2wxString(udf_get_filename(udf_dirent))));
            }
        }
        else
        {
            wxString file_path = Join(dir, Convert::Char2wxString(udf_get_filename(udf_dirent)));
            file_list[file_path.Upper()] = std::make_pair(file_path, nullptr);
        }
    }
}

ISOJobStatus ISOExtractorUdf::ExtractFile(const wxString& target_file_path, const wxString& write_file_path)
{
    ISOJobStatus ret = ISOJobStatus::SUCCESS;
    udf_dirent_t* udf_root = nullptr;
    udf_dirent_t* udf_file = nullptr;

    wxString target_file_path_correct = get_correct_filename(target_file_path);

    udf_root = udf_get_root(udf, true, 0);
    if(!udf_root)
    {
        ret = ISOJobStatus::TARGETFILE_OPEN_ERROR;
        goto END;
    }
    
    udf_file = udf_fopen(udf_root, Convert::wxString2String(target_file_path_correct).c_str());
    if (!udf_file) 
    {
        ret = ISOJobStatus::TARGETFILE_OPEN_ERROR;
        goto END;
    }

    {
    wxFile write_file(write_file_path, wxFile::write);
    if (!write_file.IsOpened())
    {
        ret = ISOJobStatus::WRITEFILE_OPEN_ERROR;
        return ret;
    }

    uint64_t file_length = udf_get_file_length(udf_file);
    const unsigned int i_blocks = (unsigned int)CEILING(file_length, UDF_BLOCKSIZE);
    uint64_t bytes_read_sum = 0;
    int progress_pre = 0;
    for (unsigned int i = 0; i < i_blocks; i++)
    {
        double progress_double = (i == i_blocks - 1) ? 100.0
                                                     : ((double)i / (double)i_blocks) * 100.0;
        int progress = (int) progress_double;
        if(progress_pre != progress)
        {
            if(report_progress_file)
              wxQueueEvent(thread->EventHandler(), new ProgressEvent(progress_double));
        }
        progress_pre = progress;

        if(thread->TerminateRequired())
        {
            ret = ISOJobStatus::TERMINATED;
            goto END;
        }

        char buf[UDF_BLOCKSIZE] = {
            '\0',
        };
        ssize_t bytes_read = udf_read_block(udf_file, buf, 1);

        if (bytes_read < 0)
        {
            ret = ISOJobStatus::TARGETFILE_READ_ERROR;
            goto END;
        }
        if(bytes_read_sum + bytes_read > file_length)
        {
            bytes_read = file_length - bytes_read_sum;
        }
        bytes_read_sum += bytes_read;

        size_t bytes_written = write_file.Write(buf, bytes_read);
        if(bytes_written != bytes_read)
        {
            ret = ISOJobStatus::WRITEFILE_WRITE_ERROR;
            goto END;
        }
    }
    }

END:
    if(udf_root != nullptr)
    {
        udf_dirent_free(udf_root);
    }
    if(udf_file != nullptr)
    {
        udf_dirent_free(udf_file);
    }
    return ret;
}

ISOJobStatus ISOExtractorUdf::extract_folder(udf_dirent_t* target_dirent, const wxString& target_dir_path, const wxString& write_folder_path)
{
    ISOJobStatus ret = ISOJobStatus::SUCCESS;

    wxString dir_name(Convert::Char2wxString(udf_get_filename(target_dirent)));
    dir_name.Replace(_T("/"), _T("\\"));
    wxFileName write_folder(write_folder_path, _T(""));
    if(dir_name != _T("\\"))
    {
        if(dir_name != wxEmptyString)
            write_folder.AppendDir(dir_name);
        if(!write_folder.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
        {
            ret = ISOJobStatus::WRITEFILE_MKDIR_ERROR;
            return ret;
        }
    }

    while (udf_readdir(target_dirent))
    {
        if (udf_is_dir(target_dirent))
        {
            udf_dirent_t *udf_dirent = udf_opendir(target_dirent);
            if(udf_dirent)
            {
                wxString dir_name(Convert::Char2wxString(udf_get_filename(target_dirent)));
                wxString target_dir = (target_dir_path == "/") ? "/" + dir_name : target_dir_path + "/" + dir_name;
                ret = extract_folder(udf_dirent, target_dir, write_folder.GetFullPath());
                if(ret != ISOJobStatus::SUCCESS)
                {
                    udf_dirent_free(target_dirent);
                    return ret;
                }
            }
        }
        else
        {
            wxString file_name(Convert::Char2wxString(udf_get_filename(target_dirent)));
            wxFileName write_file_name(write_folder);
            write_file_name.SetFullName(file_name);
            ExtractFile((target_dir_path == "/") ? "/" + file_name : target_dir_path + "/" + file_name , write_file_name.GetFullPath());
        }
    }

    return ret;
}

ISOJobStatus ISOExtractorUdf::ExtractFolder(const wxString& target_dir_path, const wxString& write_folder_path)
{
    ISOJobStatus ret = ISOJobStatus::SUCCESS;
    wxString target_dir_path_correct = get_correct_filename(target_dir_path);

    udf_dirent_t* udf_dir = nullptr;
    udf_dirent_t* udf_dir_tmp = nullptr;

    udf_dirent_t* udf_root = udf_get_root(udf, true, 0);
    if(!udf_root)
    {
        ret = ISOJobStatus::TARGETFILE_OPEN_ERROR;
        goto END;
    }

    if(target_dir_path_correct == _T("/"))
    {   
        udf_dir = udf_root;
    }
    else
    {
        udf_dir_tmp = udf_fopen(udf_root, Convert::wxString2String(target_dir_path_correct).c_str());
        if (!udf_dir_tmp) 
        {
            ret = ISOJobStatus::TARGETFILE_OPEN_ERROR;
            goto END;
        }
        if (!udf_is_dir(udf_dir_tmp))
        {
            ret = ISOJobStatus::TARGETFILE_OPEN_ERROR;
            goto END;
        }

        udf_dir = udf_opendir(udf_dir_tmp);
        if (!udf_dir) 
        {
            ret = ISOJobStatus::TARGETFILE_OPEN_ERROR;
            goto END;
        }
    }
    if (!udf_is_dir(udf_dir))
    {
        ret = ISOJobStatus::TARGETFILE_OPEN_ERROR;
        goto END;
    }
    ret = extract_folder(udf_dir, target_dir_path_correct, write_folder_path);


END:
    if(udf_dir == udf_root)
        udf_dir = nullptr;

    if(udf_root != nullptr)
    {
        udf_dirent_free(udf_root);
    }
    if(udf_dir_tmp != nullptr)
    {
        udf_dirent_free(udf_dir_tmp);
    }
    if(udf_dir != nullptr)
    {
        udf_dirent_free(udf_dir);
    }
    return ret;
}

void ISOExtractorIso9660::create_file_list(CdioISO9660FileList_t* entlist, const wxString& dir)
{
    if(!entlist) return;
    file_list[dir.Upper()] = std::make_pair(dir, nullptr);
    CdioListNode_t* entnode;
    iso9660_stat_t* statbuf;
    char fullpath[4096];

    _CDIO_LIST_FOREACH (entnode, entlist)
    {
        statbuf = (iso9660_stat_t*) _cdio_list_node_data(entnode);
        if ( (strcmp(statbuf->filename, ".") == 0)
             || (strcmp(statbuf->filename, "..") == 0) )
          continue;
        iso9660_name_translate_ext(statbuf->filename, fullpath, joliet_level);

        if (statbuf->type == iso9660_stat_s::_STAT_DIR)
        {
            wxString filepath = Join(dir, Convert::Char2wxString(fullpath));
            CdioISO9660FileList_t* entlist2 = iso9660_ifs_readdir(iso, Convert::wxString2String(filepath).c_str());

            if(entlist2)
            {
                create_file_list(entlist2, filepath);
                iso9660_filelist_free(entlist2);
            }
        }
        else
        {
            iso9660_stat_t* stat = (iso9660_stat_t*) malloc(sizeof(iso9660_stat_t));
            memcpy(stat, statbuf, sizeof(iso9660_stat_t));
            wxString file_path = Join(dir, Convert::Char2wxString(fullpath));
            file_list[file_path.Upper()] = std::make_pair(file_path, stat);
        }
    }
}

ISOExtractorIso9660::ISOExtractorIso9660(CustomThread* thread, const wxString& iso_file_path)
     : ISOExtractor(thread)
{
    joliet_level = 0;
    iso = iso9660_open_ext(Convert::wxString2String(iso_file_path).c_str(), ISO_EXTENSION_ALL);
    if(iso)
    {
        joliet_level = iso9660_ifs_get_joliet_level(iso);
        CdioISO9660FileList_t* entlist = iso9660_ifs_readdir(iso, "/");
        if(entlist)
        {
            create_file_list(entlist, wxString(_T("/")));
            iso9660_filelist_free(entlist);
        }
    }
}

ISOExtractorIso9660::~ISOExtractorIso9660()
{
    if(iso != nullptr)
    {
        iso9660_close(iso);
    }
}

ISOJobStatus ISOExtractorIso9660::ExtractFile(const wxString& target_file_path, const wxString& write_file_path)
{
    ISOJobStatus ret = ISOJobStatus::SUCCESS;
    std::pair<wxString, iso9660_stat_t*> correct_data = get_correct_data(target_file_path);

    iso9660_stat_t *statbuf = correct_data.second;
    if(statbuf == nullptr)
    {
        ret = ISOJobStatus::TARGETFILE_OPEN_ERROR;
        goto END;
    }

    {
    wxFile write_file(write_file_path, wxFile::write);
    if (!write_file.IsOpened())
    {
        ret = ISOJobStatus::WRITEFILE_OPEN_ERROR;
        return ret;
    }

    uint64_t file_length = statbuf->total_size;
    const unsigned int i_blocks = CEILING(statbuf->total_size, ISO_BLOCKSIZE);
    uint64_t bytes_read_sum = 0;
    int progress_pre = 0;
    for (unsigned i = 0; i < i_blocks ; i++)
    {
        double progress_double = ((double)i / (double)i_blocks) * 100.0;
        int progress = (int) progress_double;
        if(progress_pre != progress)
        {
            if(report_progress_file)
              wxQueueEvent(thread->EventHandler(), new ProgressEvent(progress_double));
        }
        progress_pre = progress;

        if(thread->TerminateRequired())
        {
            ret = ISOJobStatus::TERMINATED;
            goto END;
        }
        
        char buf[ISO_BLOCKSIZE];
        const lsn_t lsn = statbuf->lsn + i;
        memset (buf, 0, ISO_BLOCKSIZE);

        if ( ISO_BLOCKSIZE != iso9660_iso_seek_read(iso, buf, lsn, 1) )
        {
            ret = ISOJobStatus::TARGETFILE_READ_ERROR;
            goto END;
        }
        ssize_t bytes_read = ISO_BLOCKSIZE;
        if(bytes_read_sum + bytes_read > file_length)
        {
            bytes_read = file_length - bytes_read_sum;
        }
        bytes_read_sum += ISO_BLOCKSIZE;

        size_t bytes_written = write_file.Write(buf, bytes_read);
        if(bytes_written != bytes_read)
        {
            ret = ISOJobStatus::WRITEFILE_WRITE_ERROR;
            goto END;
        }
    }
    write_file.Flush();
    }
    
END:
    return ret;
}

ISOJobStatus ISOExtractorIso9660::extract_folder(CdioISO9660FileList_t* target_entlist, const wxString& target_dir, const wxString& new_dirname, const wxString& write_folder_path)
{
    CdioListNode_t* entnode = nullptr;
    iso9660_stat_t* statbuf = nullptr;
    char fullpath[4096];

    ISOJobStatus ret = ISOJobStatus::SUCCESS;
    if(!target_entlist) return ret;

    wxString target_dir_path(Join(target_dir, new_dirname));
    wxString dir_name(new_dirname);
    dir_name.Replace(_T("/"), _T("\\"));
    wxFileName write_folder(write_folder_path, _T(""));
    if(dir_name != _T("\\"))
    {
        if(dir_name.StartsWith(_T("\\")))
            dir_name = dir_name.Right(dir_name.Length() - 1);
        if(dir_name != wxEmptyString)
            write_folder.AppendDir(dir_name);

        wxString tmp(write_folder.GetFullPath());
        tmp ="";

        if(!write_folder.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
        {
            ret = ISOJobStatus::WRITEFILE_MKDIR_ERROR;
            return ret;
        }
    }

    _CDIO_LIST_FOREACH (entnode, target_entlist)
    {
        statbuf = (iso9660_stat_t*) _cdio_list_node_data(entnode);
        if ( (strcmp(statbuf->filename, ".") == 0)
             || (strcmp(statbuf->filename, "..") == 0) )
          continue;
        iso9660_name_translate_ext(statbuf->filename, fullpath, joliet_level);

        if (statbuf->type == iso9660_stat_s::_STAT_DIR)
        {
            wxString filepath = Join(target_dir_path, Convert::Char2wxString(fullpath));
            CdioISO9660FileList_t* entlist2 = iso9660_ifs_readdir(iso, Convert::wxString2String(filepath).c_str());

            if(entlist2)
            {
                ret = extract_folder(entlist2, target_dir_path, Convert::Char2wxString(fullpath), write_folder.GetFullPath());
                iso9660_filelist_free(entlist2);
                if(ret != ISOJobStatus::SUCCESS)
                {
                    return ret;
                }
            }
        }
        else
        {
            wxString file_name(Convert::Char2wxString(fullpath));
            wxFileName write_file_name(write_folder);
            write_file_name.SetFullName(file_name);
            ExtractFile((target_dir_path == "/") ? "/" + file_name : target_dir_path + "/" + file_name , write_file_name.GetFullPath());
        }
    }

    return ret;
}


ISOJobStatus ISOExtractorIso9660::ExtractFolder(const wxString& target_dir_path, const wxString& write_folder_path)
{
    ISOJobStatus ret = ISOJobStatus::SUCCESS;
    wxString target_dir_path_correct = get_correct_filename(target_dir_path);

    CdioISO9660FileList_t* entlist = iso9660_ifs_readdir(iso, Convert::wxString2String(target_dir_path_correct).c_str());
    if(!entlist)
    {
        ret = ISOJobStatus::TARGETFILE_OPEN_ERROR;
        goto END;
    }

    ret = extract_folder(entlist, _T("/"), target_dir_path_correct, write_folder_path);

END:
    if(entlist != nullptr)
      iso9660_filelist_free(entlist);
    return ret;
}


ISOExtractor* ISO::CreateExtractor(CustomThread* thread, const wxString& iso_file_path)
{
    ISOExtractor* extractor = new ISOExtractorUdf(thread, iso_file_path);
    if(!extractor->Valid())
    {
        delete extractor;
        extractor = new ISOExtractorIso9660(thread, iso_file_path);
        if(!extractor->Valid())
        {
            delete extractor;
            extractor = nullptr;
        }
    }
    return extractor;
}
