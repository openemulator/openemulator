
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
	NSString *path;
    NSString *label;
    
    BOOL didLoad;
    NSImage *image;
    NSString *type;
    NSString *description;
}

- (id)initWithPath:(NSString *)thePath;

- (NSString *)path;

- (NSString *)label;
- (NSImage *)image;
- (NSString *)type;
- (NSString *)description;

@end
