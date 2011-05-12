
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

- (void)toggleFullscreen:(id)sender;
- (void)setFrameScale:(float)scale;
- (void)setHalfSize:(id)sender;
- (void)setActualSize:(id)sender;
- (void)setDoubleSize:(id)sender;
- (void)fitToScreen:(id)sender;

@end
