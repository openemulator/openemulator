
/**
 * OpenEmulator
 * Mac OS X Application
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Intercepts app events.
 */

#import "Application.h"

@implementation Application

- (void)sendEvent:(NSEvent *)theEvent
{
	if (([theEvent type] == NSKeyDown) ||
		([theEvent type] == NSKeyUp))
	{
		// If keyboard is captured, send message directly to active window
	}
	
	[super sendEvent:theEvent];	
}

@end
