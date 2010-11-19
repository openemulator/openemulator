
/**
 * OpenEmulator
 * Mac OS X Canvas Window Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas window.
 */

#import "CanvasWindowController.h"

@implementation CanvasWindowController

- (id)initWithCanvasComponent:(void *)theCanvasComponent
{
	if (self = [self initWithWindowNibName:@"Canvas"])
		canvasComponent = canvasComponent;
	
	return self;
}

- (void *)canvasComponent
{
	return canvasComponent;
}

- (void)windowDidLoad
{
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Canvas Toolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	
	[[self window] setToolbar:toolbar];
	
	[[self window] registerForDraggedTypes:[NSArray arrayWithObjects:
											NSFilenamesPboardType, nil]];
	
	[toolbar release];
}

- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName
{
	return [displayName stringByAppendingString:@" - Monitor"];
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
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Power Down",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Initiate power-down.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconPowerDown.png"]];
		[item setAction:@selector(systemPowerDown:)];
	}
	else if ([ident isEqualToString:@"Sleep"])
	{
		[item setLabel:NSLocalizedString(@"Sleep",
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Sleep",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Initiate low power mode.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconSleep.png"]];
		[item setAction:@selector(systemSleep:)];
	}
	else if ([ident isEqualToString:@"Wake Up"])
	{
		[item setLabel:NSLocalizedString(@"Wake Up",
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Wake Up",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Initiate full power state.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconWakeUp.png"]];
		[item setAction:@selector(systemWakeUp:)];
	}
	else if ([ident isEqualToString:@"Cold Restart"])
	{
		[item setLabel:NSLocalizedString(@"Cold Restart",
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Cold Restart",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Restart the system at the most "
										   "primitive level.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconColdRestart.png"]];
		[item setAction:@selector(systemColdRestart:)];
	}
	else if ([ident isEqualToString:@"Warm Restart"])
	{
		[item setLabel:NSLocalizedString(@"Warm Restart",
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Warm Restart",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Restart the operating system.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconWarmRestart.png"]];
		[item setAction:@selector(systemWarmRestart:)];
	}
	else if ([ident isEqualToString:@"Debugger Break"])
	{
		[item setLabel:NSLocalizedString(@"Debugger Break",
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Debugger Break",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Break into the operating system debugger.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconDebuggerBreak.png"]];
		[item setAction:@selector(systemDebuggerBreak:)];
	}
	else if ([ident isEqualToString:@"Inspector"])
	{
		[item setLabel:NSLocalizedString(@"Inspector",
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Info",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Show or hide the inspector window.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconInspector.png"]];
		//		[item setTarget:self];
		[item setAction:@selector(toggleInspector:)];
	}
	else if ([ident isEqualToString:@"Audio"])
	{
		[item setLabel:NSLocalizedString(@"Audio Controls",
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Audio Controls",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Show or hide audio controls.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconAudio.png"]];
		//		[item setTarget:self];
		[item setAction:@selector(toggleAudioControls:)];
	}
	else if ([ident isEqualToString:@"Devices"])
	{
		[item setLabel:NSLocalizedString(@"Devices",
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Devices",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Show devices.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconDevices.png"]];
		//		[item setTarget:self];
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
			@"Devices",
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
			@"Inspector",
			@"Audio",
			@"Devices",
			NSToolbarSeparatorItemIdentifier,
			NSToolbarSpaceItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
			nil];
}

@end
