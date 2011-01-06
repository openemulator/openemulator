
/**
 * OpenEmulator
 * Mac OS X Library Window Controller
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the library window.
 */

#import "LibraryWindowController.h"

@implementation LibraryWindowController

- (id)init
{
	self = [super initWithWindowNibName:@"Library"];
	return self;
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Library"];
}

@end
