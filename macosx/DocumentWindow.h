
/**
 * OpenEmulator
 * Mac OS X Document Window
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Overrides document window messages.
 */

#import <Cocoa/Cocoa.h>

@interface DocumentWindow : NSWindow
{
	BOOL fullscreen;
	NSRect fullscreenExitRect;
}

- (void)setHalfSize:(id)sender;
- (void)setActualSize:(id)sender;
- (void)setDoubleSize:(id)sender;
- (void)fitToScreen:(id)sender;
- (void)toggleFullscreen:(id)sender;

@end
