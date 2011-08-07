
/**
 * OpenEmulator
 * Mac OS X Canvas Window
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles canvas window messages.
 */

#import <Cocoa/Cocoa.h>

#import "CanvasView.h"

@interface CanvasWindow : NSWindow
{
	IBOutlet CanvasView *fCanvasView;
	
	BOOL fullscreen;
	NSRect windowRect;
}

- (BOOL)isFullscreen;
- (IBAction)toggleFullscreen:(id)sender;
- (void)leaveFullscreen;

@end
