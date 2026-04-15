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

#ifndef __DATA_HOLDER__H__
#define __DATA_HOLDER__H__

#include <wx/wx.h>

struct ISO_DATA
{
    bool Arch32bit;
    bool Mbr;
    ISO_DATA()
    {
        Arch32bit = false;
        Mbr = false;
    }
};

struct FILE_PATH
{
    wxString IsoFilePath;
    FILE_PATH()
    {
        IsoFilePath = wxEmptyString;
    }
};

struct DataHolder
{
    FILE_PATH FilePath;
    ISO_DATA ISOData;
};

#endif
