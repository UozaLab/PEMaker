///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "BaseFrame.h"

///////////////////////////////////////////////////////////////////////////

Container_Common_Progress::Container_Common_Progress( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	m_panel_root = new wxScrolledWindow( this, wxID_PANEL_ROOT, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_panel_root->SetScrollRate( 5, 5 );
	m_panel_root->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	m_panel71 = new wxPanel( m_panel_root, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel71->SetBackgroundColour( wxColour( 225, 225, 225 ) );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );

	m_title = new wxStaticText( m_panel71, wxID_ANY, wxT("title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_title->Wrap( -1 );
	m_title->SetFont( wxFont( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_title->SetForegroundColour( wxColour( 26, 26, 26 ) );

	bSizer111->Add( m_title, 0, wxLEFT, 12 );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );


	m_panel71->SetSizer( bSizer111 );
	m_panel71->Layout();
	bSizer111->Fit( m_panel71 );
	bSizer20->Add( m_panel71, 0, wxEXPAND | wxALL, 0 );

	m_panel171 = new wxPanel( m_panel_root, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel171->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );

	m_panel20 = new wxPanel( m_panel171, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel20->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );

	progress = new ProgressPanel( m_panel20, wxID_ANY, wxDefaultPosition, wxSize( -1,15 ), 0 );
	bSizer32->Add( progress, 0, wxALL|wxEXPAND, 5 );

	m_textCtrl1 = new wxTextCtrl( m_panel20, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxBORDER_STATIC );
	bSizer32->Add( m_textCtrl1, 1, wxALL|wxEXPAND, 5 );


	m_panel20->SetSizer( bSizer32 );
	m_panel20->Layout();
	bSizer32->Fit( m_panel20 );
	bSizer191->Add( m_panel20, 1, wxEXPAND | wxALL, 5 );

	m_prevnext = new PrevNextPanel( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_prevnext, 0, wxALL|wxEXPAND, 5 );


	m_panel171->SetSizer( bSizer191 );
	m_panel171->Layout();
	bSizer191->Fit( m_panel171 );
	bSizer20->Add( m_panel171, 1, wxEXPAND | wxALL, 0 );


	m_panel_root->SetSizer( bSizer20 );
	m_panel_root->Layout();
	bSizer20->Fit( m_panel_root );
	bSizer8->Add( m_panel_root, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer8 );
	this->Layout();

	this->Centre( wxBOTH );
}

Container_Common_Progress::~Container_Common_Progress()
{
}

BaseFrame::BaseFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	m_panel_root = new wxPanel( this, wxID_PANEL_ROOT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel_root->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );


	m_panel_root->SetSizer( bSizer17 );
	m_panel_root->Layout();
	bSizer17->Fit( m_panel_root );
	bSizer14->Add( m_panel_root, 1, wxEXPAND | wxALL, 0 );


	this->SetSizer( bSizer14 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( BaseFrame::OnClose ) );
}

BaseFrame::~BaseFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( BaseFrame::OnClose ) );

}

Container_Main::Container_Main( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	m_panel_root = new wxPanel( this, wxID_PANEL_ROOT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel_root->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );

	m_scrolledwindow = new wxScrolledWindow( m_panel_root, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledwindow->SetScrollRate( 5, 5 );
	m_scrolledwindow->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	m_panel71 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel71->SetBackgroundColour( wxColour( 225, 225, 225 ) );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );

	m_title = new wxStaticText( m_panel71, wxID_ANY, wxT("title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_title->Wrap( -1 );
	m_title->SetFont( wxFont( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_title->SetForegroundColour( wxColour( 26, 26, 26 ) );

	bSizer111->Add( m_title, 0, wxLEFT, 12 );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );


	m_panel71->SetSizer( bSizer111 );
	m_panel71->Layout();
	bSizer111->Fit( m_panel71 );
	bSizer20->Add( m_panel71, 0, wxEXPAND | wxALL, 0 );

	m_panel171 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );

	m_customControl61 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl61, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );


	bSizer191->Add( 0, 0, 1, wxEXPAND, 0 );

	m_panel33 = new wxPanel( m_panel171, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer49;
	bSizer49 = new wxBoxSizer( wxHORIZONTAL );

	m_bitmap1 = new wxStaticBitmap( m_panel33, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_bitmap1->SetToolTip( wxT("Select Language") );

	bSizer49->Add( m_bitmap1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3 );

	wxArrayString m_choice1Choices;
	m_choice1 = new wxChoice( m_panel33, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	m_choice1->SetMaxSize( wxSize( 70,-1 ) );

	bSizer49->Add( m_choice1, 0, wxALL, 2 );


	m_panel33->SetSizer( bSizer49 );
	m_panel33->Layout();
	bSizer49->Fit( m_panel33 );
	bSizer191->Add( m_panel33, 0, wxEXPAND | wxALL, 5 );


	m_panel171->SetSizer( bSizer191 );
	m_panel171->Layout();
	bSizer191->Fit( m_panel171 );
	bSizer20->Add( m_panel171, 1, wxEXPAND | wxALL, 0 );


	m_scrolledwindow->SetSizer( bSizer20 );
	m_scrolledwindow->Layout();
	bSizer20->Fit( m_scrolledwindow );
	bSizer35->Add( m_scrolledwindow, 1, wxEXPAND | wxALL, 0 );


	m_panel_root->SetSizer( bSizer35 );
	m_panel_root->Layout();
	bSizer35->Fit( m_panel_root );
	bSizer14->Add( m_panel_root, 1, wxEXPAND | wxALL, 0 );


	this->SetSizer( bSizer14 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_Main::OnClose ) );
	m_bitmap1->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( Container_Main::OnLeftDown ), NULL, this );
	m_choice1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( Container_Main::OnChoice ), NULL, this );
}

Container_Main::~Container_Main()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_Main::OnClose ) );
	m_bitmap1->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( Container_Main::OnLeftDown ), NULL, this );
	m_choice1->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( Container_Main::OnChoice ), NULL, this );

}

Container_SelectArch::Container_SelectArch( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	m_panel_root = new wxPanel( this, wxID_PANEL_ROOT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel_root->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );

	m_scrolledwindow = new wxScrolledWindow( m_panel_root, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledwindow->SetScrollRate( 5, 5 );
	m_scrolledwindow->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	m_panel71 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel71->SetBackgroundColour( wxColour( 225, 225, 225 ) );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );

	m_title = new wxStaticText( m_panel71, wxID_ANY, wxT("title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_title->Wrap( -1 );
	m_title->SetFont( wxFont( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_title->SetForegroundColour( wxColour( 26, 26, 26 ) );

	bSizer111->Add( m_title, 0, wxLEFT, 12 );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );


	m_panel71->SetSizer( bSizer111 );
	m_panel71->Layout();
	bSizer111->Fit( m_panel71 );
	bSizer20->Add( m_panel71, 0, wxEXPAND | wxALL, 0 );

	m_panel171 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );

	m_customControl61 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl61, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );

	m_customControl611 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl611, 0, wxALL|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );

	m_arch = new wxStaticText( m_panel171, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_arch->Wrap( -1 );
	bSizer191->Add( m_arch, 0, wxLEFT|wxTOP, 12 );


	bSizer191->Add( 0, 0, 1, wxEXPAND, 5 );


	m_panel171->SetSizer( bSizer191 );
	m_panel171->Layout();
	bSizer191->Fit( m_panel171 );
	bSizer20->Add( m_panel171, 1, wxEXPAND | wxALL, 0 );


	m_scrolledwindow->SetSizer( bSizer20 );
	m_scrolledwindow->Layout();
	bSizer20->Fit( m_scrolledwindow );
	bSizer35->Add( m_scrolledwindow, 1, wxEXPAND | wxALL, 0 );

	m_prevnext = new PrevNextPanel( m_panel_root, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_prevnext, 0, wxALL|wxEXPAND, 5 );


	m_panel_root->SetSizer( bSizer35 );
	m_panel_root->Layout();
	bSizer35->Fit( m_panel_root );
	bSizer14->Add( m_panel_root, 1, wxEXPAND | wxALL, 0 );


	this->SetSizer( bSizer14 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_SelectArch::OnClose ) );
}

Container_SelectArch::~Container_SelectArch()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_SelectArch::OnClose ) );

}

Container_SelectMbrGpt::Container_SelectMbrGpt( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	m_panel_root = new wxPanel( this, wxID_PANEL_ROOT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel_root->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );

	m_scrolledwindow = new wxScrolledWindow( m_panel_root, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledwindow->SetScrollRate( 5, 5 );
	m_scrolledwindow->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	m_panel71 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel71->SetBackgroundColour( wxColour( 225, 225, 225 ) );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );

	m_title = new wxStaticText( m_panel71, wxID_ANY, wxT("title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_title->Wrap( -1 );
	m_title->SetFont( wxFont( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_title->SetForegroundColour( wxColour( 26, 26, 26 ) );

	bSizer111->Add( m_title, 0, wxLEFT, 12 );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );


	m_panel71->SetSizer( bSizer111 );
	m_panel71->Layout();
	bSizer111->Fit( m_panel71 );
	bSizer20->Add( m_panel71, 0, wxEXPAND | wxALL, 0 );

	m_panel171 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );

	m_customControl61 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl61, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );

	m_customControl611 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl611, 0, wxALL|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );

	m_mbrgpt = new wxStaticText( m_panel171, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_mbrgpt->Wrap( -1 );
	bSizer191->Add( m_mbrgpt, 0, wxLEFT|wxTOP, 12 );


	bSizer191->Add( 0, 0, 1, wxEXPAND, 5 );


	m_panel171->SetSizer( bSizer191 );
	m_panel171->Layout();
	bSizer191->Fit( m_panel171 );
	bSizer20->Add( m_panel171, 1, wxEXPAND | wxALL, 0 );


	m_scrolledwindow->SetSizer( bSizer20 );
	m_scrolledwindow->Layout();
	bSizer20->Fit( m_scrolledwindow );
	bSizer35->Add( m_scrolledwindow, 1, wxEXPAND | wxALL, 0 );

	m_prevnext = new PrevNextPanel( m_panel_root, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_prevnext, 0, wxALL|wxEXPAND, 5 );


	m_panel_root->SetSizer( bSizer35 );
	m_panel_root->Layout();
	bSizer35->Fit( m_panel_root );
	bSizer14->Add( m_panel_root, 1, wxEXPAND | wxALL, 0 );


	this->SetSizer( bSizer14 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_SelectMbrGpt::OnClose ) );
}

Container_SelectMbrGpt::~Container_SelectMbrGpt()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_SelectMbrGpt::OnClose ) );

}

Container_Complete::Container_Complete( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	m_panel_root = new wxPanel( this, wxID_PANEL_ROOT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel_root->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );

	m_scrolledwindow = new wxScrolledWindow( m_panel_root, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledwindow->SetScrollRate( 5, 5 );
	m_scrolledwindow->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	m_panel71 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel71->SetBackgroundColour( wxColour( 225, 225, 225 ) );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );

	m_title = new wxStaticText( m_panel71, wxID_ANY, wxT("title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_title->Wrap( -1 );
	m_title->SetFont( wxFont( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_title->SetForegroundColour( wxColour( 26, 26, 26 ) );

	bSizer111->Add( m_title, 0, wxLEFT, 12 );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );


	m_panel71->SetSizer( bSizer111 );
	m_panel71->Layout();
	bSizer111->Fit( m_panel71 );
	bSizer20->Add( m_panel71, 0, wxEXPAND | wxALL, 0 );

	m_panel171 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );

	m_customControl61 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl61, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );

	m_customControl611 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl611, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );

	m_customControl612 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl612, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );


	bSizer191->Add( 0, 0, 1, wxEXPAND, 5 );


	m_panel171->SetSizer( bSizer191 );
	m_panel171->Layout();
	bSizer191->Fit( m_panel171 );
	bSizer20->Add( m_panel171, 1, wxEXPAND | wxALL, 0 );


	m_scrolledwindow->SetSizer( bSizer20 );
	m_scrolledwindow->Layout();
	bSizer20->Fit( m_scrolledwindow );
	bSizer35->Add( m_scrolledwindow, 1, wxEXPAND | wxALL, 0 );


	m_panel_root->SetSizer( bSizer35 );
	m_panel_root->Layout();
	bSizer35->Fit( m_panel_root );
	bSizer14->Add( m_panel_root, 1, wxEXPAND | wxALL, 0 );


	this->SetSizer( bSizer14 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_Complete::OnClose ) );
}

Container_Complete::~Container_Complete()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_Complete::OnClose ) );

}

Container_Sample::Container_Sample( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	m_panel_root = new wxPanel( this, wxID_PANEL_ROOT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel_root->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );

	m_scrolledwindow = new wxScrolledWindow( m_panel_root, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledwindow->SetScrollRate( 5, 5 );
	m_scrolledwindow->SetBackgroundColour( wxColour( 245, 245, 245 ) );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	m_panel71 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel71->SetBackgroundColour( wxColour( 225, 225, 225 ) );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );

	m_title = new wxStaticText( m_panel71, wxID_ANY, wxT("title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_title->Wrap( -1 );
	m_title->SetFont( wxFont( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	m_title->SetForegroundColour( wxColour( 26, 26, 26 ) );

	bSizer111->Add( m_title, 0, wxLEFT, 12 );


	bSizer111->Add( 0, 8, 0, wxEXPAND, 5 );


	m_panel71->SetSizer( bSizer111 );
	m_panel71->Layout();
	bSizer111->Fit( m_panel71 );
	bSizer20->Add( m_panel71, 0, wxEXPAND | wxALL, 0 );

	m_panel171 = new wxPanel( m_scrolledwindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );

	m_customControl61 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl61, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );

	m_customControl611 = new SimpleButton( m_panel171, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer191->Add( m_customControl611, 0, wxALL, 5 );


	bSizer191->Add( 0, 0, 1, wxEXPAND, 5 );


	m_panel171->SetSizer( bSizer191 );
	m_panel171->Layout();
	bSizer191->Fit( m_panel171 );
	bSizer20->Add( m_panel171, 1, wxEXPAND | wxALL, 0 );


	m_scrolledwindow->SetSizer( bSizer20 );
	m_scrolledwindow->Layout();
	bSizer20->Fit( m_scrolledwindow );
	bSizer35->Add( m_scrolledwindow, 1, wxEXPAND | wxALL, 0 );

	m_prevnext = new PrevNextPanel( m_panel_root, wxID_RESTORE_SELECTFILE, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_prevnext, 0, wxALL|wxEXPAND, 5 );


	m_panel_root->SetSizer( bSizer35 );
	m_panel_root->Layout();
	bSizer35->Fit( m_panel_root );
	bSizer14->Add( m_panel_root, 1, wxEXPAND | wxALL, 0 );


	this->SetSizer( bSizer14 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_Sample::OnClose ) );
}

Container_Sample::~Container_Sample()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Container_Sample::OnClose ) );

}
