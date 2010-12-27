
/**
 * OpenEmulator
 * Chooser Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a chooser cell.
 */

#import "ChooserCell.h"

@implementation ChooserCell

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	float cellLeftPadding = 5.0;
	
    NSSize contentSize = [self cellSize];
	cellFrame.origin.x += cellLeftPadding;
    cellFrame.origin.y += (cellFrame.size.height - contentSize.height) / 2.0;
	cellFrame.size.width -= cellLeftPadding;
    cellFrame.size.height = contentSize.height;
	
    [super drawWithFrame:cellFrame inView:controlView];
}

@end
