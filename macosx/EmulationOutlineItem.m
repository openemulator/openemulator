
/**
 * OpenEmulator
 * Mac OS X Emulation Outline Item
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation outline item.
 */

#import "EmulationOutlineItem.h"

@implementation EmulationOutlineItem

- (id)initWithUid:(NSString *)theUid
			label:(NSString *)theLabel
			image:(NSImage *)theImage
		 showable:(BOOL)isShowable
		ejectable:(BOOL)isEjectable;
{
    if (self = [super init])
	{
		uid = [theUid copy];
		label = [theLabel copy];
		image = [theImage copy];
		showable = isShowable;
		ejectable = isEjectable;
		
		children = [[NSMutableArray alloc] init];
	}
	
    return self;
}

- (void)dealloc
{
	[uid release];
	[label release];
	[image release];
	
	[children release];
	
	[super dealloc];
}

- (NSString *)uid
{
	return uid;
}

- (NSString *)label
{
	return label;
}

- (NSImage *)image
{
	return image;
}

- (BOOL)showable
{
	return showable;
}

- (BOOL)ejectable
{
	return ejectable;
}

- (NSMutableArray *)children
{
    return children;
}

- (EmulationOutlineItem *)objectWithUid:(NSString *)theUid
{
	for (EmulationOutlineItem *item in children)
	{
		if ([[item uid] compare:theUid] == NSOrderedSame)
			return item;
	}
	
	return nil;
}

@end
