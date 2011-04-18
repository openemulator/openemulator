
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

#define SPLIT_VERT_LEFT_MIN 128
#define SPLIT_VERT_RIGHT_MIN 351

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
	float thickness = NSMinY([fSplitView frame]);
	[[self window] setContentBorderThickness:thickness forEdge:NSMinYEdge];
	
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Emulation Toolbar"];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	[[self window] setToolbar:toolbar];
	[toolbar release];
	
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
	NSString *uid = [[selectedItem uid] copy];
	
	[rootItem release];
	rootItem = [[EmulationItem alloc] initRootWithDocument:[self document]];
	
	[fOutlineView reloadData];
	[fOutlineView expandItem:nil expandChildren:YES];
	
	if (![self selectItem:rootItem withUid:uid])
		[fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:1]
				  byExtendingSelection:NO];
	[uid release];
}

- (void)updateDetails
{
	NSString *title = @"No Selection";
	
	NSImage *image = nil;
	NSString *locationLabel = @"";
	NSString *stateLabel = @"";
	BOOL isEnabled = YES;
	
	BOOL hasCanvases = NO;
	BOOL isStorageDevice = NO;
	BOOL isMount = NO;
	
	if (selectedItem)
	{
		title = [selectedItem label];
		image = [selectedItem image];
		locationLabel = [selectedItem locationLabel];
		if (![locationLabel length])
			locationLabel = NSLocalizedString(@"Built-in", @"Emulation Value.");
		stateLabel = NSLocalizedString([selectedItem stateLabel],
									   @"Emulation Value.");
		hasCanvases = [selectedItem hasCanvases];
		isStorageDevice = [selectedItem isStorageDevice];
		isMount = [selectedItem isMount];
	}
	
	[fDeviceBox setTitle:title];
	
	[fDeviceImage setImage:image];
	[fDeviceLocationValue setStringValue:locationLabel];
	[fDeviceStateValue setStringValue:stateLabel];
	
	if (hasCanvases)
	{
		[fDeviceButton setTitle:NSLocalizedString(@"Show Device",
												  @"Emulation Button Label.")];
		[fDeviceButton setHidden:NO];
	}
	else if (isStorageDevice)
	{
		[fDeviceButton setTitle:NSLocalizedString(@"Open...",
												  @"Emulation Button Label.")];
		[fDeviceButton setHidden:NO];
	}
	else if (isMount)
	{
		[fDeviceButton setTitle:NSLocalizedString(@"Unmount",
												  @"Emulation Button Label.")];
		[fDeviceButton setHidden:NO];
	}
	else
		[fDeviceButton setHidden:YES];
	
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
	for (NSInteger i = 0; i < [children count]; i++)
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
	
	if (action == @selector(paste:))
	{
		NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
		return [[pasteboard types] containsObject:NSStringPboardType];
	}
	else if (action == @selector(showDevice:))
		return [item hasCanvases];
	else if (action == @selector(openDiskImage:))
		return [item isStorageDevice];
	else if (action == @selector(unmount:))
		return [item isMount];
	else if (action == @selector(revealInFinder:))
		return [item isMount];
	else if (action == @selector(delete:))
		return NO;
	
	return YES;
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
										 @"Canvas Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Power Down",
												@"Canvas Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Initiate power-down.",
										   @"Canvas Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconPowerDown.png"]];
		[item setAction:@selector(systemPowerDown:)];
	}
	else if ([ident isEqualToString:@"Sleep"])
	{
		[item setLabel:NSLocalizedString(@"Sleep",
										 @"Canvas Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Sleep",
												@"Canvas Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Initiate low power mode.",
										   @"Canvas Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconSleep.png"]];
		[item setAction:@selector(systemSleep:)];
	}
	else if ([ident isEqualToString:@"Wake Up"])
	{
		[item setLabel:NSLocalizedString(@"Wake Up",
										 @"Canvas Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Wake Up",
												@"Canvas Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Initiate full power state.",
										   @"Canvas Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconWakeUp.png"]];
		[item setAction:@selector(systemWakeUp:)];
	}
	else if ([ident isEqualToString:@"Cold Restart"])
	{
		[item setLabel:NSLocalizedString(@"Cold Restart",
										 @"Canvas Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Cold Restart",
												@"Canvas Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Restart the system at the most "
										   "primitive level.",
										   @"Canvas Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconColdRestart.png"]];
		[item setAction:@selector(systemColdRestart:)];
	}
	else if ([ident isEqualToString:@"Warm Restart"])
	{
		[item setLabel:NSLocalizedString(@"Warm Restart",
										 @"Canvas Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Warm Restart",
												@"Canvas Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Restart the operating system.",
										   @"Canvas Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconWarmRestart.png"]];
		[item setAction:@selector(systemWarmRestart:)];
	}
	else if ([ident isEqualToString:@"Debugger Break"])
	{
		[item setLabel:NSLocalizedString(@"Debugger Break",
										 @"Canvas Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Debugger Break",
												@"Canvas Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Break into the operating system debugger.",
										   @"Canvas Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconDebuggerBreak.png"]];
		[item setAction:@selector(systemDebuggerBreak:)];
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
	else if ([ident isEqualToString:@"Library"])
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
			@"Power Down",
			@"Sleep",
			@"Wake Up",
			@"Cold Restart",
			@"Warm Restart",
			@"Debugger Break",
			@"AudioControls",
			@"Library",
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
		
		frame.size.height += deltaHeight;
		if (i == 0)
		{
			float rightWidth = (newSize.width - [sender dividerThickness] -
								frame.size.width);
			if (rightWidth < SPLIT_VERT_RIGHT_MIN)
				frame.size.width += rightWidth - SPLIT_VERT_RIGHT_MIN;
		}
		else
		{
			frame.size.width += deltaWidth;
			if (frame.size.width < SPLIT_VERT_RIGHT_MIN)
			{
				frame.origin.x = newSize.width - SPLIT_VERT_RIGHT_MIN;
				frame.size.width = SPLIT_VERT_RIGHT_MIN;
			}
		}
		
		[subview setFrame:frame];
	}
}

- (CGFloat)splitView:(NSSplitView *)splitView
constrainMinCoordinate:(CGFloat)proposedMin
		 ofSubviewAt:(NSInteger)dividerIndex
{
	return SPLIT_VERT_LEFT_MIN;
}

- (CGFloat)splitView:(NSSplitView *)splitView
constrainMaxCoordinate:(CGFloat)proposedMax
		 ofSubviewAt:(NSInteger)dividerIndex
{
	return ([splitView bounds].size.width - SPLIT_VERT_RIGHT_MIN -
			[splitView dividerThickness]);
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
	if (![item isStorageDevice] || (index != -1))
		return NSDragOperationNone;
	
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	NSPasteboard *pasteboard = [info draggingPasteboard];
	NSString *path = [[pasteboard propertyListForType:NSFilenamesPboardType]
					  objectAtIndex:0];
	
	NSString *pathExtension = [[path pathExtension] lowercaseString];
	if (![[documentController diskImagePathExtensions] containsObject:pathExtension])
		return NSDragOperationNone;
	
	if (![item testMount:path])
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
	
	return [self mount:path inItem:item];
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
		
		if ([item hasCanvases])
			[self showDevice:sender];
		else if ([item isStorageDevice])
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



- (void)tableViewDataCellDidUpdate:(id)sender
{
	[selectedItem setValue:[sender stringValue]
		 forSettingAtIndex:[fTableView clickedRow]];
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
				[sliderCell setContinuous:YES];
				[sliderCell setAction:@selector(tableViewDataCellDidUpdate:)];
				
				return sliderCell;
			}
		}
	}
	
	return nil;
}



- (BOOL)forceMount:(NSString *)path inItem:(EmulationItem *)item
{
	if (![item mount:path])
	{
		NSBeginAlertSheet([NSString localizedStringWithFormat:
						   @"The document \u201C%@\u201D couldn't be mounted "
						   "in \u201C%@\u201D.",
						   [path lastPathComponent], [item label]],
						  nil, nil, nil,
						  [self window],
						  self, nil, nil, nil,
						  [NSString localizedStringWithFormat:
						   @"Try mounting the document in some other device."]);
		
		return NO;
	}
	
	return YES;
}

- (BOOL)mount:(NSString *)path inItem:(EmulationItem *)item
{
	if ([item isLocked])
	{
		NSDictionary *dict = [[NSDictionary dictionaryWithObjectsAndKeys:
							   item, @"item",
							   path, @"path",
							   nil] retain];
		
		NSBeginAlertSheet([NSString localizedStringWithFormat:
						   @"Replace the document in \u201C%@\u201D with \u201C%@\u201D?",
						   [item label], [path lastPathComponent]],
						  NSLocalizedString(@"Cancel", @"Emulation Alert"),
						  NSLocalizedString(@"Replace", @"Emulation Alert"),
						  nil,
						  [self window], self,
						  @selector(remountPanelDidEnd:returnCode:contextInfo:),
						  nil, dict,
						  [NSString localizedStringWithFormat:
						   @"The current document is locked by the emulation. "
						   "It is safer to eject the document from the emulation."
						   ]);
		
		return NO;
	}
	
	return [self forceMount:path inItem:item];
}

- (void)remountPanelDidEnd:(NSWindow *)sheet
				returnCode:(int)returnCode
			   contextInfo:(void *)contextInfo
{
	NSDictionary *dict = contextInfo;
	
	if (returnCode == NSAlertAlternateReturn)
	{
		[sheet orderOut:self];
		
		EmulationItem *item = [dict objectForKey:@"item"];
		NSString *path = [dict objectForKey:@"path"];
		
		[self forceMount:path inItem:item];
	}
	
	[dict release];
}

- (IBAction)buttonAction:(id)sender
{
	EmulationItem *item = [self itemForSender:sender];
	
	if ([item hasCanvases])
		[self showDevice:sender];
	else if ([item isStorageDevice])
		[self openDiskImage:sender];
	else if ([item isMount])
		[self unmount:sender];
}

- (IBAction)showDevice:(id)sender
{
	EmulationItem *item = [self itemForSender:sender];
	
	[item showCanvases];
}

- (IBAction)revealInFinder:(id)sender
{
	EmulationItem *item = [self itemForSender:sender];
	
	[item revealInFinder];
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
		[self mount:path inItem:item];
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
						   "It is safer to eject the document from the emulation."
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

- (void)systemPowerDown:(id)sender
{
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

@end
