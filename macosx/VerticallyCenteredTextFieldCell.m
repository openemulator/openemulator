
/**
 * OpenEmulator
 * Vertically Centerred Text Field Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a vertically centered text cell.
 */

#import "VerticallyCenteredTextFieldCell.h"

@implementation VerticallyCenteredTextFieldCell

- (void)setHorizontalOffset:(float)theOffset
{
	offset = theOffset;
}

- (NSRect)titleRectForBounds:(NSRect)theRect
{
    NSRect titleRect = [super titleRectForBounds:theRect];
    NSSize titleSize = [[self attributedStringValue] size];
	
	float diff = (theRect.size.height - titleSize.height) / 2.0;
	titleRect = NSInsetRect(titleRect, offset, diff);
	
    return titleRect;
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
    NSRect titleRect = [self titleRectForBounds:cellFrame];
    [[self attributedStringValue] drawInRect:titleRect];
}

@end
