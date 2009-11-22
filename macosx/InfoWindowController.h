
/**
 * OpenEmulator
 * Mac OS X Info Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation's info window.
 */

#import <Cocoa/Cocoa.h>

#import "Document.h"
#import "DocumentController.h"

@interface InfoWindowController : NSWindowController
{
	IBOutlet id fInfoView;
	
	Document *document;
	DocumentController *documentController;
}

@end
