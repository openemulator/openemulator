
/**
 * OpenEmulator
 * Mac OS X Tablet Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a tablet input device.
 */

#import "TabletController.h"


@implementation TabletController

- (id)init
{
	self = [super initWithWindowNibName:@"Tablet"];
	
	return self;
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Tablet"];
}

@end
