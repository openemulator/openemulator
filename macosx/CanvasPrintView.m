
/**
 * OpenEmulator
 * Mac OS X Canvas Print View
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas print view.
 */

#import "CanvasPrintView.h"

@implementation CanvasPrintView

- (void)drawRect:(NSRect)theRect
{
	
}

- (BOOL)knowsPageRange:(NSRangePointer)aRange
{
	return [super knowsPageRange:aRange];	
}

- (NSRect)rectForPage:(NSInteger)pageNumber
{
	
}

@end
