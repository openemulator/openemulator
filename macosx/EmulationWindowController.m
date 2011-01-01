
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
#import "EmulationOutlineCell.h"
#import "VerticallyCenteredTextFieldCell.h"
#import "StringConversion.h"

#define SPLIT_VERT_MIN 128
#define SPLIT_VERT_MAX 256

@implementation EmulationWindowController

- (id)init
{
	if (self = [self initWithWindowNibName:@"Emulation"])
	{
		checkBoxCell = [[NSButtonCell alloc] initTextCell:@""];
		[checkBoxCell setControlSize:NSSmallControlSize];
		[checkBoxCell setFont:[NSFont labelFontOfSize:[NSFont smallSystemFontSize]]];
		[checkBoxCell setButtonType:NSSwitchButton];
		
		popUpButtonCell = [[NSPopUpButtonCell alloc] initTextCell:@""];
		[popUpButtonCell setControlSize:NSSmallControlSize];
		[popUpButtonCell setFont:[NSFont labelFontOfSize:[NSFont smallSystemFontSize]]];
		[popUpButtonCell setBordered:NO];
		
		sliderCell = [[NSSliderCell alloc] initTextCell:@""];
		[sliderCell setControlSize:NSSmallControlSize];
		[sliderCell	setNumberOfTickMarks:3];
	}
	
	return self;
}

- (void)dealloc
{
	[rootItem release];
	
	[checkBoxCell release];
	[popUpButtonCell release];
	[sliderCell release];
	
	[super dealloc];
}

- (void)updateDetail
{
	NSString *title = @"No Selection";
	NSImage *image = nil;
	NSString *locationLabel = @"";
	NSString *stateLabel = @"";
	BOOL showable = NO;
	BOOL mountable = NO;
	BOOL mounted = NO;
	
	if (selectedItem)
	{
		title = [selectedItem label];
		image = [selectedItem image];
		locationLabel = [selectedItem location];
		if (![locationLabel length])
			locationLabel = NSLocalizedString(@"Built-in",
											  @"Location Label");
		stateLabel = NSLocalizedString([selectedItem state],
									   @"State Label");
		showable = [selectedItem showable];
		mountable = [selectedItem mountable];
		mounted = [selectedItem mounted];
	}
	
	[fDeviceBox setTitle:title];
	[fDeviceImage setImage:image];
	[fDeviceLocationLabel setStringValue:locationLabel];
	[fDeviceStateLabel setStringValue:stateLabel];
	[fDeviceButton setHidden:!(showable || mountable || mounted)];
	if (showable)
		[fDeviceButton setTitle:NSLocalizedString(@"Show Device",
												  @"Emulation Button Label")];
	else if (mountable)
		[fDeviceButton setTitle:NSLocalizedString(@"Open...",
												  @"Emulation Button Label")];
	else if (mounted)
		[fDeviceButton setTitle:NSLocalizedString(@"Unmount",
												  @"Emulation Button Label")];
	
	[fTableView reloadData];
}

- (void)updateSidebar
{
	[rootItem release];
	rootItem = [[EmulationItem alloc] initWithDocument:[self document]];
	
	[fOutlineView reloadData];
}

- (void)updateWindowPosition
{
	BOOL isVisible = [[self window] isVisible];
	NSString *frameString = [[self window] stringWithSavedFrame];
	
	frameString = [frameString stringByAppendingFormat:@"%d", isVisible];
	
	[[self document] setEDLOptions:frameString];
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
	
	[self updateSidebar];
	
	[fOutlineView setDelegate:self];
	[fOutlineView setDataSource:self];
	[fOutlineView registerForDraggedTypes:[NSArray arrayWithObjects:
										   NSFilenamesPboardType,
										   nil]];
	[fOutlineView expandItem:nil expandChildren:YES];
	
	[fTableView setDelegate:self];
	[fTableView setDataSource:self];
	
	[fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:1]
			  byExtendingSelection:NO];
	
	// Parse window frame
	NSString *frameString = [[self document] getEDLOptions];
	NSArray *components = [frameString componentsSeparatedByString:@" "];
	if ([components count] == 9)
	{
		NSString *theString = [NSString string];
		for (NSInteger i = 0; i < 8; i++)
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



- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
	 itemForItemIdentifier:(NSString *)ident
 willBeInsertedIntoToolbar:(BOOL)flag
{
	NSToolbarItem *item;
	item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
	if (!item)
		return nil;
	
	[item autorelease];
	if ([ident isEqualToString:@"AudioControls"])
	{
		[item setLabel:NSLocalizedString(@"Audio Controls",
										 "Canvas window toolbar item")];
		[item setPaletteLabel:NSLocalizedString(@"Audio Controls",
												"Canvas window toolbar item")];
		[item setToolTip:NSLocalizedString(@"Show or hide audio controls.",
										   "Canvas window toolbar item")];
		[item setImage:[NSImage imageNamed:@"IconAudio.png"]];
		[item setAction:@selector(toggleAudioControls:)];
	}
	
	return item;
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
			NSToolbarFlexibleSpaceItemIdentifier,
			@"AudioControls",
			nil];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
			@"AudioControls",
			NSToolbarSeparatorItemIdentifier,
			NSToolbarSpaceItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
			nil];
}



- (void)splitView:(NSSplitView *)sender
resizeSubviewsWithOldSize:(NSSize)oldSize
{
	NSArray *subviews = [sender subviews];
	
	NSSize newSize = [sender frame].size;
	float deltaWidth = newSize.width - oldSize.width;
	float deltaHeight = newSize.height - oldSize.height;
	
	for (NSInteger i = 0; i < [subviews count]; i++)
	{
		NSView *subview = [subviews objectAtIndex:i];
		NSRect frame = subview.frame;
		
		if ([sender isVertical])
		{
			frame.size.height += deltaHeight;
			if (i == 1)
				frame.size.width += deltaWidth;
		}
		
		[subview setFrame:frame];
	}
}

- (CGFloat)splitView:(NSSplitView *)splitView
constrainMinCoordinate:(CGFloat)proposedMin
		 ofSubviewAt:(NSInteger)dividerIndex
{
	return SPLIT_VERT_MIN;
}

- (CGFloat)splitView:(NSSplitView *)splitView
constrainMaxCoordinate:(CGFloat)proposedMax
		 ofSubviewAt:(NSInteger)dividerIndex
{
	return SPLIT_VERT_MAX;
}



- (NSInteger)outlineView:(NSOutlineView *)outlineView
  numberOfChildrenOfItem:(id)item
{
	if (!item)
		item = rootItem;
	
	return [[item children] count];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
   isItemExpandable:(id)item
{
	if (!item)
		item = rootItem;
	
	return ([[item children] count] > 0);
}

- (id)outlineView:(NSOutlineView *)outlineView
			child:(NSInteger)index 
		   ofItem:(id)item
{
	if (!item)
		item = rootItem;
	
	return [[item children] objectAtIndex:index];
}

- (id)outlineView:(NSOutlineView *)outlineView
objectValueForTableColumn:(NSTableColumn *)tableColumn
		   byItem:(id)item
{
	return [item label];
}

- (NSDragOperation)outlineView:(NSOutlineView *)outlineView
				  validateDrop:(id < NSDraggingInfo >)info
				  proposedItem:(id)item
			proposedChildIndex:(NSInteger)index
{
	// To-Do: validate file type
	return [outlineView parentForItem:item] ? NSDragOperationCopy : NSDragOperationNone;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
		 acceptDrop:(id < NSDraggingInfo >)info
			   item:(id)item
		 childIndex:(NSInteger)index
{
	return YES;
}



- (void)outlineView:(NSOutlineView *)outlineView
	willDisplayCell:(id)cell
	 forTableColumn:(NSTableColumn *)tableColumn
			   item:(id)item
{
	[cell setRepresentedObject:item];
	
	NSInteger rowIndex = [fOutlineView rowForItem:item];
	[cell setButtonRollover:(rowIndex == [fOutlineView trackedRow])];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
		isGroupItem:(id)item
{
	return ![outlineView parentForItem:item];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
   shouldSelectItem:(id)item
{
	return ([outlineView parentForItem:item] != nil);
}

- (void)outlineViewSelectionIsChanging:(NSNotification *)notification
{
	if ([fOutlineView forcedRow] != -1)
	{
		NSInteger row = [fOutlineView forcedRow];
		[fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:row]
				  byExtendingSelection:NO];
	}
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	NSInteger row = [fOutlineView selectedRow];
	selectedItem = [fOutlineView itemAtRow:row];
	
	[fRemoveDevice setEnabled:selectedItem ? [selectedItem removable] : NO];
	
	[self updateDetail];
}



- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	if (selectedItem)
		return [selectedItem numberOfSettings];
	
	return 0;
}

- (id)tableView:(NSTableView *)aTableView
objectValueForTableColumn:(NSTableColumn *)aTableColumn
			row:(NSInteger)rowIndex
{
	if (selectedItem)
	{
		if (aTableColumn == fTableKeyColumn)
			return [selectedItem labelForSettingAtIndex:rowIndex];
		else if (aTableColumn == fTableValueColumn)
			return [selectedItem valueForSettingAtIndex:rowIndex];
	}
	
	return @"";
}

- (NSCell *)tableView:(NSTableView *)tableView
dataCellForTableColumn:(NSTableColumn *)tableColumn
				  row:(NSInteger)row
{
	if (selectedItem)
	{
		if (tableColumn == fTableValueColumn)
		{
			NSString *type = [selectedItem typeForSettingAtIndex:row];
			if ([type compare:@"checkbox"] == NSOrderedSame)
				return checkBoxCell;
			else if ([type compare:@"select"] == NSOrderedSame)
			{
				[popUpButtonCell removeAllItems];
				
				NSArray *options = [selectedItem optionsForSettingAtIndex:row];
				[popUpButtonCell addItemsWithTitles:options];
				
				return popUpButtonCell;
			}
			else if ([type compare:@"slider"] == NSOrderedSame)
			{
				NSArray *options = [selectedItem optionsForSettingAtIndex:row];
				[sliderCell setMinValue:[[options objectAtIndex:0] floatValue]];
				[sliderCell setMaxValue:[[options objectAtIndex:1] floatValue]];
				
				return sliderCell;
			}
		}
	}
	
	return nil;
}



- (void)emulationDoubleAction:(id)sender
{
	// Get selected device
	// If storage is present and disk is not mounted, mountDevice()
	// else if storage is present and disk is mounted, revealInFinder()
	// else if canvas is present, showDevice()
}

- (void)addEDL:(id)sender
{
	// Start hardware chooser
}

- (void)removeDevice:(id)sender
{
	// Remove device
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
