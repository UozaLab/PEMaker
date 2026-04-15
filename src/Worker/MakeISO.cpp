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
#include "MakeISO.h"
#include "tstring.h"
#include <wx/filename.h>
#include "ckcore/filestream.hh"
#include "ckfilesystem/const.hh"
#include "ckfilesystem/filesystemwriter.hh"
#include "ckfilesystem/filesystem.hh"
#include "ckfilesystem/iso.hh"

MakeISO::MakeISO(CustomThread* _thread, const wxString& _iso_file_path, const wxString& _base_dir, bool _uefi)
 : thread(_thread), file_comparator(false), file_set(file_comparator),
   iso_file_path(_iso_file_path), bootable(false), uefi(_uefi)
{
    if(_base_dir.EndsWith(_T("\\")))
    {
        base_dir = _base_dir.BeforeLast(_T('\\'));
    }
    else
    {
        base_dir = _base_dir;
    }
}

bool MakeISO::Make()
{
    wxDir dir(base_dir);
    if(!dir.IsOpened()) return false;

    DirTraverse traverser(base_dir, file_set);
    dir.Traverse(traverser, wxEmptyString, wxDIR_DEFAULT);

    ckcore::FileOutStream filestream(iso_file_path.wc_str());
    if (!filestream.open())
    {
        return false;
    }

    Log log;
    Progress progress(thread);
    ckfilesystem::FileSystem file_sys(ckfilesystem::FileSystem::TYPE_ISO_UDF_JOLIET, file_set);
    file_sys.set_interchange_level(ckfilesystem::Iso::LEVEL_3);
    file_sys.set_long_joliet_names(true);
    file_sys.set_udf_shortad(true);
    if(bootable)
    {
        wxFileName filename(boot_image_path);
        if(!filename.FileExists())
        {
            return false;
        }
        wxULongLong filesize = filename.GetSize();
        filesize /= 512;
        unsigned long filesize_ul = filesize.ToULong();
        if(!file_sys.add_boot_image_no_emu(boot_image_path, boot_image_internal_path.wc_str(),
                                               true, 0x0000, (ckcore::tuint16) filesize_ul))
        {
            return false;
        }
    }
    file_sys.set_volume_label(_T("PE Volume"));
    ckfilesystem::FileSystemWriter writer(log, file_sys, true);
    if(catalog_filepath != wxEmptyString && catalog_internal_path != wxEmptyString)
      writer.set_catalog_file_path(catalog_filepath.wc_str(), catalog_internal_path.wc_str());
    if (writer.write(filestream, progress) != RESULT_OK)
    {
        return false;
    }      
    return true;
}

void MakeISO::SetBootable(const wxString& _boot_image_path, const wxString& _boot_image_internal_path)
{
    boot_image_path = _boot_image_path;
    boot_image_internal_path = _boot_image_internal_path;
    bootable = true;
}
