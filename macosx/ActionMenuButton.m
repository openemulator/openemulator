
/**
 * OpenEmulator
 * Mac OS X Action Menu Butotn
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an action menu button.
 */

#import "ActionMenuButton.h"

@implementation ActionMenuButton

- (void)mouseDown:(NSEvent *)theEvent
{
	if (![self menu])
		[super mouseDown:theEvent];
	else
	{
		if ([self isEnabled] &&
			NSPointInRect([self convertPoint:[theEvent locationInWindow] fromView:nil],
						  [self bounds]))
		{
			[self highlight:YES];
			
			NSPoint point = [self convertPoint:[self bounds].origin toView:nil];
			point.y -= NSHeight([self frame]) + 2;
			point.x += 3;
			
			NSEvent *event = [NSEvent mouseEventWithType:[theEvent type]
												location:point
										   modifierFlags:[theEvent modifierFlags]
											   timestamp:[theEvent timestamp]
											windowNumber:[[theEvent window] windowNumber]
												 context:[theEvent context]
											 eventNumber:[theEvent eventNumber]
											  clickCount:[theEvent clickCount]
												pressure:[theEvent pressure]];
			[NSMenu popUpContextMenu:[self menu] withEvent:event forView:self];
			
			[self mouseUp:[[NSApplication sharedApplication] currentEvent]];
		}
	}
}

- (void)mouseUp:(NSEvent *)theEvent
{
	[self highlight:NO];
	
	[super mouseUp:theEvent];
}

@end
