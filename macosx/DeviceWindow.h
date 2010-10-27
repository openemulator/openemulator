
/**
 * OpenEmulator
 * Mac OS X Device Window
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles device window messages.
 */

#import <Cocoa/Cocoa.h>

@interface DeviceWindow : NSWindow
{
	BOOL fullscreen;
	NSRect windowRect;
}

- (void) toggleFullscreen:(id) sender;
- (void) setHalfSize:(id) sender;
- (void) setActualSize:(id) sender;
- (void) setDoubleSize:(id) sender;
- (void) fitToScreen:(id) sender;

@end
