
/**
 * OpenEmulator
 * Mac OS X Emulation Window Controller
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation window.
 */

#import "EmulationWindowController.h"

@implementation EmulationWindowController

- (id)init
{
	self = [self initWithWindowNibName:@"Emulation"];
	
	return self;
}

- (void)windowDidLoad
{
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Emulation Toolbar"];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
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
	if ([ident isEqualToString:@"Inspector"])
	{
		[item setLabel:NSLocalizedString(@"Inspector",
										 "Emulation window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Inspector",
												"Emulation window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Show or hide the inspector window.",
										   "Emulation window toolbar item")];
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

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return 4;
}

- (id)tableView:(NSTableView *)tableView
objectValueForTableColumn:(NSTableColumn *)tableColumn
			row:(NSInteger)rowIndex
{
	return nil;
}

@end
