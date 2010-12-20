
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
	if (self = [self initWithWindowNibName:@"Emulation"])
	{
		emulationTableCell = [[EmulationTableCell alloc] init];
	}
	
	return self;
}

- (void)dealloc
{
	[emulationTableCell release];
	
	[super dealloc];
}

- (BOOL)respondsToSelector:(SEL)aSelector
{
	// To-Do: Forward emulation messages accordingly
	
	return [super respondsToSelector:aSelector];
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
	
	[fEmulationTableView setDataSource:self];
	[fEmulationTableView setDelegate:self];
	[fEmulationTableView setDoubleAction:@selector(emulationDoubleAction:)];
	[fEmulationTableView registerForDraggedTypes:[NSArray arrayWithObjects:
												  NSFilenamesPboardType,
												  nil]];
	
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

- (NSCell *)tableView:(NSTableView *)tableView
dataCellForTableColumn:(NSTableColumn *)tableColumn
				  row:(NSInteger)row
{
	return emulationTableCell;
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	OEDevicesInfo *devicesInfo = (OEDevicesInfo *)[[self document] devicesInfo];
	
	if (devicesInfo)
		return (devicesInfo)->size();
	
	return 0;
}

- (id)tableView:(NSTableView *)tableView
objectValueForTableColumn:(NSTableColumn *)tableColumn
			row:(NSInteger)rowIndex
{	
	OEDevicesInfo *devicesInfo = (OEDevicesInfo *)[[self document] devicesInfo];
	
	if (devicesInfo && (rowIndex < devicesInfo->size()))
		return getNSString((*devicesInfo)[rowIndex].id);
	
	return nil;
}

- (NSDragOperation)tableView:(NSTableView *)aTableView
				validateDrop:(id < NSDraggingInfo >)info
				 proposedRow:(NSInteger)row
	   proposedDropOperation:(NSTableViewDropOperation)operation
{
	if (operation == NSTableViewDropOn)
		return NSDragOperationCopy;
	
	return NSDragOperationNone;
}

- (BOOL)tableView:(NSTableView *)aTableView
	   acceptDrop:(id < NSDraggingInfo >)info
			  row:(NSInteger)row
	dropOperation:(NSTableViewDropOperation)operation
{
	return YES;
}

- (void)emulationDoubleAction:(id)sender
{
	// Get selected device
	// If storage is present and disk is not mounted, call mountDevice()
	// else if storage is present and disk is mounted, call revealInFinder()
	// else if canvas is present, call showDevice()
	// else open inspector
}

- (void)addEDL:(id)sender
{
	// Start hardware chooser
}

- (void)removeDevice:(id)sender
{
	// Remove device
}

- (void)systemPowerDown:(id)sender
{
	// Get selected device
	// Forward to all canvases
}

- (void)systemSleep:(id)sender
{
}

- (void)systemWakeUp:(id)sender
{
}

- (void)systemColdRestart:(id)sender
{
}

- (void)systemWarmRestart:(id)sender
{
}

- (void)systemDebuggerBreak:(id)sender
{
}

- (void)showDevice:(id)sender
{
	// Get selected device
	// Show all associated windows
}

- (void)revealInFinder:(id)sender
{
	// Get selected device
	// Get path to mounted disk image
	// Open Finder at path
}

- (void)mountDevice:(id)sender
{
	// Get selected device
	// Open disk image open dialog
	// Attempt to mount disk image in device's storage components
}

- (void)ejectDevice:(id)sender
{
	// Get selected device
	// Attempt to eject all device's storage components
}

@end
