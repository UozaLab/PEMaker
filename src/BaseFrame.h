///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include "Widgets/ProgressPanel.h"
#include <wx/textctrl.h>
#include "Widgets/PrevNextPanel.h"
#include <wx/scrolwin.h>
#include <wx/frame.h>
#include "Widgets/SimpleButton.h"
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/choice.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_PANEL_ROOT 1000
#define wxID_RESTORE_SELECTFILE 1001

///////////////////////////////////////////////////////////////////////////////
/// Class Container_Common_Progress
///////////////////////////////////////////////////////////////////////////////
class Container_Common_Progress : public wxFrame
{
	private:

	protected:
		wxPanel* m_panel71;
		wxStaticText* m_title;
		wxPanel* m_panel171;
		wxPanel* m_panel20;
		ProgressPanel* progress;
		wxTextCtrl* m_textCtrl1;
		PrevNextPanel* m_prevnext;

	public:
		wxScrolledWindow* m_panel_root;

		Container_Common_Progress( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~Container_Common_Progress();

};

///////////////////////////////////////////////////////////////////////////////
/// Class BaseFrame
///////////////////////////////////////////////////////////////////////////////
class BaseFrame : public wxFrame
{
	private:

	protected:

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }


	public:
		wxPanel* m_panel_root;

		BaseFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~BaseFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Container_Main
///////////////////////////////////////////////////////////////////////////////
class Container_Main : public wxFrame
{
	private:

	protected:
		wxScrolledWindow* m_scrolledwindow;
		wxPanel* m_panel71;
		wxStaticText* m_title;
		wxPanel* m_panel171;
		SimpleButton* m_customControl61;
		wxPanel* m_panel33;
		wxStaticBitmap* m_bitmap1;
		wxChoice* m_choice1;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnChoice( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxPanel* m_panel_root;

		Container_Main( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~Container_Main();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Container_SelectArch
///////////////////////////////////////////////////////////////////////////////
class Container_SelectArch : public wxFrame
{
	private:

	protected:
		wxScrolledWindow* m_scrolledwindow;
		wxPanel* m_panel71;
		wxStaticText* m_title;
		wxPanel* m_panel171;
		SimpleButton* m_customControl61;
		SimpleButton* m_customControl611;
		wxStaticText* m_arch;
		PrevNextPanel* m_prevnext;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }


	public:
		wxPanel* m_panel_root;

		Container_SelectArch( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~Container_SelectArch();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Container_SelectMbrGpt
///////////////////////////////////////////////////////////////////////////////
class Container_SelectMbrGpt : public wxFrame
{
	private:

	protected:
		wxScrolledWindow* m_scrolledwindow;
		wxPanel* m_panel71;
		wxStaticText* m_title;
		wxPanel* m_panel171;
		SimpleButton* m_customControl61;
		SimpleButton* m_customControl611;
		wxStaticText* m_mbrgpt;
		PrevNextPanel* m_prevnext;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }


	public:
		wxPanel* m_panel_root;

		Container_SelectMbrGpt( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~Container_SelectMbrGpt();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Container_Complete
///////////////////////////////////////////////////////////////////////////////
class Container_Complete : public wxFrame
{
	private:

	protected:
		wxScrolledWindow* m_scrolledwindow;
		wxPanel* m_panel71;
		wxStaticText* m_title;
		wxPanel* m_panel171;
		SimpleButton* m_customControl61;
		SimpleButton* m_customControl611;
		SimpleButton* m_customControl612;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }


	public:
		wxPanel* m_panel_root;

		Container_Complete( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~Container_Complete();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Container_Sample
///////////////////////////////////////////////////////////////////////////////
class Container_Sample : public wxFrame
{
	private:

	protected:
		wxScrolledWindow* m_scrolledwindow;
		wxPanel* m_panel71;
		wxStaticText* m_title;
		wxPanel* m_panel171;
		SimpleButton* m_customControl61;
		SimpleButton* m_customControl611;
		PrevNextPanel* m_prevnext;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }


	public:
		wxPanel* m_panel_root;

		Container_Sample( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~Container_Sample();

};

