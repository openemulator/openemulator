
/**
 * OpenEmulator
 * Vertically Centerred Text Field Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a vertically centered text cell.
 */

#import "VerticallyCenteredTextFieldCell.h"

#define EDIT_OFFSET	2

@implementation VerticallyCenteredTextFieldCell

- (void)setHorizontalInset:(float)theInset;
{
	horizontalInset = theInset;
}

- (NSRect)titleRectForBounds:(NSRect)theRect
{
	NSRect titleRect = [super titleRectForBounds:theRect];
	NSSize titleSize = [[self attributedStringValue] size];
	
	float diff = (theRect.size.height - titleSize.height) / 2.0;
	titleRect = NSInsetRect(titleRect, horizontalInset, diff);
	
	return titleRect;
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	NSRect titleRect = [self titleRectForBounds:cellFrame];
	[[self attributedStringValue] drawInRect:titleRect];
}

- (void)editWithFrame:(NSRect)aRect
			   inView:(NSView *)controlView
			   editor:(NSText *)textObj
			 delegate:(id)anObject
				event:(NSEvent *)theEvent
{
	NSRect titleRect = [self titleRectForBounds:aRect];
	titleRect.origin.x -= EDIT_OFFSET;
	titleRect.size.width += EDIT_OFFSET;
	[super editWithFrame:titleRect
				  inView:controlView
				  editor:textObj
				delegate:anObject
				   event:theEvent];
}

- (void)selectWithFrame:(NSRect)aRect
				 inView:(NSView *)controlView
				 editor:(NSText *)textObj
			   delegate:(id)anObject
				  start:(NSInteger)selStart
				 length:(NSInteger)selLength
{
	NSRect titleRect = [self titleRectForBounds:aRect];
	titleRect.origin.x -= EDIT_OFFSET;
	titleRect.size.width += EDIT_OFFSET;
	[super selectWithFrame:titleRect
					inView:controlView
					editor:textObj
				  delegate:anObject
					 start:selStart
					length:selLength];
}


@end
