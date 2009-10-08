
/**
 * OpenEmulator
 * Vertically centered cell
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a vertically centered cell.
 */

#import "VerticallyCenteredCell.h"

@implementation VerticallyCenteredCell

- (NSRect)titleRectForBounds:(NSRect)theRect
{
	NSRect titleFrame = [super titleRectForBounds:theRect];
	NSSize titleSize = [[self attributedStringValue] size];
	titleFrame.origin.y = theRect.origin.y + (theRect.size.height - titleSize.height) / 2.0;
	return titleFrame;
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {
    NSRect titleRect = [self titleRectForBounds:cellFrame];
    [[self attributedStringValue] drawInRect:titleRect];
}

@end
