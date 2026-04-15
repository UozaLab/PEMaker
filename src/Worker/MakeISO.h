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
#ifndef __MAKE_ISO_H__
#define __MAKE_ISO_H__

#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include "Events.h"
#include "ckfilesystem/fileset.hh"
#include "ckcore/log.hh"
#include "ckcore/progress.hh"

class DirTraverse : public wxDirTraverser
{
private:
    ckfilesystem::FileSet &file_set;
    wxString base_dir;

public:
    DirTraverse(const wxString& _base_dir, ckfilesystem::FileSet& fs): base_dir(_base_dir), file_set(fs) { }

    wxDirTraverseResult OnFile(const wxString& filename)
    {
        wxString internal_path = filename;
        internal_path.Replace(base_dir, wxEmptyString, false);
        internal_path.Replace(_T("\\"), _T("/"), true);
        file_set.insert(new ckfilesystem::FileDescriptor(internal_path, filename));
        return wxDIR_CONTINUE;
    }

    wxDirTraverseResult OnDir(const wxString& dirname)
    {
        wxString internal_path = dirname;
        internal_path.Replace(base_dir, wxEmptyString, false);
        internal_path.Replace(_T("\\"), _T("/"), true);
        if(internal_path == _T("/")) return wxDIR_CONTINUE;
        file_set.insert(new ckfilesystem::FileDescriptor(internal_path, dirname,
                                                         ckfilesystem::FileDescriptor::FLAG_DIRECTORY));
        return wxDIR_CONTINUE;
    }
};

class Log : public ckcore::Log
{
public:
    void print(const ckcore::tchar *format,...){}
    void print_line(const ckcore::tchar *format,...){}
};

class Progress : public ckcore::Progress
{
private:
    CustomThread* thread;
    unsigned char last_progress;

public:
    Progress(CustomThread* _thread)
         : thread(_thread), last_progress(0){}

    // ckcore::Progress interface.
    void set_progress(unsigned char progress)
    {
        if(last_progress != progress)
          wxQueueEvent(thread->EventHandler(), new ProgressEvent(progress));
        last_progress = progress;
    }
    void set_status(const ckcore::tchar *format,...){}
    void notify(MessageType type,const ckcore::tchar *format,...){}
    bool cancelled(){ return thread->TerminateRequired(); }
};


class MakeISO
{
private:
    CustomThread* thread;
    ckfilesystem::FileComparator file_comparator;
    ckfilesystem::FileSet file_set;
    wxString iso_file_path;
    wxString base_dir;
    wxString boot_image_path;
    wxString boot_image_internal_path;
    wxString catalog_filepath;
    wxString catalog_internal_path;
    bool bootable;
    bool uefi;
public:
    MakeISO(CustomThread* _thread, const wxString& _iso_file_path, const wxString& _base_dir, bool _uefi);
    void SetBootable(const wxString& _boot_image_path, const wxString& _boot_image_internal_path);
    void SetCatalogFilepath(const wxString& _catalog_filepath, const wxString& _catalog_internal_path)
    {
        catalog_filepath = _catalog_filepath;
        catalog_internal_path = _catalog_internal_path;
    }
    bool Make();
};

#endif
