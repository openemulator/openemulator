
/**
 * OpenEmulator
 * Mac OS X Emulation Window Controller
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation window
 */

#import "EmulationWindowController.h"

#import "EmulationItem.h"
#import "EmulationOutlineView.h"
#import "EmulationOutlineCell.h"
#import "VerticallyCenteredTextFieldCell.h"
#import "DocumentController.h"

#define SPLIT_VERT_LEFT_MIN 128
#define SPLIT_VERT_RIGHT_MIN 351

@implementation EmulationWindowController

- (id)init
{
    self = [super initWithWindowNibName:@"Emulation"];
    
    if (self)
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
    document = [self document];
    
    [self setWindowFrameAutosaveName:@"Emulation"];
    
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
                                           @"OEDeviceType",
                                           nil]];
    
    [fTableView setDelegate:self];
    [fTableView setDataSource:self];
    
    [[self window] setDelegate:self];
    
    [self updateWindow:self];
}

- (void)updateWindow:(id)sender
{
    if (![self isWindowLoaded])
        return;
    
    // Preserve selected uid
    NSString *uid = [[selectedItem uid] copy];
    
    [rootItem release];
    rootItem = [[EmulationItem alloc] initRootWithDocument:document];
    
    [fOutlineView reloadData];
    [fOutlineView expandItem:nil expandChildren:YES];
    
    if (![self selectItem:rootItem withUid:uid])
        [fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:1]
                  byExtendingSelection:NO];
    [uid release];
    
    int deviceCount = 0;
    int availablePortCount = 0;
    
    NSMutableArray *groups = [rootItem children];
    for (int i = 0; i < [groups count]; i++)
    {
        EmulationItem *groupItem = [groups objectAtIndex:i];
        NSMutableArray *children = [groupItem children];
        for (int j = 0; j < [children count]; j++)
        {
            EmulationItem *item = [children objectAtIndex:j];
            if (![item isPort])
                deviceCount++;
            else
                availablePortCount++;
        }
    }
    
    NSString *label = @"";
    if (deviceCount == 1)
        label = [label stringByAppendingString:
                 [NSString localizedStringWithFormat:@"1 device"]];
    else if (deviceCount > 1)
        label = [label stringByAppendingString:
                 [NSString localizedStringWithFormat: @"%d devices",
                  deviceCount]];
    
    if (deviceCount && availablePortCount)
        label = [label stringByAppendingString:@", "];
    
    if (availablePortCount == 1)
        label = [label stringByAppendingString:
                 [NSString localizedStringWithFormat:@"1 available port"]];
    else if (availablePortCount > 1)
        label = [label stringByAppendingString:
                 [NSString localizedStringWithFormat: @"%d available ports",
                  availablePortCount]];
    [fStatusLabelView setStringValue:label];
}

- (void)updateDetails
{
    NSString *title = @"No Selection";
    
    NSImage *image = nil;
    NSString *locationLabel = @"";
    NSString *stateTitle = @"";
    NSString *stateLabel = @"";
    BOOL isEnabled = YES;
    
    BOOL hasStorages = NO;
    BOOL isMount = NO;
    BOOL hasCanvases = NO;
    
    if (selectedItem)
    {
        title = [selectedItem label];
        image = [selectedItem image];
        locationLabel = [selectedItem locationLabel];
        if (![locationLabel length])
            locationLabel = NSLocalizedString(@"System", @"Emulation Value.");
        if ([selectedItem isMount])
            stateTitle = NSLocalizedString(@"Format:", @"Emulation Format.");
        else
            stateTitle = NSLocalizedString(@"State:", @"Emulation State.");
        stateLabel = NSLocalizedString([selectedItem stateLabel], @"Emulation Value.");
        if (![stateLabel length])
        {
            if ([selectedItem isPort])
                stateLabel = NSLocalizedString(@"Disconnected", @"Emulation Value.");
            else
                stateLabel = NSLocalizedString(@"Connected", @"Emulation Value.");
        }
        hasStorages = [selectedItem hasStorages];
        isMount = [selectedItem isMount];
        hasCanvases = [selectedItem hasCanvases];
    }
    
    [fDeviceBox setTitle:title];
    
    [fDeviceImage setImage:image];
    [fDeviceLocationLabel setStringValue:locationLabel];
    [fDeviceStateTitle setStringValue:stateTitle];
    [fDeviceStateLabel setStringValue:stateLabel];
    
    if (hasStorages)
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
    else if (hasCanvases)
    {
        [fDeviceButton setTitle:NSLocalizedString(@"Show Device",
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
        
        EmulationItem *item = [fOutlineView itemAtRow:clickedRow];
        
        if (item)
            return item;
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
    
    if (action == @selector(paste:))
    {
        NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
        return [[pasteboard types] containsObject:NSStringPboardType];
    }
    else if (action == @selector(showDevice:))
        return [item hasCanvases];
    else if (action == @selector(openDiskImage:))
        return [item hasStorages];
    else if (action == @selector(unmount:))
        return [item isMount];
    else if (action == @selector(showInFinder:))
        return [item isMount];
    else if (action == @selector(delete:))
        return [item isRemovable];
    else if ((action == @selector(sendPowerDown:)) ||
             (action == @selector(sendSleep:)) ||
             (action == @selector(sendWakeUp:)) ||
             (action == @selector(sendColdRestart:)) ||
             (action == @selector(sendWarmRestart:)) ||
             (action == @selector(sendDebuggerBreak:)))
        return [item isDeviceObserved];
    
    return YES;
}

// Toolbar

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
        [item setAction:@selector(sendPowerDown:)];
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
        [item setAction:@selector(sendSleep:)];
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
        [item setAction:@selector(sendWakeUp:)];
    }
    else if ([ident isEqualToString:@"Cold Restart"])
    {
        [item setLabel:NSLocalizedString(@"Restart",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Restart",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Restart the system at the most "
                                           "primitive level.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconColdRestart.png"]];
        [item setAction:@selector(sendColdRestart:)];
    }
    else if ([ident isEqualToString:@"Warm Restart"])
    {
        [item setLabel:NSLocalizedString(@"Reset",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Reset",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Restart the operating system.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconWarmRestart.png"]];
        [item setAction:@selector(sendWarmRestart:)];
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
        [item setAction:@selector(sendDebuggerBreak:)];
    }
    else if ([ident isEqualToString:@"Revert to Saved"])
    {
        [item setLabel:NSLocalizedString(@"Revert to Saved",
                                         @"Emulation Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Revert to Saved",
                                                @"Emulation Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Revert document to saved state.",
                                           @"Emulation Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconRevert.png"]];
        [item setAction:@selector(revertDocumentToSaved:)];
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
            @"Power Down",
            @"Sleep",
            @"Wake Up",
            @"Warm Restart",
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
            @"Revert to Saved",
            @"AudioControls",
            @"Library",
            NSToolbarSeparatorItemIdentifier,
            NSToolbarSpaceItemIdentifier,
            NSToolbarFlexibleSpaceItemIdentifier,
            nil];
}

// Split view

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

// Outline view

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

- (BOOL)outlineView:(NSOutlineView *)outlineView
 shouldCollapseItem:(id)item
{
    return NO;
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
    NSPasteboard *pboard = [info draggingPasteboard];
    if ([[pboard types] containsObject:NSFilenamesPboardType])
    {
        NSString *path = [[pboard propertyListForType:NSFilenamesPboardType]
                          objectAtIndex:0];
        
        NSString *pathExtension = [[path pathExtension] lowercaseString];
        
        DocumentController *documentController;
        documentController = [NSDocumentController sharedDocumentController];
        
        if ([[documentController diskImagePathExtensions] containsObject:pathExtension])
        {
            if ([item hasStorages] && (index == -1))
            {
                if ([item canMount:path])
                    return NSDragOperationCopy;
            }
        }
    }
    else if ([[pboard types] containsObject:@"OEDeviceType"])
    {
        if (index == -1)
        {
            NSDictionary *dict = [pboard propertyListForType:@"OEDeviceType"];
            
            NSString *type = [dict objectForKey:@"type"];
            if ([type compare:[item portType]] == NSOrderedSame)
                return NSDragOperationCopy;
        }
    }
    
    return NSDragOperationNone;	
}

- (BOOL)outlineView:(NSOutlineView *)outlineView
         acceptDrop:(id < NSDraggingInfo >)info
               item:(id)item
         childIndex:(NSInteger)index
{
    NSPasteboard *pboard = [info draggingPasteboard];
    if ([[pboard types] containsObject:NSFilenamesPboardType])
    {
        NSString *path = [[pboard propertyListForType:NSFilenamesPboardType]
                          objectAtIndex:0];
        
        DocumentController *documentController;
        documentController = [NSDocumentController sharedDocumentController];
        
        NSString *pathExtension = [[path pathExtension] lowercaseString];
        if ([[documentController diskImagePathExtensions] containsObject:pathExtension])
        {
            [self mount:path inItem:item];
            
            return YES;
        }
    }
    else if ([[pboard types] containsObject:@"OEDeviceType"])
    {
        NSDictionary *dict = [pboard propertyListForType:@"OEDeviceType"];
        
        NSString *path = [dict objectForKey:@"path"];
        NSString *label = [dict objectForKey:@"label"];
        
        [self connect:path label:label toItem:item];
        
        return YES;
    }
    
    return NO;
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
        
        if ([item hasStorages])
            [self openDiskImage:sender];
        else if ([item hasCanvases])
            [self showDevice:sender];
    }
}

// Table view

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

- (IBAction)outlineAction:(id)sender
{
    EmulationItem *item = [self itemForSender:sender];
    
    if ([item hasCanvases])
        [self showDevice:sender];
    else if ([item isMount])
        [self unmount:sender];
}

- (IBAction)buttonAction:(id)sender
{
    EmulationItem *item = [self itemForSender:sender];
    
    if ([item hasStorages])
        [self openDiskImage:sender];
    else if ([item isMount])
        [self unmount:sender];
    else if ([item hasCanvases])
        [self showDevice:sender];
}

// Operations

- (IBAction)openDiskImage:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    DocumentController *documentController;
    documentController = [NSDocumentController sharedDocumentController];
    
    [panel setAllowedFileTypes:[documentController diskImagePathExtensions]];
    [panel beginSheetModalForWindow:[self window]
                  completionHandler:^(NSInteger returnCode)
     {
         if (returnCode == NSOKButton)
         {
             [panel close];
             
             NSString *path = [[panel URL] path];
             EmulationItem *item = [self itemForSender:sender];
             
             [self mount:path inItem:item];
         }
     }];
}

- (BOOL)mount:(NSString *)path inItem:(EmulationItem *)item
{
    if ([item isLocked])
    {
        NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                              item, @"item",
                              path, @"path",
                              nil];
        
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
    
    return [self doMount:path inItem:item];
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
        
        [self doMount:path inItem:item];
    }
    
    [dict release];
}

- (BOOL)doMount:(NSString *)path inItem:(EmulationItem *)item
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
                           @"Try mounting the document in some other device."]);
        
        return NO;
    }
    
    return YES;
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
    
    [self doUnmount:item];
}

- (void)unmountPanelDidEnd:(NSWindow *)sheet
                returnCode:(int)returnCode
               contextInfo:(void *)contextInfo
{
    EmulationItem *item = contextInfo;
    
    if (returnCode == NSAlertAlternateReturn)
        [self doUnmount:item];
}

- (BOOL)doUnmount:(EmulationItem *)item
{
    if (![item unmount])
    {
        NSBeginAlertSheet([NSString localizedStringWithFormat:
                           @"The document in \u201C%@\u201D can't be unmounted.",
                           [item label]],
                          nil, nil, nil,
                          [self window],
                          self, nil, nil, nil,
                          [NSString localizedStringWithFormat:
                           @"Check the console for additional information."]);
        
        return NO;
    }
    
    return YES;
}

- (IBAction)showDevice:(id)sender
{
    EmulationItem *item = [self itemForSender:sender];
    
    [item showCanvases];
}

- (IBAction)showInFinder:(id)sender
{
    EmulationItem *item = [self itemForSender:sender];
    
    [item showInFinder];
}

- (IBAction)delete:(id)sender
{
    EmulationItem *item = [self itemForSender:sender];
    
    if ([item isMount])
    {
        [item unmount];
        
        return;
    }
    
    if (![item isRemovable])
        return;
    
    NSBeginAlertSheet([NSString localizedStringWithFormat:
                       @"Are you sure you want to delete the device \u201C%@\u201D?",
                       [item label]],
                      NSLocalizedString(@"Delete", @"Emulation Alert"),
                      NSLocalizedString(@"Cancel", @"Emulation Alert"),
                      nil,
                      [self window], self,
                      @selector(deletePanelDidEnd:returnCode:contextInfo:),
                      nil, item,
                      [NSString localizedStringWithFormat:
                       @"This might also delete the devices connected to \u201C%@\u201D.",
                       [item label]]);
    
    return;
}

- (void)deletePanelDidEnd:(NSWindow *)sheet
               returnCode:(int)returnCode
              contextInfo:(void *)contextInfo
{
    if (returnCode == NSAlertDefaultReturn)
    {
        EmulationItem *item = contextInfo;
        
        [item remove];
        
        [self updateWindow:self];
        
        [document updateChangeCount:NSChangeDone];
    }
}

- (BOOL)connect:(NSString *)thePath
          label:(NSString *)theLabel
         toItem:(EmulationItem *)theItem
{
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    NSString *libraryPath = [resourcePath
                             stringByAppendingPathComponent:@"library"];
    NSString *path = [libraryPath stringByAppendingPathComponent:thePath];
    
    if ([theItem isPort])
        return [self doConnect:path toItem:theItem];
    else
    {
        NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                              path, @"path",
                              theLabel, @"label",
                              [NSValue valueWithPointer:theItem], @"item",
                              nil];
        
        NSBeginAlertSheet([NSString localizedStringWithFormat:
                           @"Are you sure you want to replace the device \u201C%@\u201D?",
                           [theItem label]],
                          NSLocalizedString(@"Replace", @"Emulation Alert"),
                          NSLocalizedString(@"Cancel", @"Emulation Alert"),
                          nil,
                          [self window], self,
                          @selector(connectPanelDidEnd:returnCode:contextInfo:),
                          nil, dict,
                          [NSString localizedStringWithFormat:
                           @"This might also delete the devices connected to \u201C%@\u201D.",
                           [theItem label]]);
    }
    
    return YES;
}

- (void)connectPanelDidEnd:(NSWindow *)sheet
                returnCode:(int)returnCode
               contextInfo:(void *)contextInfo
{
    NSDictionary *dict = (NSDictionary *)contextInfo;
    
    if (returnCode == NSAlertDefaultReturn)
    {
        [sheet orderOut:self];
        
        EmulationItem *item = (EmulationItem *)[[dict objectForKey:@"item"] pointerValue];
        NSString *path = [dict objectForKey:@"path"];
        
        [self doConnect:path toItem:item];
    }
    
    [dict release];
}

- (BOOL)doConnect:(NSString *)thePath
           toItem:(EmulationItem *)theItem
{
    if ([theItem addOEDocument:thePath])
    {
        [self updateWindow:self];
        
        [document updateChangeCount:NSChangeDone];
        
        return YES;
    }
    else
        NSBeginAlertSheet([NSString localizedStringWithFormat:
                           @"The device could not be connected."],
                          nil, nil, nil,
                          [self window],
                          self, nil, nil, nil,
                          [NSString localizedStringWithFormat:
                           @"Check the console for additional information."]);
    
    return NO;
}

- (void)sendPowerDown:(id)sender
{
    if (selectedItem)
        [selectedItem sendDeviceEvent:EMULATIONDEVICEEVENT_POWERDOWN];
}

- (void)sendSleep:(id)sender
{
    if (selectedItem)
        [selectedItem sendDeviceEvent:EMULATIONDEVICEEVENT_SLEEP];
}

- (void)sendWakeUp:(id)sender
{
    if (selectedItem)
        [selectedItem sendDeviceEvent:EMULATIONDEVICEEVENT_WAKEUP];
}

- (void)sendColdRestart:(id)sender
{
    if (selectedItem)
        [selectedItem sendDeviceEvent:EMULATIONDEVICEEVENT_COLDRESTART];
}

- (void)sendWarmRestart:(id)sender
{
    if (selectedItem)
        [selectedItem sendDeviceEvent:EMULATIONDEVICEEVENT_WARMRESTART];
}

- (void)sendDebuggerBreak:(id)sender
{
    if (selectedItem)
        [selectedItem sendDeviceEvent:EMULATIONDEVICEEVENT_DEBUGGERBREAK];
}

@end
