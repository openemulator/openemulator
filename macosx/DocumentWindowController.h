
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
#import "DocumentController.h"

@interface DocumentWindowController : NSWindowController
{
	IBOutlet id fDocumentView;
	
	Document *document;
	DocumentController *documentController;
	
	BOOL fullscreen;
	id fullscreenWindow;
	NSView *fullscreenView;
}

- (void)setHalfSize:(id)sender;
- (void)setActualSize:(id)sender;
- (void)setDoubleSize:(id)sender;
- (void)fitToScreen:(id)sender;
- (void)toggleFullscreen:(id)sender;

@end
