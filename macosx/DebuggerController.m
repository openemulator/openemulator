
/**
 * OpenEmulator
 * Mac OS X Debuger Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the debugger console
 */

#import "DebuggerController.h"

@implementation DebuggerController

- (id)init
{
	self = [super initWithWindowNibName:@"Debugger"];
	
	return self;
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Debugger"];
}

@end
