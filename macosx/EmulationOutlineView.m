
/**
 * OpenEmulator
 * Mac OS X Emulation Outline View
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation outline view.
 */

#import "EmulationOutlineView.h"
#import "EmulationOutlineCell.h"

@implementation EmulationOutlineView

- (id) initWithCoder:(NSCoder *)decoder
{
	if ((self = [super initWithCoder:decoder]))
	{
		forcedRow = -1;
		trackedRow = -1;
	}
	
	return self;
}

- (NSInteger)forcedRow
{
	return forcedRow;
}

- (void)setTrackedRow:(NSInteger)row
{
	if (row >= 0)
	{
		trackedRow = row;
		[self setNeedsDisplayInRect:[self rectOfRow:row]];
	}
	else
	{
		if (trackedRow >= 0)
			[self setNeedsDisplayInRect:[self rectOfRow:trackedRow]];
		trackedRow = -1;
	}
}

- (NSInteger)trackedRow
{
	return trackedRow;
}

- (void)removeTrackingAreas
{
	trackedRow = -1;
	for (NSTrackingArea *area in [self trackingAreas])
	{
		if (([area owner] == self) &&
			[[area userInfo] objectForKey:@"row"])
			[self removeTrackingArea:area];
	}
}

- (void)mouseDown:(NSEvent *) event
{
	NSPoint mouseLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	NSInteger row = [self rowAtPoint:mouseLocation];
	
	// Don't change selected rows when pushing a button
	if ((row != -1) && (trackedRow == row))
		forcedRow = [self selectedRow];
	
	[super mouseDown:event];
	
	forcedRow = -1;
}

- (void)mouseEntered:(NSEvent *)event
{
	NSDictionary *dict = [event userData];
	if ([dict objectForKey:@"row"])
		[self setTrackedRow:[[dict objectForKey:@"row"] integerValue]];
}

- (void)mouseExited:(NSEvent *)event
{
	[self setTrackedRow:-1];
}

- (void)updateTrackingAreas
{
	[super updateTrackingAreas];
	
	[self removeTrackingAreas];
	
	NSRange rows = [self rowsInRect:[self visibleRect]];
	NSPoint mouseLocation = [self convertPoint:[[self window] convertScreenToBase:
												[NSEvent mouseLocation]]
									  fromView:nil];
	for (NSInteger row = rows.location;
		 row < NSMaxRange(rows);
		 row++)
	{
		NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
							  [NSNumber numberWithInteger:row], @"row",
							  nil];
		EmulationOutlineCell *cell;
		cell = (EmulationOutlineCell *)[self preparedCellAtColumn:0 row:row];
		[cell addTrackingAreasForView:self
							   inRect:[self rectOfRow:row]
						 withUserInfo:dict
						mouseLocation:mouseLocation];
	}
}

@end
