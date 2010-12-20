
/**
 * OpenEmulator
 * Mac OS X Canvas Window
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles canvas window messages.
 */

#import <Cocoa/Cocoa.h>

@interface CanvasWindow : NSWindow
{
	BOOL fullscreen;
	NSRect windowRect;
}

- (void *)canvasComponent;

- (void)toggleFullscreen:(id)sender;
- (void)setHalfSize:(id)sender;
- (void)setActualSize:(id)sender;
- (void)setDoubleSize:(id)sender;
- (void)fitToScreen:(id)sender;

@end
