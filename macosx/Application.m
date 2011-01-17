
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

- (void)setCapture:(BOOL)value
{
	capture = value;
}

- (void)sendEvent:(NSEvent *)theEvent
{
	if (([theEvent type] == NSKeyDown) ||
		([theEvent type] == NSKeyUp))
	{
		// If HELP key was pressed, or capture is active
		// send event directly to key window
		if (([theEvent keyCode] == 0x72) || capture)
		{
			[[NSApp keyWindow] sendEvent:theEvent];
			
			return;
		}
	}
	
	[super sendEvent:theEvent];	
}

@end
