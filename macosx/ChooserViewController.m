
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device or template chooser view.
 */

#import "ChooserViewController.h"
#import "ChooserItem.h"

@implementation ChooserViewController

- (id)init
{
	self = [super initWithNibName:@"Chooser" bundle:nil];
	
	if (self)
	{
		chooserDelegate = nil;
		
		groups = [[NSMutableArray alloc] init];
		selectedGroup = nil;
		
		items = [[NSMutableDictionary alloc] init];
	}
	
	return self;
}

- (void)dealloc
{
	[groups release];
	[selectedGroup release];
	
	[items release];
	
	[super dealloc];
}

- (void)setDelegate:(id)theDelegate
{
	chooserDelegate = theDelegate;
}

- (void)awakeFromNib
{
	NSSize cellSize;
	NSDictionary *titleAttributes;
	NSDictionary *subtitleAttributes;
	
	cellSize.width = 108;
	cellSize.height = 64;
	
	titleAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
					   [NSFont messageFontOfSize:11.0f], NSFontAttributeName,
					   [NSColor blackColor], NSForegroundColorAttributeName,
					   nil];
	
	subtitleAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
						  [NSFont messageFontOfSize:11.0f], NSFontAttributeName,
						  [NSColor whiteColor], NSForegroundColorAttributeName,
						  nil];
	
	[fImageBrowserView setAllowsEmptySelection:NO];
	[fImageBrowserView setAllowsMultipleSelection:NO];
	[fImageBrowserView setCellsStyleMask:IKCellsStyleTitled];
	[fImageBrowserView setCellSize:cellSize];
	[fImageBrowserView setDataSource:self];
	[fImageBrowserView setDelegate:self];
	[fImageBrowserView setValue:titleAttributes
						 forKey:IKImageBrowserCellsTitleAttributesKey];
	[fImageBrowserView setValue:subtitleAttributes
						 forKey:IKImageBrowserCellsHighlightedTitleAttributesKey];
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	if (!item)
	{
		[self loadGroups];
		return [groups count];
	}
	
	return 0;
}

- (id)outlineView:(NSOutlineView *)outlineView
objectValueForTableColumn:(NSTableColumn *)tableColumn
		   byItem:(id)item
{
	return item;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	return NO;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	if (!item)
		return [groups objectAtIndex:index];
	
	return nil;
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	int groupIndex = [fOutlineView selectedRow];
	if (groupIndex != -1)
		selectedGroup = [[groups objectAtIndex:groupIndex] copy];
	else
	{
		[selectedGroup release];
		selectedGroup = nil;
	}
	
	[fImageBrowserView reloadData];
	[fImageBrowserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:0]
					  byExtendingSelection:NO];
}

- (void)loadGroups
{
}

- (NSUInteger)numberOfItemsInImageBrowser:(IKImageBrowserView *)aBrowser
{
	if (!selectedGroup)
		return 0;
	
	if (![items objectForKey:selectedGroup])
		[self loadItems];
	
	return [[items objectForKey:selectedGroup] count];
}

- (id)imageBrowser:(IKImageBrowserView *)aBrowser itemAtIndex:(NSUInteger)index
{
	if (!selectedGroup)
		return nil;
	
	return [[items objectForKey:selectedGroup] objectAtIndex:index];
}

- (void)imageBrowserSelectionDidChange:(IKImageBrowserView *) aBrowser
{
	if ([chooserDelegate respondsToSelector:@selector(chooserSelectionDidChange:)])
		[chooserDelegate chooserSelectionDidChange:self];
}

- (void)imageBrowser:(IKImageBrowserView *)aBrowser
cellWasDoubleClickedAtIndex:(NSUInteger)index
{
	if ([chooserDelegate respondsToSelector:@selector(chooserItemWasDoubleClicked:)])
		[chooserDelegate chooserItemWasDoubleClicked:self];
}

- (void)loadItems
{
}

- (void)selectItemWithPath:(NSString *)path
				   inGroup:(NSString *)group
{
	int groupIndex = 0;
	int itemIndex = 0;
	
	if ([groups containsObject:group])
		groupIndex = [groups indexOfObject:group];
	
	NSArray *groupItems = [items objectForKey:group];
	for (int i = 0; i < [groupItems count]; i++)
	{
		ChooserItem *item = [groupItems objectAtIndex:i];
		if ([[item edlPath] compare:path] == NSOrderedSame)
			itemIndex = i;
	}
	
	[fOutlineView reloadData];
	[fOutlineView selectRowIndexes:[NSIndexSet indexSet]
			  byExtendingSelection:NO];
	[fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:groupIndex]
			  byExtendingSelection:NO];
	
	[fImageBrowserView reloadData];
	[fImageBrowserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:itemIndex]
					  byExtendingSelection:NO];
	[fImageBrowserView scrollIndexToVisible:itemIndex];
}

- (NSString *)selectedGroup
{
	if (selectedGroup)
		return [NSString stringWithString:selectedGroup];
	else
		return nil;
}

- (NSString *)selectedItemPath
{
	NSUInteger index = [[fImageBrowserView selectionIndexes] firstIndex];
	if (index == NSNotFound)
		return nil;
	
	ChooserItem *item = [self imageBrowser:fImageBrowserView
							   itemAtIndex:index];
	return [item edlPath];
}

@end
