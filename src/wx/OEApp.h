/**
 * OpenEmulator wxWidgets GUI
 * Main application
 * (C) 2013 by Tobias Eriksson (tobier@tobier.se)
 * Released under the GPL
 *
 * Implements the main application class
 */

#ifndef _OEAPP_H_
#define _OEAPP_H_

#include <wx/wx.h>

class OEApp : public wxApp
{
public:
  virtual bool OnInit();
};

DECLARE_APP(OEApp)

#endif
