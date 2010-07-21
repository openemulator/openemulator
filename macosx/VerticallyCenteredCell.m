
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

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
    NSSize contentSize = [self cellSize];
    cellFrame.origin.y += (cellFrame.size.height - contentSize.height) / 2.0;
    cellFrame.size.height = contentSize.height;
	
    [super drawInteriorWithFrame:cellFrame inView:controlView];
}

@end
