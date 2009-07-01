
/**
 * OpenEmulator
 * Mac OS X Document Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation's window.
 */

#import <Cocoa/Cocoa.h>

#import "DocumentWindow.h"

@interface DocumentWindowController : NSWindowController {
	BOOL isFullscreen;
	DocumentWindow *fullscreenWindow;
	NSView *fullscreenView;
}

- (void)saveDocumentAsTemplate:(id)sender;
- (void)toggleInspectorPanel:(id)sender;
- (void)setHalfSize:(id)sender;
- (void)setActualSize:(id)sender;
- (void)setDoubleSize:(id)sender;
- (void)fitToScreen:(id)sender;
- (void)toggleFullscreen:(id)sender;

@end
