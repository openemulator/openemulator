
/**
 * OpenEmulator
 * Chooser Outline Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a chooser outline cell.
 */

#import "ChooserOutlineCell.h"

@implementation ChooserOutlineCell

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
    NSSize contentSize = [self cellSize];
    cellFrame.origin.y += (cellFrame.size.height - contentSize.height) / 2.0;
    cellFrame.size.height = contentSize.height;
	
    [super drawInteriorWithFrame:cellFrame inView:controlView];
}

@end
