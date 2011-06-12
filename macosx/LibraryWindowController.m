
/**
 * OpenEmulator
 * Mac OS X Library Window Controller
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the library window.
 */

#import "LibraryWindowController.h"
#import "LibraryItem.h"

#define SPLIT_MIN 128
#define SPLIT_MAX 256

@implementation LibraryWindowController

- (id)init
{
    self = [super initWithWindowNibName:@"Library"];
	
	return self;
}

- (void)dealloc
{
	[filterPaths release];
	[items release];
	[filteredItems release];
	
    [cell release];
    
	[super dealloc];
}

- (void)windowDidLoad
{
    filterPaths = [[NSMutableArray alloc] init];
    items = [[NSMutableArray alloc] init];
    filteredItems = [[NSMutableArray alloc] init];
    
    cell = [[LibraryTableCell alloc] init];
    
	[self setWindowFrameAutosaveName:@"Library"];
	
    [filterPaths addObject:@""];
    [fPathFilter addItemWithTitle:NSLocalizedString(@"Library", @"Library Root")];
    
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *libraryPath = [resourcePath
							 stringByAppendingPathComponent:@"library"];
	
	NSDirectoryEnumerator *dirEnum = [[NSFileManager defaultManager]
									  enumeratorAtPath:libraryPath];
	NSString *path;
	while ((path = [dirEnum nextObject]))
	{
		if ([[path pathExtension] isEqualToString:@"xml"])
		{
			LibraryItem *item = [[LibraryItem alloc] initWithPath:path];
			
			[items addObject:item];
			
			[item release];
		}
        else if ([[path pathExtension] isEqualToString:@""])
        {
            NSString *title = NSLocalizedString([path lastPathComponent],
                                                @"Library Path");
            
            NSString *tempPath = [[path copy] autorelease];
            while ([tempPath length])
            {
                title = [@"  " stringByAppendingString:title];
                tempPath = [tempPath stringByDeletingLastPathComponent];
            }
            
            [filterPaths addObject:path];
            [fPathFilter addItemWithTitle:title];
        }
	}
	
	[fTableView setDataSource:self];
	[fTableView setDelegate:self];
    
    [self filterItems:self];
}

- (IBAction)filterItems:(id)sender
{
    NSString *filterPath = [filterPaths objectAtIndex:
                            [fPathFilter indexOfSelectedItem]];
    if ([filterPath length])
        filterPath = [filterPath stringByAppendingString:@"/"];
    int filterPathLength = (int) [filterPath length];
    
    NSString *searchString = [fSearchFilter stringValue];
    
    [filteredItems removeAllObjects];
    
    for (int i = 0; i < [items count]; i++)
    {
        LibraryItem *item = [items objectAtIndex:i];
        
        // Validate path condition
        if ([[[item path] substringToIndex:filterPathLength] compare:
            filterPath] != NSOrderedSame)
            continue;
        
        // Validate search condition
        if ([searchString length])
        {
            NSRange range;
            
            range = [[item path] rangeOfString:searchString
                                       options:NSCaseInsensitiveSearch];
            if (range.location == NSNotFound)
                continue;
        }
        
        [filteredItems addObject:item];
    }
    
    [fTableView reloadData];
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
		
        frame.size.width += deltaWidth;
        if (i == 0)
        {
            frame.size.height += deltaHeight;
            if (frame.size.height < SPLIT_MIN)
                frame.size.height = SPLIT_MIN;
        }
        else
        {
            frame.origin.y += deltaHeight;
            float maxHeight = (newSize.height - [sender dividerThickness] -
                               SPLIT_MIN);
            if (frame.size.height >= maxHeight)
            {
                frame.origin.y += frame.size.height - maxHeight;
                frame.size.height = maxHeight;
            }
        }
		
		[subview setFrame:frame];
	}
}

- (CGFloat)splitView:(NSSplitView *)splitView
constrainMinCoordinate:(CGFloat)proposedMin
		 ofSubviewAt:(NSInteger)dividerIndex
{
	return SPLIT_MIN;
}



- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [filteredItems count];
}

- (id)tableView:(NSTableView *)aTableView
objectValueForTableColumn:(NSTableColumn *)aTableColumn
			row:(NSInteger)rowIndex
{
	return [[filteredItems objectAtIndex:rowIndex] label];
}

- (BOOL)tableView:(NSTableView *)aTableView
writeRowsWithIndexes:(NSIndexSet *)rowIndexes
     toPasteboard:(NSPasteboard *)pboard
{
    LibraryItem *item = [filteredItems objectAtIndex:[rowIndexes firstIndex]];

    NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                          [item path], @"path",
                          [item label], @"label",
                          [item type], @"type",
                          nil];
    
    [pboard declareTypes:[NSArray arrayWithObject:@"OEDeviceType"] owner:self];
    [pboard clearContents];
    [pboard setPropertyList:dict forType:@"OEDeviceType"];
    
    return YES;
}



- (NSCell *)tableView:(NSTableView *)tableView
dataCellForTableColumn:(NSTableColumn *)tableColumn
				  row:(NSInteger)row
{
    [cell setRepresentedObject:[filteredItems objectAtIndex:row]];
    
    return cell;
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    LibraryItem *item = nil;
    
	NSInteger index = [fTableView selectedRow];
	if (index != -1)
        item = [items objectAtIndex:index];
	
	[fSelImage setImage:[item image]];
    [fSelLabel setStringValue:NSLocalizedString([item label],
                                                @"Device Label")];
    [fSelType setStringValue:NSLocalizedString([item type],
                                               @"Device Type")];
    [fSelDescription setStringValue:NSLocalizedString([item description],
                                                      @"Device Description")];
}

@end
