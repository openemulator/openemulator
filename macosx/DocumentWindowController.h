
/**
 * OpenEmulator
 * Mac OS X Document Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation's window.
 */

#import <Cocoa/Cocoa.h>

#import "Document.h"

@interface DocumentWindowController : NSWindowController
{
	Document *document;
	
	BOOL fullscreen;
	NSRect fullscreenExitRect;
}

@end
