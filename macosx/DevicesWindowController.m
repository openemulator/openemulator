
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
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Devices Toolbar"];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	
	[[self window] setToolbar:toolbar];
	
	[toolbar release];
}

- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName
{
	return [displayName stringByAppendingString:@" - Devices"];
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
	if ([ident isEqualToString:@"Inspector"])
	{
		[item setLabel:NSLocalizedString(@"Inspector",
										 "Device window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Inspector",
												"Device window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Show or hide the inspector window.",
										   "Device window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconInspector.png"]];
		[item setTarget:nil];
		[item setAction:@selector(toggleInspector:)];
	}
	
	return item;
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Inspector",
			nil];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
			@"Inspector",
			NSToolbarSeparatorItemIdentifier,
			NSToolbarSpaceItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
			nil];
}

- (void)showDevices:(id)sender
{
	[self showWindow:sender];
}

@end
