
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

#define SPLIT_VERT_MIN 128
#define SPLIT_VERT_MAX 256

@implementation EmulationWindowController

- (id)init
{
	if (self = [self initWithWindowNibName:@"Emulation"])
	{
		contents = [[EmulationOutlineItem alloc] initWithUid:nil
													   label:nil
													   image:nil
													showable:NO
												   ejectable:NO];
		
		textCell = [[VerticallyCenteredTextFieldCell alloc] initTextCell:@""];
		[textCell setControlSize:NSSmallControlSize];
		[textCell setFont:[NSFont labelFontOfSize:[NSFont smallSystemFontSize]]];
		[textCell setLineBreakMode:NSLineBreakByTruncatingTail];
		[textCell setHorizontalOffset:6.0];
		
		checkBoxCell = [[NSButtonCell alloc] initTextCell:@""];
		[checkBoxCell setControlSize:NSSmallControlSize];
		[checkBoxCell setFont:[NSFont labelFontOfSize:[NSFont smallSystemFontSize]]];
		[checkBoxCell setButtonType:NSSwitchButton];
		
		popUpButtonCell = [[NSPopUpButtonCell alloc] initTextCell:@""];
		[popUpButtonCell setControlSize:NSSmallControlSize];
		[popUpButtonCell setFont:[NSFont labelFontOfSize:[NSFont smallSystemFontSize]]];
		[popUpButtonCell addItemWithTitle:@"hi there"];
		[popUpButtonCell addItemWithTitle:@"hi again"];
		[popUpButtonCell setBordered:NO];
		
		sliderCell = [[NSSliderCell alloc] initTextCell:@""];
		[sliderCell setControlSize:NSSmallControlSize];
		[sliderCell setMinValue:0.0];
		[sliderCell setMaxValue:1.0];
		[sliderCell	setNumberOfTickMarks:1];
//		[sliderCell setAllowsTickMarkValuesOnly:YES];
	}
	
	return self;
}

- (void)dealloc
{
	[contents release];
	
	[textCell release];
	[checkBoxCell release];
	[popUpButtonCell release];
	[sliderCell release];
	
	[super dealloc];
}

- (NSImage *)getImage:(NSString *)path
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	path = [resourcePath stringByAppendingPathComponent:path];
	
	return [[[NSImage alloc] initByReferencingFile:path] autorelease];
}

- (void)updateContents
{
	OEDevicesInfo *devicesInfo = (OEDevicesInfo *)[[self document] devicesInfo];
	
	[[contents children] removeAllObjects];
	
	for (int i = 0; i < devicesInfo->size(); i++)
	{
		string uid = (*devicesInfo)[i].id;
		string label = (*devicesInfo)[i].label;
		string image = (*devicesInfo)[i].image;
		BOOL showable = ((*devicesInfo)[i].canvases.size() != 0);
		BOOL ejectable = (*devicesInfo)[i].mounted;
		string group = (*devicesInfo)[i].group;
		
		NSString *groupName = getNSString(group);
		EmulationOutlineItem *groupItem = [contents objectWithUid:groupName];
		
		if (!groupItem)
		{
			NSString *label = [groupName uppercaseString];
			groupItem = [[EmulationOutlineItem alloc] initWithUid:groupName
															label:label
															image:nil
														 showable:showable
														ejectable:ejectable];
			[[contents children] addObject:groupItem];
			[groupItem release];
		}
		
		EmulationOutlineItem *newItem;
		NSImage *outlineImage = [self getImage:getNSString(image)];
		newItem = [[EmulationOutlineItem alloc] initWithUid:getNSString(uid)
													  label:getNSString(label)
													  image:outlineImage
												   showable:showable
												  ejectable:ejectable];
		[[groupItem children] addObject:newItem];
		[newItem release];
	}
}

- (void)updateWindowPosition
{
	int isVisible = [[self window] isVisible];
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
	
	[self updateContents];
	
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

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
	 itemForItemIdentifier:(NSString *)ident
 willBeInsertedIntoToolbar:(BOOL)flag
{
	NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
	
	if (!item)
		return nil;
	
	if ([ident isEqualToString:@"AudioControls"])
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
	
	[item autorelease];
	
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
	
	for (int i = 0; i < [subviews count]; i++)
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
		item = contents;
	
    return [[item children] count];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
   isItemExpandable:(id)item
{
	if (!item)
		item = contents;
	
	return ([[item children] count] > 0);
}

- (id)outlineView:(NSOutlineView *)outlineView
			child:(NSInteger)index 
		   ofItem:(id)item
{
	if (!item)
        item = contents;
	
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
	[cell setUid:[item uid]
		   image:[item image]
		showable:[item showable]
	   ejectable:[item ejectable]];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
   shouldSelectItem:(id)item
{
	return ([outlineView parentForItem:item] != nil);
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
		isGroupItem:(id)item
{
	return ![outlineView parentForItem:item];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return 0;
}

- (id)tableView:(NSTableView *)aTableView
objectValueForTableColumn:(NSTableColumn *)aTableColumn
			row:(NSInteger)rowIndex
{
	if (aTableColumn == fTableKeyColumn)
		return @"Auto CR after LF";
	else if (aTableColumn == fTableValueColumn)
		return @"A Long Value";
	
	return @"";
}

- (NSCell *)tableView:(NSTableView *)tableView
dataCellForTableColumn:(NSTableColumn *)tableColumn
				  row:(NSInteger)row
{
	if (tableColumn == fTableKeyColumn)
		return textCell;
	
	if (tableColumn == fTableValueColumn)
	{
		switch (row)
		{
			case 1:
				return checkBoxCell;
			case 2: 
				return popUpButtonCell;
			case 3:
				return sliderCell;
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
