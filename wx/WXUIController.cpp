
/**
 * AppleIIGo
 * wxWidgets UI Controller
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "WXUIController.h"

WXUIController::WXUIController() : wxFrame((wxFrame *) NULL,
										   wxID_ANY,
										   _T("AppleIIGo"),
										   wxPoint(250,250),
										   wxSize(200,200))
{
	wxMenu * fileMenu = new wxMenu();
	
//	fileMenu->Append(_T("&New"));
	
	wxMenuBar * menuBar = new wxMenuBar(wxMB_DOCKABLE);
	
	wxMenu * editMenu = new wxMenu();
	wxMenu * viewMenu = new wxMenu();
	wxMenu * windowMenu = new wxMenu();
	wxMenu * helpMenu = new wxMenu();
	
	menuBar->Append(fileMenu, _T("&File"));
	menuBar->Append(editMenu, _T("&Edit"));
	menuBar->Append(viewMenu, _T("&View"));
	menuBar->Append(windowMenu, _T("&Window"));
	menuBar->Append(helpMenu, _T("&Help"));
	
	SetMenuBar(menuBar);
}

WXUIController::~WXUIController()
{
}
