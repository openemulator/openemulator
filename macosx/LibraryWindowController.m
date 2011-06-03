
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

@implementation LibraryWindowController

- (id)init
{
	if (self = [super initWithWindowNibName:@"Library"])
	{
		items = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void)dealloc
{
	[items release];
	
	[super dealloc];
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Library"];
	
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *libraryPath = [resourcePath
							 stringByAppendingPathComponent:@"library"];
	
	NSDirectoryEnumerator *dirEnum = [[NSFileManager defaultManager]
									  enumeratorAtPath:libraryPath];
	NSString *path;
	while (path = [dirEnum nextObject])
	{
		if ([[path pathExtension] isEqualToString:@"xml"])
		{
			NSString *name = [[path lastPathComponent]
							  stringByDeletingPathExtension];
			NSString *fullPath = [libraryPath
								  stringByAppendingPathComponent:path];
			
			LibraryItem *item = [[LibraryItem alloc] initWithName:name
															 path:fullPath];
			
			[items addObject:item];
			
			[item release];
		}
	}
	
	[fTableView setDataSource:self];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [items count];
}

- (id)tableView:(NSTableView *)aTableView
objectValueForTableColumn:(NSTableColumn *)aTableColumn
			row:(NSInteger)rowIndex
{
	return [[items objectAtIndex:rowIndex] name];
}

@end
