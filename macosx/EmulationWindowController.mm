
/**
 * OpenEmulator
 * Mac OS X Emulation Window Controller
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation window.
 */

#import "EmulationWindowController.h"
#import "Document.h"
#import "StringConversion.h"

#import "OEEmulation.h"

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
	
	NSString *frameString = [[self document] getEDLOptions];
	NSArray *components = [frameString componentsSeparatedByString:@" "];
	if ([components count] == 9)
	{
		NSString *theString = [NSString string];
		for (int i = 0; i < 8; i++)
		{
			theString = [theString stringByAppendingString:[components objectAtIndex:i]];
			theString = [theString stringByAppendingString:@" "];
		}
		
		NSString *isVisible = [components objectAtIndex:8];
		
		[[self window] setFrameFromString:theString];
		if ([isVisible compare:@"0"] == NSOrderedSame)
			[[self window] orderOut:self];
	}
}

- (void)updateOptions
{
	int isVisible = [[self window] isVisible];
	NSString *frameString = [[self window] stringWithSavedFrame];
	
	frameString = [frameString stringByAppendingFormat:@"%d", isVisible];
	
	[[self document] setEDLOptions:frameString];
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
	else if ([ident isEqualToString:@"AudioControls"])
	{
		[item setLabel:NSLocalizedString(@"Audio Controls",
										 "Emulation window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Audio Controls",
												"Emulation window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Show or hide the audio controls window.",
										   "Emulation window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconAudio.png"]];
		[item setTarget:nil];
		[item setAction:@selector(toggleAudioControls:)];
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
			@"AudioControls",
			NSToolbarSeparatorItemIdentifier,
			NSToolbarSpaceItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
			nil];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	OEDevicesInfo *devicesInfo = (OEDevicesInfo *)[[self document] devicesInfo];
	
	return (devicesInfo)->size();
}

- (id)tableView:(NSTableView *)tableView
objectValueForTableColumn:(NSTableColumn *)tableColumn
			row:(NSInteger)rowIndex
{	
	OEDevicesInfo *devicesInfo = (OEDevicesInfo *)[[self document] devicesInfo];
	
	if (rowIndex < devicesInfo->size())
		return getNSString((*devicesInfo)[rowIndex].label);
	
	return nil;
}

@end
