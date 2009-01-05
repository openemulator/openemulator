
/**
 * AppleIIGo
 * wxWidgets main
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "wx/wx.h"

class WXApp : public wxApp
	{
	private:
		bool OnInit();
		
		wxFrame *frame;
	};