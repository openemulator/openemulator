
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
	
    if (self)
		items = [[NSMutableArray alloc] init];
	
	return self;
}

- (void)dealloc
{
    [cell release];
    
	[items release];
	
	[super dealloc];
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Library"];
	
    cell = [[LibraryTableCell alloc] init];
    
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
            // Add to filters
        }
	}
	
	[fTableView setDataSource:self];
	[fTableView setDelegate:self];
    [fTableView setSelectionHighlightStyle:NSTableViewSelectionHighlightStyleNone];
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
	return [items count];
}

- (id)tableView:(NSTableView *)aTableView
objectValueForTableColumn:(NSTableColumn *)aTableColumn
			row:(NSInteger)rowIndex
{
	return [[items objectAtIndex:rowIndex] label];
}

- (NSCell *)tableView:(NSTableView *)tableView
dataCellForTableColumn:(NSTableColumn *)tableColumn
				  row:(NSInteger)row
{
    [cell setRepresentedObject:[items objectAtIndex:row]];
    
    return cell;
}



- (void)tableView:(NSTableView *)aTableView
  willDisplayCell:(id)aCell
   forTableColumn:(NSTableColumn *)aTableColumn
              row:(NSInteger)rowIndex
{
    [aCell setCustomFirstResponder:([[fTableView window] firstResponder] == fTableView)];
    [aCell setCustomSelected:[[aTableView selectedRowIndexes]
                              containsIndex:rowIndex]];
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
