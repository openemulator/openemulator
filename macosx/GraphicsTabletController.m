
/**
 * OpenEmulator
 * Mac OS X Graphics Tablet Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple Graphics Tablet.
 */

#import "GraphicsTabletController.h"


@implementation GraphicsTabletController

- (id)init
{
	return [super initWithWindowNibName:@"GraphicsTablet"];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	NSPanel *panel = (NSPanel *)[self window];
	[panel setFrameUsingName:@"GraphicsTablet"];
	[panel setFrameAutosaveName:@"GraphicsTablet"];
}

@end
