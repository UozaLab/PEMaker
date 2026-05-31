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

#ifndef __FRAME_IMPL_H__
#define __FRAME_IMPL_H__

#include "BaseFrame.h"
#include "DataHolder.h"
#include "Worker/Events.h"
#include "Widgets/ProgressPanel.h"
#include "Worker/CheckISOImage.h"
#include "Worker/MakePEImage.h"

class ProgressHandler
{
private:

protected:
    bool ready_to_goback;
    bool ready_to_gonext;
    bool error;
    wxEvtHandler* event_handler;
    void write_msg(wxTextCtrl* ctrl, const wxString& str);
    void write_msg(wxTextCtrl* ctrl, const wxString& str, wxColor color);
    void write_msg(wxTextCtrl* ctrl, const MsgEvent& msg);

public:
    ProgressHandler(wxEvtHandler* _event_handler);
    virtual ~ProgressHandler();

    virtual void OnProgress(ProgressEvent& event){ event.Skip(); }
    virtual void OnMsg(MsgEvent& event){ event.Skip(); }
    virtual void OnError(ErrorEvent& event){ event.Skip(); }
    virtual void OnThreadEvent(wxThreadEvent& event){ event.Skip(); }

};


class FrameImpl : public BaseFrame
{
  private:
    wxFrame* CurrentFrame;
    DataHolder dataholder;
  public:
    FrameImpl();
    DataHolder* Holder() { return &dataholder; }
    void OnClickButton( wxCommandEvent& event );
    void OnClose( wxCloseEvent& event );
};

class ContainerCommon
{
  protected:
    FrameImpl* frame_impl;
  public:
    ContainerCommon(FrameImpl* _frame_impl)
         :frame_impl(_frame_impl) {}
};

class Container_MainImpl : public Container_Main, ContainerCommon
{
  public:
    Container_MainImpl(FrameImpl* frame_impl);
    void OnClose( wxCloseEvent& event );
    void OnClickREButton( wxCommandEvent& event );
    void OnClickISOButton( wxCommandEvent& event );
    void OnLeftDown( wxMouseEvent& event );
    void OnChoice( wxCommandEvent& event );

};

class Container_CheckISO : public Container_Common_Progress, ProgressHandler, ContainerCommon
{
  private:
    CheckISOImage* check_iso_image_worker;
  public:
    Container_CheckISO(FrameImpl* frame_impl);
    void OnClose( wxCloseEvent& event );
    void OnClickPrevButton( wxCommandEvent& event );
    void OnClickNextButton( wxCommandEvent& event );
    void OnMsg(MsgEvent& event);
    void OnError(ErrorEvent& event);
    void OnThreadEvent(wxThreadEvent& event);
    void OnProgress(ProgressEvent& event);
};

class Container_SelectArchImpl : public Container_SelectArch, ContainerCommon
{
  public:
    Container_SelectArchImpl(FrameImpl* frame_impl);
    void OnClose( wxCloseEvent& event );
    void OnClickPrevButton( wxCommandEvent& event );
    void OnClickNextButton( wxCommandEvent& event );
    void OnClickButton32bit( wxCommandEvent& event );
    void OnClickButton64bit( wxCommandEvent& event );
};

class Container_SelectMbrGptImpl : public Container_SelectMbrGpt, ContainerCommon
{
  public:
    Container_SelectMbrGptImpl(FrameImpl* frame_impl);
    void OnClose( wxCloseEvent& event );
    void OnClickPrevButton( wxCommandEvent& event );
    void OnClickNextButton( wxCommandEvent& event );
    void OnClickButtonMbr( wxCommandEvent& event );
    void OnClickButtonGpt( wxCommandEvent& event );
};

class Container_Extract : public Container_Common_Progress, ProgressHandler, ContainerCommon
{
  private:
    MakePEImage* make_pe_image_worker;
  public:
    Container_Extract(FrameImpl* frame_impl);
    void OnClose( wxCloseEvent& event );
    void OnClickNextButton( wxCommandEvent& event );
    void OnMsg(MsgEvent& event);
    void OnError(ErrorEvent& event);
    void OnThreadEvent(wxThreadEvent& event);
    void OnProgress(ProgressEvent& event);
};

class Container_CompleteImpl : public Container_Complete, ContainerCommon
{
  public:
    Container_CompleteImpl(FrameImpl* frame_impl);
    void OnClose( wxCloseEvent& event );
    void OnClickButtonOpenFolder( wxCommandEvent& event );
    void OnClickButtonTop( wxCommandEvent& event );
    void OnClickButtonQuit( wxCommandEvent& event );
};

class Container_Util
{
  private:
    static wxFrame* BaseFrame;
    static wxFrame* CurrentFrame;
  public:
    static void DisposeCurrentFrame()
    {
        if(CurrentFrame != nullptr)
        {
            CurrentFrame->Destroy();
            delete CurrentFrame;
        }
        CurrentFrame = nullptr;
    }

    static void SetNewframe(wxFrame* newframe)
    {
        if(BaseFrame == nullptr)
        {
            BaseFrame = newframe;
            return;
        }

        wxWindow* panel_root = BaseFrame->FindWindow(wxID_PANEL_ROOT);
        const wxWindowList nodes = panel_root->GetChildren();
        for(wxWindowList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            wxWindow* win = *it;
            win->Show(false);
        }

        if(CurrentFrame != NULL)
        {
            CurrentFrame->Close();
            delete CurrentFrame;
        
            const wxWindowList nodes = panel_root->GetChildren();
            for(wxWindowList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
            {
                wxWindow* win = *it;
                win->Destroy();
            }
        }

        CurrentFrame = newframe;

        wxWindow* found_panel = newframe->FindWindow(wxID_PANEL_ROOT);
        if(found_panel != nullptr)
        {
            found_panel->Reparent(panel_root);
            wxSizer* sizer = found_panel->GetContainingSizer();
            sizer->Detach(found_panel);
            panel_root->GetSizer()->Add(found_panel, 1, wxEXPAND | wxALL, 0);
            panel_root->Layout();
        }
    }
    
    static bool SetFrameLayout(wxWindow* child)
    {
        wxWindow* target = child;
        do
        {
            if(typeid(*target) ==  typeid(FrameImpl))
            {
                target->Layout();
                return true;
            }
            target = target->GetParent();
        }while(target != NULL);

        return false;
    }
};

#endif
