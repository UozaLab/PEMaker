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

#include "FrameImpl.h"
#include "MiscWx.h"
#include "Widgets/Defs.h"
#include "resource.h"
#include <wx/msw/private.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>

wxFrame* Container_Util::BaseFrame = nullptr;
wxFrame* Container_Util::CurrentFrame = nullptr;


//
// ProgressHandler
//
ProgressHandler::ProgressHandler(wxEvtHandler* _event_handler)
     : event_handler(_event_handler), ready_to_goback(false), ready_to_gonext(false), error(false)
{
    event_handler->Bind(myEVT_PROGRESS, &ProgressHandler::OnProgress, this);
    event_handler->Bind(myEVT_MSG, &ProgressHandler::OnMsg, this);
    event_handler->Bind(myEVT_ERROR, &ProgressHandler::OnError, this);
    event_handler->Bind(wxEVT_THREAD, &ProgressHandler::OnThreadEvent, this);
}

ProgressHandler::~ProgressHandler()
{
    event_handler->Unbind(wxEVT_THREAD, &ProgressHandler::OnThreadEvent, this);
    event_handler->Unbind(myEVT_ERROR, &ProgressHandler::OnError, this);
    event_handler->Unbind(myEVT_MSG, &ProgressHandler::OnMsg, this);
    event_handler->Unbind(myEVT_PROGRESS, &ProgressHandler::OnProgress, this);
}

void ProgressHandler::write_msg(wxTextCtrl* ctrl, const wxString& str)
{
    ctrl->AppendText(wxDateTime::Now().Format(wxString("%H:%M:%S ")));
    ctrl->AppendText(wxString::Format("%s\r\n", str));
}

void ProgressHandler::write_msg(wxTextCtrl* ctrl, const wxString& str, wxColor color)
{
    ctrl->AppendText(wxDateTime::Now().Format(wxString("%H:%M:%S ")));
    long start = ctrl->GetInsertionPoint();
    ctrl->AppendText(str);
    long end = ctrl->GetInsertionPoint();
    wxTextAttr style(color);
    ctrl->SetStyle(start, end, style);
    ctrl->AppendText("\r\n");
}

void ProgressHandler::write_msg(wxTextCtrl* ctrl, const MsgEvent& msg)
{
    long start = ctrl->GetInsertionPoint();
    if(msg.IsControled())
    {
        ctrl->AppendText(wxDateTime::Now().Format(wxString("%H:%M:%S ")));
        start = ctrl->GetInsertionPoint();
        ctrl->AppendText(msg.GetData());
    }
    else
    {
        ctrl->AppendText(msg.GetData());
    }
    long end = ctrl->GetInsertionPoint();

    if(msg.IsColorized())
    {
        wxTextAttr style(msg.GetColor());
        ctrl->SetStyle(start, end, style);
    }
    if(msg.AppendCRLF()) ctrl->AppendText("\r\n");
}

//
// FrameImpl
//
FrameImpl::FrameImpl()
     : BaseFrame(nullptr)
{
    HICON hicon = (HICON) LoadIcon(wxGetInstance(), MAKEINTRESOURCE(IDI_ICON1));
    wxIcon icon;
    icon.CreateFromHICON(hicon);
    SetIcon(icon);

    Container_Util::SetNewframe(this);
    Container_Util::SetNewframe(new Container_MainImpl(this));
}

void FrameImpl::OnClose( wxCloseEvent& event )
{
    Container_Util::DisposeCurrentFrame();
    Destroy();
}

//
// Container_MainImpl
//
Container_MainImpl::Container_MainImpl(FrameImpl* frame_impl)
     : Container_Main(nullptr), ContainerCommon(frame_impl)
{
    wxFileName file = wxFileName(wxStandardPaths::Get().GetExecutablePath());
    file.AppendDir("lang");
    wxArrayString as;
    int num_files = wxDir::GetAllFiles(file.GetPath(), &as);
    for(int i = 0; i < num_files; i++)
    {
        wxFileName lang_file = wxFileName(as[i]);
        m_choice1->Append(lang_file.GetName());
    }
    if(num_files == 0)
    {
        m_choice1->Enable(false);
    }
    m_bitmap1->SetSize(16, 16);
    Utility::SetIcon(m_bitmap1, IDB_PNG13);
    m_bitmap1->SetBackgroundColour(COLOR_BACKGROUND_PANELBASE);
    m_choice1->Show(false);
    
    m_title->SetLabel(wxString::Format("PEMaker %s", Utility::GetVersion()));
    Utility::SetIcon(m_customControl61, IDB_PNG9, ttt("SelectFileToBuild"));
    m_customControl61->SetFrameMode(true);
    m_customControl61->SetNormalColour(COLOR_BUTTON_FACE_FRAME);
    m_customControl61->Bind(myEVT_SimpleButtonClicked, &Container_MainImpl::OnClickButton, this);
    Layout();
}

void Container_MainImpl::OnClose( wxCloseEvent& event )
{
    m_customControl61->Unbind(myEVT_SimpleButtonClicked, &Container_MainImpl::OnClickButton, this);
    Destroy();
}

void Container_MainImpl::OnClickButton( wxCommandEvent& event )
{
    wxFileDialog openFileDialog(nullptr, ttt("OpenImageFile"), "", "",
                                "ISO files (*.iso)|*.iso|All (*)|*", 
                                wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    frame_impl->Holder()->FilePath.IsoFilePath = openFileDialog.GetPath();
    Container_Util::SetNewframe(new Container_CheckISO(frame_impl));
}

void Container_MainImpl::OnLeftDown( wxMouseEvent& event )
{
    m_choice1->Show(true);
    Layout();
}

void Container_MainImpl::OnChoice( wxCommandEvent& event )
{
    int target = m_choice1->GetSelection();
    wxString selected_item_text = m_choice1->GetString(target);
    MultiLanguage::Instance()->SetLocale(selected_item_text);
    m_choice1->Show(false);
    Container_Util::SetNewframe(new Container_MainImpl(frame_impl));
}

//
// Container_CheckISO
//
Container_CheckISO::Container_CheckISO(FrameImpl* frame_impl)
     : Container_Common_Progress(nullptr), ProgressHandler(this), ContainerCommon(frame_impl)
{
    m_prevnext->SetEnabledNext(true);
    m_prevnext->SetEnabledPrev(false);
    m_prevnext->SetTextNext(ttt("CancelBtnText"));
    m_title->SetLabel(ttt("Title_CheckISO"));
    m_prevnext->GetButtonNext()->Bind(myEVT_SimpleButtonClicked, &Container_CheckISO::OnClickNextButton, this);
    m_prevnext->GetButtonPrev()->Bind(myEVT_SimpleButtonClicked, &Container_CheckISO::OnClickPrevButton, this);
    check_iso_image_worker = new CheckISOImage(this, frame_impl->Holder());
    check_iso_image_worker->Run();
}

void Container_CheckISO::OnClose( wxCloseEvent& event )
{
    m_prevnext->GetButtonNext()->Unbind(myEVT_SimpleButtonClicked, &Container_CheckISO::OnClickNextButton, this);
    m_prevnext->GetButtonPrev()->Unbind(myEVT_SimpleButtonClicked, &Container_CheckISO::OnClickPrevButton, this);
    delete check_iso_image_worker;
    Destroy();
}

void Container_CheckISO::OnClickPrevButton( wxCommandEvent& event )
{
    Container_Util::SetNewframe(new Container_MainImpl(frame_impl));
}

void Container_CheckISO::OnClickNextButton( wxCommandEvent& event )
{
    if(ready_to_goback)
      Container_Util::SetNewframe(new Container_MainImpl(frame_impl));
    else if(ready_to_gonext)
      Container_Util::SetNewframe(new Container_SelectArchImpl(frame_impl));
    else 
      check_iso_image_worker->Terminate();
}

void Container_CheckISO::OnError(ErrorEvent& event)
{
    progress->SetError(true);
    write_msg(m_textCtrl1, wxString::Format("### ERROR ### %s", event.GetData()), wxColour(255, 0, 0));
}

void Container_CheckISO::OnMsg(MsgEvent& event)
{
    write_msg(m_textCtrl1, event);
}

void Container_CheckISO::OnThreadEvent(wxThreadEvent& event)
{
    ThreadState thread_state = event.GetPayload<ThreadState>();

    if(thread_state == THREAD_STARTED)
    {
    }
    else if(thread_state == THREAD_FINISHED)
    {
        m_prevnext->SetEnabledPrev(true);
        m_prevnext->SetTextNext(ttt("NextBtnText"));
        ready_to_gonext = true;
    }
    else if(thread_state == THREAD_TERMINATING)
    {
        m_prevnext->SetTextNext(ttt("CancelingBtnText"));
    }
    else if(thread_state == THREAD_TERMINATED)
    {
        m_prevnext->SetTextNext(ttt("CanceledBtnText"));
        ready_to_goback = true;
    }
}

void Container_CheckISO::OnProgress(ProgressEvent& event)
{
    progress->SetProgress(event.GetData());
}


//
// Container_SelectArchImpl
//
Container_SelectArchImpl::Container_SelectArchImpl(FrameImpl* frame_impl)
     : Container_SelectArch(nullptr), ContainerCommon(frame_impl)
{
    m_title->SetLabel(ttt("Title_Select_Arch"));

    m_prevnext->GetButtonPrev()->Bind(myEVT_SimpleButtonClicked, &Container_SelectArchImpl::OnClickPrevButton, this);
    m_prevnext->GetButtonNext()->Bind(myEVT_SimpleButtonClicked, &Container_SelectArchImpl::OnClickNextButton, this);
    m_prevnext->SetEnabledNext(false);
    m_prevnext->SetEnabledPrev(true);

    m_customControl61->Bind(myEVT_SimpleButtonClicked, &Container_SelectArchImpl::OnClickButton32bit, this);
    m_customControl611->Bind(myEVT_SimpleButtonClicked, &Container_SelectArchImpl::OnClickButton64bit, this);

    Utility::SetIcon(m_customControl61, IDB_PNG12, ttt("Select32bit"));
    m_customControl61->SetFrameMode(true);
    m_customControl61->SetNormalColour(COLOR_BUTTON_FACE_FRAME);

    Utility::SetIcon(m_customControl611, IDB_PNG12, ttt("Select64bit"));
    m_customControl611->SetFrameMode(true);
    m_customControl611->SetNormalColour(COLOR_BUTTON_FACE_FRAME);

    m_arch->SetLabel(wxString::Format("%s\r\n%s", ttt("ArchDesc1"), ttt("ArchDesc2")));

}
void Container_SelectArchImpl::OnClose( wxCloseEvent& event )
{
    m_customControl61->Unbind(myEVT_SimpleButtonClicked, &Container_SelectArchImpl::OnClickButton32bit, this);
    m_customControl611->Unbind(myEVT_SimpleButtonClicked, &Container_SelectArchImpl::OnClickButton64bit, this);
    m_prevnext->GetButtonPrev()->Unbind(myEVT_SimpleButtonClicked, &Container_SelectArchImpl::OnClickPrevButton, this);
    m_prevnext->GetButtonNext()->Unbind(myEVT_SimpleButtonClicked, &Container_SelectArchImpl::OnClickNextButton, this);

    Destroy();
}

void Container_SelectArchImpl::OnClickPrevButton( wxCommandEvent& event )
{
    Container_Util::SetNewframe(new Container_MainImpl(frame_impl));
}

void Container_SelectArchImpl::OnClickNextButton( wxCommandEvent& event )
{
    Container_Util::SetNewframe(new Container_SelectMbrGptImpl(frame_impl));
}

void Container_SelectArchImpl::OnClickButton32bit( wxCommandEvent& event )
{
    frame_impl->Holder()->ISOData.Arch32bit = true;
    m_customControl61->SetSelected(true);
    m_customControl611->SetSelected(false);
    m_prevnext->SetEnabledNext(true);
}
void Container_SelectArchImpl::OnClickButton64bit( wxCommandEvent& event )
{
    frame_impl->Holder()->ISOData.Arch32bit = false;
    m_customControl61->SetSelected(false);
    m_customControl611->SetSelected(true);
    m_prevnext->SetEnabledNext(true);
}


//
// Container_SelectMbrGptImpl
//
Container_SelectMbrGptImpl::Container_SelectMbrGptImpl(FrameImpl* frame_impl)
     : Container_SelectMbrGpt(nullptr), ContainerCommon(frame_impl)
{
    m_title->SetLabel(ttt("Title_Select_System"));

    m_prevnext->GetButtonPrev()->Bind(myEVT_SimpleButtonClicked, &Container_SelectMbrGptImpl::OnClickPrevButton, this);
    m_prevnext->GetButtonNext()->Bind(myEVT_SimpleButtonClicked, &Container_SelectMbrGptImpl::OnClickNextButton, this);
    m_prevnext->SetEnabledNext(false);
    m_prevnext->SetEnabledPrev(true);
    m_prevnext->SetTextNext(ttt("StartBtnText"));
    m_prevnext->SetEnhanceNext(true);

    m_customControl61->Bind(myEVT_SimpleButtonClicked, &Container_SelectMbrGptImpl::OnClickButtonMbr, this);
    m_customControl611->Bind(myEVT_SimpleButtonClicked, &Container_SelectMbrGptImpl::OnClickButtonGpt, this);

    Utility::SetIcon(m_customControl61, IDB_PNG12, ttt("SelectMbr"));
    m_customControl61->SetFrameMode(true);
    m_customControl61->SetNormalColour(COLOR_BUTTON_FACE_FRAME);

    Utility::SetIcon(m_customControl611, IDB_PNG12, ttt("SelectGpt"));
    m_customControl611->SetFrameMode(true);
    m_customControl611->SetNormalColour(COLOR_BUTTON_FACE_FRAME);

    m_mbrgpt->SetLabel(wxString::Format("%s\r\n%s", ttt("MbrGptDesc1"), ttt("MbrGptDesc2")));

}
void Container_SelectMbrGptImpl::OnClose( wxCloseEvent& event )
{
    m_customControl61->Unbind(myEVT_SimpleButtonClicked, &Container_SelectMbrGptImpl::OnClickButtonMbr, this);
    m_customControl611->Unbind(myEVT_SimpleButtonClicked, &Container_SelectMbrGptImpl::OnClickButtonGpt, this);
    m_prevnext->GetButtonPrev()->Unbind(myEVT_SimpleButtonClicked, &Container_SelectMbrGptImpl::OnClickPrevButton, this);
    m_prevnext->GetButtonNext()->Unbind(myEVT_SimpleButtonClicked, &Container_SelectMbrGptImpl::OnClickNextButton, this);

    Destroy();
}

void Container_SelectMbrGptImpl::OnClickPrevButton( wxCommandEvent& event )
{
    Container_Util::SetNewframe(new Container_SelectArchImpl(frame_impl));
}

void Container_SelectMbrGptImpl::OnClickNextButton( wxCommandEvent& event )
{
    Container_Util::SetNewframe(new Container_Extract(frame_impl));
}

void Container_SelectMbrGptImpl::OnClickButtonMbr( wxCommandEvent& event )
{
    frame_impl->Holder()->ISOData.Mbr = true;
    m_customControl61->SetSelected(true);
    m_customControl611->SetSelected(false);
    m_prevnext->SetEnabledNext(true);
}

void Container_SelectMbrGptImpl::OnClickButtonGpt( wxCommandEvent& event )
{
    frame_impl->Holder()->ISOData.Mbr = false;
    m_customControl61->SetSelected(false);
    m_customControl611->SetSelected(true);
    m_prevnext->SetEnabledNext(true);
}

//
// Container_Extract
//
Container_Extract::Container_Extract(FrameImpl* frame_impl)
     : Container_Common_Progress(nullptr), ProgressHandler(this), ContainerCommon(frame_impl)
{
    m_title->SetLabel(ttt("Title_Make_ISO"));
    m_prevnext->SetEnabledNext(true);
    m_prevnext->SetEnabledPrev(false);
    m_prevnext->SetTextNext(ttt("CancelBtnText"));
    m_prevnext->GetButtonNext()->Bind(myEVT_SimpleButtonClicked, &Container_Extract::OnClickNextButton, this);
    make_pe_image_worker = new MakePEImage(this, frame_impl->Holder());
    make_pe_image_worker->Run();
}

void Container_Extract::OnClose( wxCloseEvent& event )
{
    m_prevnext->GetButtonNext()->Unbind(myEVT_SimpleButtonClicked, &Container_Extract::OnClickNextButton, this);
    delete make_pe_image_worker;
    Destroy();
}

void Container_Extract::OnClickNextButton( wxCommandEvent& event )
{
    if(ready_to_goback)
      Container_Util::SetNewframe(new Container_MainImpl(frame_impl));
    else if(ready_to_gonext)
      Container_Util::SetNewframe(new Container_CompleteImpl(frame_impl));
    else
      make_pe_image_worker->Terminate();
}

void Container_Extract::OnError(ErrorEvent& event)
{
    error = true;
    progress->SetError(true);
    write_msg(m_textCtrl1, wxString::Format("### ERROR ### %s", event.GetData()), wxColour(255, 0, 0));
}

void Container_Extract::OnMsg(MsgEvent& event)
{
    write_msg(m_textCtrl1, event);
}

void Container_Extract::OnThreadEvent(wxThreadEvent& event)
{
    ThreadState thread_state = event.GetPayload<ThreadState>();

    if(thread_state == THREAD_STARTED)
    {
    }
    else if(thread_state == THREAD_FINISHED)
    {
        if(error)
        {
            m_prevnext->SetTextNext(ttt("ErrorBtnText"));
            ready_to_goback = true;
        }
        else
        {
            write_msg(m_textCtrl1, wxEmptyString);
            write_msg(m_textCtrl1, ttt("QuitOrProceed"));
            m_prevnext->SetTextNext(ttt("NextBtnText"));
            m_title->SetLabel(ttt("Title_Make_ISO_Done"));
            ready_to_gonext = true;
        }
    }
    else if(thread_state == THREAD_TERMINATING)
    {
        m_prevnext->SetTextNext(ttt("CancelingBtnText"));
    }
    else if(thread_state == THREAD_TERMINATED)
    {
        m_prevnext->SetTextNext(ttt("CanceledBtnText"));
        ready_to_goback = true;
    }
}

void Container_Extract::OnProgress(ProgressEvent& event)
{
    progress->SetProgress(event.GetData());
}


//
// Container_CompleteImpl
//
Container_CompleteImpl::Container_CompleteImpl(FrameImpl* frame_impl)
     : Container_Complete(nullptr), ContainerCommon(frame_impl)
{
    m_title->SetLabel(ttt("Title_Complete"));
    
    Utility::SetIcon(m_customControl61, IDB_PNG9, ttt("CompleteItemOpen"));
    m_customControl61->SetFrameMode(true);
    m_customControl61->SetNormalColour(COLOR_BUTTON_FACE_FRAME);
    m_customControl61->Bind(myEVT_SimpleButtonClicked, &Container_CompleteImpl::OnClickButtonOpenFolder, this);

    Utility::SetIcon(m_customControl611, IDB_PNG3, ttt("CompleteItemBack"));
    m_customControl611->SetFrameMode(true);
    m_customControl611->SetNormalColour(COLOR_BUTTON_FACE_FRAME);
    m_customControl611->Bind(myEVT_SimpleButtonClicked, &Container_CompleteImpl::OnClickButtonTop, this);

    Utility::SetIcon(m_customControl612, IDB_PNG14, ttt("CompleteItemQuit"));
    m_customControl612->SetFrameMode(true);
    m_customControl612->SetNormalColour(COLOR_BUTTON_FACE_FRAME);
    m_customControl612->Bind(myEVT_SimpleButtonClicked, &Container_CompleteImpl::OnClickButtonQuit, this);
}

void Container_CompleteImpl::OnClose( wxCloseEvent& event )
{
    m_customControl612->Unbind(myEVT_SimpleButtonClicked, &Container_CompleteImpl::OnClickButtonQuit, this);
    m_customControl611->Unbind(myEVT_SimpleButtonClicked, &Container_CompleteImpl::OnClickButtonTop, this);
    m_customControl61->Unbind(myEVT_SimpleButtonClicked, &Container_CompleteImpl::OnClickButtonOpenFolder, this);
    Destroy();
}

void Container_CompleteImpl::OnClickButtonTop( wxCommandEvent& event )
{
    Container_Util::SetNewframe(new Container_MainImpl(frame_impl));
}

void Container_CompleteImpl::OnClickButtonQuit( wxCommandEvent& event )
{
    frame_impl->Close();
}

void Container_CompleteImpl::OnClickButtonOpenFolder( wxCommandEvent& event )
{
    wxExecute(wxString::Format("explorer %s", WorkDir::GetWorkDir().GetFullPath()));
}
