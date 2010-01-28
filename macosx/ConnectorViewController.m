
/**
 * OpenEmulator
 * Mac OS X Connector View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a connector view.
 */

#import "ConnectorViewController.h"
#import "ChooserItem.h"

@implementation ConnectorViewController

- (id) init
{
	self = [super initWithNibName:@"Connector" bundle:nil];
	
	if (self)
		items = [[NSMutableArray alloc] init];
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
	
	[items release];
}

- (void) setDelegate:(id)theDelegate
{
	connectorDelegate = theDelegate;
}

- (void) awakeFromNib
{
	[super awakeFromNib];
	
	NSSize aSize;
	aSize.width = 160;
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
	[fImageBrowserView setCellsStyleMask:IKCellsStyleTitled];
	[fImageBrowserView setCellSize:aSize];
	[fImageBrowserView setValue:attrs
						 forKey:IKImageBrowserCellsTitleAttributesKey];
	[fImageBrowserView setValue:hAttrs
						 forKey:IKImageBrowserCellsHighlightedTitleAttributesKey];
}

- (NSUInteger) numberOfItemsInImageBrowser:(IKImageBrowserView *) aBrowser
{
	return [items count];
}

- (id) imageBrowser:(IKImageBrowserView *) aBrowser itemAtIndex:(NSUInteger) index
{
	return [items objectAtIndex:index];
}

- (void) imageBrowser:(IKImageBrowserView *) aBrowser
cellWasDoubleClickedAtIndex:(NSUInteger) index
{
	if ([connectorDelegate respondsToSelector:
		 @selector(connectorWasDoubleClicked:)])
		[connectorDelegate connectorWasDoubleClicked:self];
}

- (void) updateWithInlets:(NSArray *) inlets
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *imagesPath = [resourcePath
							stringByAppendingPathComponent:@"images"];
	
	[items removeAllObjects];
	for (int i = 0; i < [inlets count]; i++)
	{
		NSMutableDictionary *inlet = [inlets objectAtIndex:i];
		
		NSString *imageName = [inlet objectForKey:@"image"];
		NSString *imagePath = [imagesPath
							   stringByAppendingPathComponent:imageName];
		
		ChooserItem *item = [[ChooserItem alloc]
							 initWithTitle:[inlet objectForKey:@"label"]
							 subtitle:@""
							 imagePath:imagePath
							 data:[inlet objectForKey:@"ref"]];
		
		if (item)
		{
			[item autorelease];
			[items addObject:item];
		}
	}
	
	[fImageBrowserView reloadData];
	[fImageBrowserView setSelectionIndexes:[NSIndexSet
											indexSetWithIndex:0]
					  byExtendingSelection:NO];
}

- (NSString *) selectedInletRef
{
	NSUInteger index = [[fImageBrowserView selectionIndexes] firstIndex];
	if (index == NSNotFound)
		return nil;
	
	ChooserItem *item = [self imageBrowser:fImageBrowserView
							   itemAtIndex:index];
	return [[[item data] copy] autorelease];
}

@end
