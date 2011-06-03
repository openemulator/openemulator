
/**
 * OpenEmulator
 * Mac OS X Library Item
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a library item.
 */

#import <Cocoa/Cocoa.h>

@interface LibraryItem : NSObject
{
	NSString *name;
	NSString *path;
}

- (id)initWithName:(NSString *)name
			  path:(NSString *)path;

- (NSString *)name;
- (NSString *)path;

@end
