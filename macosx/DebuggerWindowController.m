
/**
 * OpenEmulator
 * Mac OS X Debugger Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the debugger window
 */

#import "DebuggerWindowController.h"

@implementation DebuggerWindowController

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
