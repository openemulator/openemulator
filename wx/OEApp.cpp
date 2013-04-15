/**
 * OpenEmulator wxWidgets GUI
 * Main application
 * (C) 2013 by Tobias Eriksson (tobier@tobier.se)
 * Released under the GPL
 *
 * Implements the main application class
 */

#include "OEApp.h"

IMPLEMENT_APP(OEApp)

bool OEApp::OnInit()
{
  wxFrame *frame = new wxFrame((wxFrame*) NULL, -1, _T("Hello wxWidgets World"));
  frame->CreateStatusBar();
  frame->SetStatusText(_T("Hello World"));
  frame->Show(true);
  SetTopWindow(frame);
  return true;
}
