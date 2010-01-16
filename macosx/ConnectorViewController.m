
/**
 * OpenEmulator
 * Mac OS X Connector View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a conncetor view.
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

- (NSString *) selectedItemPath
{
	NSUInteger index = [[fImageBrowserView selectionIndexes] firstIndex];
	if (index == NSNotFound)
		return nil;
	
	ChooserItem *item = [self imageBrowser:fImageBrowserView
							   itemAtIndex:index];
	return [[[item data] copy] autorelease];
}

@end
