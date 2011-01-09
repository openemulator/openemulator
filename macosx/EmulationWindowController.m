
/**
 * OpenEmulator
 * Mac OS X Emulation Window Controller
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation window.
 */

#import "EmulationWindowController.h"
#import "EmulationItem.h"
#import "EmulationOutlineView.h"
#import "EmulationOutlineCell.h"
#import "VerticallyCenteredTextFieldCell.h"
#import "DocumentController.h"
#import "StringConversion.h"

#define SPLIT_VERT_MIN 128
#define SPLIT_VERT_MAX 256

@implementation EmulationWindowController

- (id)init
{
	if (self = [super initWithWindowNibName:@"Emulation"])
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
	
	float thickness = NSMinY([fSplitView frame]);
	[[self window] setContentBorderThickness:thickness forEdge: NSMinYEdge];
	
	[fOutlineView setDelegate:self];
	[fOutlineView setDataSource:self];
	[fOutlineView setDoubleAction:@selector(outlineDoubleAction:)];
	[fOutlineView registerForDraggedTypes:[NSArray arrayWithObjects:
										   NSFilenamesPboardType,
										   nil]];
	
	[fTableView setDelegate:self];
	[fTableView setDataSource:self];
	
	[self updateEmulation:self];
}

- (void)updateEmulation:(id)sender
{
	[rootItem release];
	rootItem = [[EmulationItem alloc] initWithDocument:[self document]];
	
	NSString *uid = [[selectedItem uid] copy];
	
	[fOutlineView reloadData];
	[fOutlineView expandItem:nil expandChildren:YES];
	
	if (![self selectItem:rootItem withUid:uid])
		[fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:1]
				  byExtendingSelection:NO];
	
	[uid release];
}

- (void)updateDetails
{
	NSString *locationLabel = NSLocalizedString(@"Location:", "Emulation Label");
	NSString *stateLabel = NSLocalizedString(@"State:", "Emulation Label");
	NSString *formatLabel = NSLocalizedString(@"Format:", "Emulation Label");
	NSString *capacityLabel = NSLocalizedString(@"Capacity:", "Emulation Label");
	
	NSString *title = @"No Selection";
	
	NSImage *image = nil;
	NSString *location = @"";
	NSString *state = @"";
	BOOL isEnabled = YES;
	
	BOOL isCanvas = NO;
	
	BOOL isStorage = NO;
	BOOL isMounted = NO;
	BOOL isLocked = NO;
	NSString *diskImageFormat = @"";
	NSString *diskImageCapacity = @"";
	
	if (selectedItem)
	{
		title = [selectedItem label];
		image = [selectedItem image];
		location = [selectedItem location];
		if (![location length])
			location = NSLocalizedString(@"Built-in", @"Emulation Value.");
		state = NSLocalizedString([selectedItem state],
								  @"Emulation Value.");
		isCanvas = [selectedItem isCanvas];
		isStorage = [selectedItem isStorage];
		isMounted = [selectedItem isMounted];
		isLocked = [selectedItem isLocked];
		diskImageFormat = [selectedItem diskImageFormat];
		diskImageCapacity = [selectedItem diskImageCapacity];
	}
	
	[fDeviceBox setTitle:title];
	
	[fDeviceImage setImage:image];
	if (isMounted)
	{
		[fDeviceState1Label setStringValue:locationLabel];
		[fDeviceState2Label setStringValue:stateLabel];
		[fDeviceState3Label setStringValue:formatLabel];
		[fDeviceState4Label setStringValue:capacityLabel];
		[fDeviceState1Value setStringValue:location];
		[fDeviceState2Value setStringValue:state];
		[fDeviceState3Value setStringValue:diskImageFormat];
		[fDeviceState4Value setStringValue:diskImageCapacity];
	}
	else
	{
		[fDeviceState1Label setStringValue:@""];
		[fDeviceState2Label setStringValue:locationLabel];
		[fDeviceState3Label setStringValue:stateLabel];
		[fDeviceState4Label setStringValue:@""];
		[fDeviceState1Value setStringValue:@""];
		[fDeviceState2Value setStringValue:location];
		[fDeviceState3Value setStringValue:state];
		[fDeviceState4Value setStringValue:@""];
	}
	
	[fDeviceButton setHidden:!(isCanvas || isStorage || isMounted)];
	if (isCanvas)
		[fDeviceButton setTitle:NSLocalizedString(@"Show Device",
												  @"Emulation Button Label.")];
	else if (isMounted)
		[fDeviceButton setTitle:NSLocalizedString(@"Unmount",
												  @"Emulation Button Label.")];
	else if (isStorage)
	{
		[fDeviceButton setTitle:NSLocalizedString(@"Open...",
												  @"Emulation Button Label.")];
		isEnabled = !isLocked;
	}
	
	[fDeviceButton setEnabled:isEnabled];
	
	[fTableView reloadData];
}

- (EmulationItem *)itemForSender:(id)sender
{
	if ([sender isKindOfClass:[NSMenuItem class]] ||
		[sender isKindOfClass:[EmulationOutlineView class]])
	{
		NSInteger clickedRow = [fOutlineView clickedRow];
		return [fOutlineView itemAtRow:clickedRow];
	}
	
	return selectedItem;
}

- (BOOL)selectItem:(EmulationItem *)item withUid:(NSString *)uid
{
	NSString *otherUID = [item uid];
	if (uid && otherUID && ([uid compare:otherUID] == NSOrderedSame))
	{
		NSInteger row = [fOutlineView rowForItem:item];
		[fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:row]
				  byExtendingSelection:NO];
		return YES;
	}
	
	NSArray *children = [item children];
	for (int i = 0; i < [children count]; i++)
	{
		if ([self selectItem:[children objectAtIndex:i] withUid:uid])
			return YES;
	}
	
	return NO;
}

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem >)anItem
{
	SEL action = [anItem action];
	EmulationItem *item = [self itemForSender:anItem];
	
	if (action == @selector(showDevice:))
		return [item isCanvas];
	else if (action == @selector(openDiskImage:))
		return [item isStorage] && ![item isLocked];
	else if (action == @selector(revealInFinder:))
		return [item isMounted];
	else if (action == @selector(unmount:))
		return [item isMounted];
	else if (action == @selector(delete:))
		return [item isRemovable];
	
	return NO;
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
	if ([ident isEqualToString:@"Library"])
	{
		[item setLabel:NSLocalizedString(@"Hardware Library",
										 @"Emulation Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Hardware Library",
												@"Emulation Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Show or hide the hardware library.",
										   @"Emulation Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconLibrary.png"]];
		[item setAction:@selector(toggleLibrary:)];
	}
	else if ([ident isEqualToString:@"AudioControls"])
	{
		[item setLabel:NSLocalizedString(@"Audio Controls",
										 @"Emulation Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Audio Controls",
												@"Emulation Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Show or hide audio controls.",
										   @"Emulation Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconAudio.png"]];
		[item setAction:@selector(toggleAudioControls:)];
	}
	
	return item;
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Library",
			nil];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:
			@"Library",
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
	if (![item isStorage] || (index != -1))
		return NSDragOperationNone;
	
	NSPasteboard *pasteboard = [info draggingPasteboard];
	
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	NSString *path = [[pasteboard propertyListForType:NSFilenamesPboardType]
					  objectAtIndex:0];
	NSString *pathExtension = [[path pathExtension] lowercaseString];
	
	if (![[documentController diskImagePathExtensions] containsObject:pathExtension])
		return NSDragOperationNone;
	
	if (![item canMount:path])
		return NSDragOperationNone;
	
	return NSDragOperationCopy;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
		 acceptDrop:(id < NSDraggingInfo >)info
			   item:(id)item
		 childIndex:(NSInteger)index
{
	NSPasteboard *pasteboard = [info draggingPasteboard];
	NSString *path = [[pasteboard propertyListForType:NSFilenamesPboardType]
					  objectAtIndex:0];
	
	return [item mount:path];
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
	
	[self updateDetails];
}



- (void)outlineDoubleAction:(id)sender
{
	if ([fOutlineView forcedRow] != -1)
		return;
	
	NSInteger clickedRow = [fOutlineView clickedRow];
	if (clickedRow != -1)
	{
		EmulationItem *item = [fOutlineView itemAtRow:clickedRow];
		
		if ([item isCanvas])
			[self showDevice:sender];
		else if ([item isStorage] && ![item isLocked])
			[self openDiskImage:sender];
	}
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

- (void)tableView:(NSTableView *)aTableView
   setObjectValue:(id)anObject
   forTableColumn:(NSTableColumn *)aTableColumn
			  row:(NSInteger)rowIndex
{
	[selectedItem setValue:[anObject stringValue]
		 forSettingAtIndex:rowIndex];
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



- (BOOL)mount:(NSString *)path inItem:(EmulationItem *)item
{
	if (![item mount:path])
	{
		NSBeginAlertSheet([NSString localizedStringWithFormat:
						   @"The document \u201C%@\u201D can't be mounted "
						   "in \u201C%@\u201D.",
						   [path lastPathComponent], [item label]],
						  nil, nil, nil,
						  [self window],
						  self, nil, nil, nil,
						  [NSString localizedStringWithFormat:
						   @"Try mounting the document in some other device "
						   "or emulation."]);
		
		return NO;
	}
	
	return YES;
}

- (IBAction)buttonAction:(id)sender
{
	EmulationItem *item = [self itemForSender:sender];
	
	if ([item isCanvas])
		[self showDevice:sender];
	else if ([item isMounted])
		[self unmount:sender];
	else if ([item isStorage])
		[self openDiskImage:sender];
}

- (IBAction)showDevice:(id)sender
{
	// Get selected device
	// Show all windows
}

- (IBAction)revealInFinder:(id)sender
{
	EmulationItem *item = [self itemForSender:sender];
	
	[[NSWorkspace sharedWorkspace] selectFile:[item diskImagePath]
					 inFileViewerRootedAtPath:@""];
}

- (IBAction)openDiskImage:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	NSArray *fileTypes = [documentController diskImagePathExtensions];
	
	[panel beginSheetForDirectory:nil
							 file:nil
							types:fileTypes
				   modalForWindow:[self window]
					modalDelegate:self
				   didEndSelector:@selector(openPanelDidEnd:returnCode:contextInfo:)
					  contextInfo:[self itemForSender:sender]];
}

- (void)openPanelDidEnd:(NSOpenPanel *)panel
			 returnCode:(int)returnCode
			contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		NSString *path = [[panel URL] path];
		[panel close];
		
		EmulationItem *item = contextInfo;
		if (![item mount:path])
		{
			NSBeginAlertSheet([NSString localizedStringWithFormat:
							   @"The document \u201C%@\u201D can't be mounted "
							   "in \u201C%@\u201D.",
							   [path lastPathComponent], [item label]],
							  nil, nil, nil,
							  [self window],
							  self, nil, nil, nil,
							  [NSString localizedStringWithFormat:
							   @"Try mounting the document in some other device "
							   "or emulation."]);
		}
	}
}

- (IBAction)unmount:(id)sender
{
	EmulationItem *item = [self itemForSender:sender];
	
	if ([item isLocked])
	{
		NSBeginAlertSheet([NSString localizedStringWithFormat:
						   @"Unmount the document \u201C%@\u201D?",
						   [item label]],
						  NSLocalizedString(@"Cancel", @"Emulation Alert"),
						  NSLocalizedString(@"Unmount", @"Emulation Alert"),
						  nil,
						  [self window], self,
						  @selector(unmountPanelDidEnd:returnCode:contextInfo:),
						  nil, item,
						  [NSString localizedStringWithFormat:
						   @"The document is locked by the emulation. "
						   "It is unsafe to force unmount the document."
						   ]);
		
		return;
	}
	
	[item unmount];
}

- (void)unmountPanelDidEnd:(NSWindow *)sheet
				returnCode:(int)returnCode
			   contextInfo:(void *)contextInfo
{
	EmulationItem *item = contextInfo;
	if (returnCode == NSAlertAlternateReturn)
		[item unmount];
}

- (IBAction)delete:(id)sender
{
	// Remove device
}

@end
