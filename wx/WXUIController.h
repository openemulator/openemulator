
/**
 * AppleIIGo
 * wxWidgets UI Controller
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "wx/wx.h"

#if !wxUSE_MENUS
#error "menu sample requires wxUSE_MENUS=1"
#endif

class WXUIController : public wxFrame
	{
	public:
		WXUIController();
		virtual ~WXUIController();
		
		void newDocument();
		void openDocument();
		void openRecentDocument();
		void ejectDocument();
		void saveDocumentAs();
		void print();
		void copy();
		void paste();
		void startSpeaking();
		void stopSpeaking();
		void setFullscreen();
		void setCharacterSet();
		void pause();
		void mute();
		void reset();
		void powerOff();
	};
