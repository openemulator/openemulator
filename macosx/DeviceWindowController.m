
/**
 * OpenEmulator
 * Mac OS X Device Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device window.
 */

#import "DeviceWindowController.h"

@implementation DeviceWindowController

- (id)init
{
	self = [self initWithWindowNibName:@"Device"];
	
	return self;
}

- (void)windowDidLoad
{
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Document Toolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	[[self window] setToolbar:toolbar];
	[toolbar release];
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
	 itemForItemIdentifier:(NSString *)ident
 willBeInsertedIntoToolbar:(BOOL)flag
{
	NSToolbarItem *item;
	item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
	if (!item)
		return nil;
	
	[item autorelease];
	if ([ident isEqualToString:@"Power Down"])
	{
		[item setLabel:NSLocalizedString(@"Power Down",
										 "Device window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Power Down",
												"Device window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Initiate power-down.",
										   "Device window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconPowerDown.png"]];
		[item setAction:@selector(powerDown:)];
	}
	else if ([ident isEqualToString:@"Sleep"])
	{
		[item setLabel:NSLocalizedString(@"Sleep",
										 "Device window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Sleep",
												"Device window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Initiate low power mode.",
										   "Device window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconSleep.png"]];
		[item setAction:@selector(sleep:)];
	}
	else if ([ident isEqualToString:@"Wake Up"])
	{
		[item setLabel:NSLocalizedString(@"Wake Up",
										 "Device window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Wake Up",
												"Device window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Initiate full power state.",
										   "Device window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconWakeUp.png"]];
		[item setAction:@selector(wakeUp:)];
	}
	else if ([ident isEqualToString:@"Cold Restart"])
	{
		[item setLabel:NSLocalizedString(@"Cold Restart",
										 "Device window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Cold Restart",
												"Device window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Restart the system at the most "
										   "primitive level.",
										   "Device window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconColdRestart.png"]];
		[item setAction:@selector(coldRestart:)];
	}
	else if ([ident isEqualToString:@"Warm Restart"])
	{
		[item setLabel:NSLocalizedString(@"Warm Restart",
										 "Device window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Warm Restart",
												"Device window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Restart the operating system.",
										   "Device window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconWarmRestart.png"]];
		[item setAction:@selector(warmRestart:)];
	}
	else if ([ident isEqualToString:@"Debugger Break"])
	{
		[item setLabel:NSLocalizedString(@"Debugger Break",
										 "Device window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Debugger Break",
												"Device window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Break into the operating system debugger.",
										   "Device window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconDebuggerBreak.png"]];
		[item setAction:@selector(debuggerBreak:)];
	}
	else if ([ident isEqualToString:@"Devices"])
	{
		[item setLabel:NSLocalizedString(@"Devices",
										 "Device window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Devices",
												"Device window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Show devices.",
										   "Device window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconDevices.png"]];
		[item setTarget:self];
		[item setAction:@selector(showDevices:)];
	}
	
	return item;
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
			@"Power Down",
			@"Sleep",
			@"Wake Up",
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Info",
			nil];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
			@"Power Down",
			@"Sleep",
			@"Wake Up",
			@"Cold Restart",
			@"Warm Restart",
			@"Debugger Break",
			@"Info",
			NSToolbarSeparatorItemIdentifier,
			NSToolbarSpaceItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
			nil];
}

- (void)showDevices:(id)sender
{
	[[NSNotificationCenter defaultCenter] postNotificationName:@"showDevices"
														object:self];
}

@end
