
/**
 * OpenEmulator
 * Mac OS X Inspector Window Controller
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the inspector window.
 */

#import "InspectorWindowController.h"

@implementation InspectorWindowController

- (id)init
{
	self = [self initWithWindowNibName:@"Inspector"];
	
	return self;
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Inspector"];
}

@end
