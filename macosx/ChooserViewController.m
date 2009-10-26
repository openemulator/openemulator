
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an chooser view controller for template and device choosing.
 */

#import "ChooserViewController.h"
#import "ChooserItem.h"

@implementation ChooserViewController

- (id) init
{
	self = [super initWithNibName:@"Chooser" bundle:nil];
	
	if (self)
	{
		groups = [[NSMutableDictionary alloc] init];
		groupNames = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void) dealloc
{
	[groupNames release];
	[groups release];
	
	if (selectedGroup)
		[selectedGroup release];
	
	if (fOutlineView)
		[fOutlineView release];
	if (fImageBrowserView)
		[fImageBrowserView release];
	
	[super dealloc];
}

- (void) awakeFromNib
{
	NSSize aSize;
	aSize.width = 112;
	aSize.height = 64;
	
	NSDictionary *attrs = [NSDictionary dictionaryWithObjectsAndKeys:
						   [NSFont messageFontOfSize:11.0f], NSFontAttributeName,
						   [NSColor blackColor], NSForegroundColorAttributeName,
						   nil];
	NSDictionary *hAttrs = [NSDictionary dictionaryWithObjectsAndKeys:
							[NSFont messageFontOfSize:11.0f], NSFontAttributeName,
							[NSColor whiteColor], NSForegroundColorAttributeName,
							nil];
	
	[fImageBrowserView setAllowsEmptySelection:NO];
	[fImageBrowserView setAllowsMultipleSelection:NO];
	[fImageBrowserView setCellSize:aSize];
	[fImageBrowserView setValue:attrs
						 forKey:IKImageBrowserCellsTitleAttributesKey];
	[fImageBrowserView setValue:hAttrs
						 forKey:IKImageBrowserCellsHighlightedTitleAttributesKey];
}

- (void) setDelegate:(id)theDelegate
{
	chooserDelegate = theDelegate;
}

- (id) outlineView:(NSOutlineView *) outlineView child:(NSInteger) index ofItem:(id) item
{
	if (!item)
		return [groupNames objectAtIndex:index];
	
	return nil;
}

- (BOOL) outlineView:(NSOutlineView *) outlineView isItemExpandable:(id) item
{
	return NO;
}

- (NSInteger) outlineView:(NSOutlineView *) outlineView numberOfChildrenOfItem:(id) item
{
	if (!item)
		return [groupNames count];
	
	return 0;
}

- (id)outlineView: (NSOutlineView *) outlineView
objectValueForTableColumn:(NSTableColumn *) tableColumn
		   byItem:(id) item
{
	return item;
}

- (void) outlineViewSelectionDidChange:(NSNotification *) notification
{
	if (selectedGroup)
	{
		[selectedGroup release];
		selectedGroup = nil;
	}
	
	int row = [fOutlineView selectedRow];
	if (row != -1)
	{
		selectedGroup = [groupNames objectAtIndex:row];
		[selectedGroup retain];
	}
	
	[fImageBrowserView reloadData];
	[fImageBrowserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:0]
					  byExtendingSelection:NO];
}

- (NSUInteger) numberOfItemsInImageBrowser:(IKImageBrowserView *) aBrowser
{
	if (!selectedGroup)
		return 0;
	
	return [[groups objectForKey:selectedGroup] count];
}

- (id) imageBrowser:(IKImageBrowserView *) aBrowser itemAtIndex:(NSUInteger) index
{
	if (!selectedGroup)
		return nil;
	
	return [[groups objectForKey:selectedGroup] objectAtIndex:index];
}

- (void) imageBrowser:(IKImageBrowserView *) aBrowser
cellWasDoubleClickedAtIndex:(NSUInteger) index
{
	if ([chooserDelegate respondsToSelector:
		 @selector(chooserWasDoubleClicked:)])
		[chooserDelegate chooserWasDoubleClicked:self];
}

- (void) selectItemWithItemPath:(NSString *) itemPath
{
	int groupIndex = 0;
	int chooserIndex = 0;
	
	int groupsCount = [groupNames count];
	for (int i = 0; i < groupsCount; i++)
	{
		NSString *groupName = [groupNames objectAtIndex:i];
		NSArray *templates = [groups objectForKey:groupName];
		
		int templatesCount = [templates count]; 
		for (int j = 0; j < templatesCount; j++)
		{
			ChooserItem *item = [templates objectAtIndex:j];
			if ([[item itemPath] compare:itemPath] == NSOrderedSame)
			{
				groupIndex = i;
				chooserIndex = j;
			}
		}
	}
	
	[fOutlineView selectRowIndexes:[NSIndexSet indexSet]
			  byExtendingSelection:NO];
	[fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:groupIndex]
			  byExtendingSelection:NO];
	
	[fImageBrowserView reloadData];
	[fImageBrowserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:chooserIndex]
					  byExtendingSelection:NO];
	[fImageBrowserView scrollIndexToVisible:chooserIndex];
}

- (NSString *) selectedItemPath
{
	int index = [[fImageBrowserView selectionIndexes] firstIndex];
	if (index == NSNotFound)
		return nil;
	
	ChooserItem *item = [self imageBrowser:fImageBrowserView
							   itemAtIndex:index];
	return [[[item itemPath] copy] autorelease];
}

@end
