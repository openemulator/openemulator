
/**
 * OpenEmulator
 * Mac OS X Devices Window Controller
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a devices window.
 */

#import "DevicesWindowController.h"

@implementation DevicesWindowController

- (id)init
{
	self = [self initWithWindowNibName:@"Devices"];
	
	return self;
}

- (void)windowDidLoad
{
/*	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Devices Toolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	[[self window] setToolbar:toolbar];
	[toolbar release];*/
}

@end
