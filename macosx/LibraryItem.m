
/**
 * OpenEmulator
 * Mac OS X Library Item
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a library item.
 */

#import "LibraryItem.h"

@implementation LibraryItem

- (id)initWithName:(NSString *)theName
			  path:(NSString *)thePath
{
	if (self = [super init])
	{
		name = [theName copy];
		path = [thePath copy];
	}
	
	return self;
}

- (void)dealloc
{
	[name release];
	[path release];
	
	[super dealloc];
}

- (NSString *)name
{
	return name;
}

- (NSString *)path
{
	return path;
}

@end
