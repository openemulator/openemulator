
/**
 * OpenEmulator
 * Mac OS X Emulation Outline Item
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation outline item.
 */

#import <Cocoa/Cocoa.h>

@interface EmulationOutlineItem : NSObject
{
    NSString *uid;
	NSString *label;
	NSImage *image;
	BOOL showable;
	BOOL ejectable;
	
    NSMutableArray *children;
}

- (id)initWithUid:(NSString *)theUid
			label:(NSString *)theLabel
			image:(NSImage *)theImage
		 showable:(BOOL)isShowable
		ejectable:(BOOL)isEjectable;

- (NSString *)uid;
- (NSString *)label;
- (NSImage *)image;
- (BOOL)showable;
- (BOOL)ejectable;

- (NSMutableArray *)children;
- (EmulationOutlineItem *)objectWithUid:(NSString *)theUid;

@end
