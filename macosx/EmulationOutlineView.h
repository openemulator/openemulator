
/**
 * OpenEmulator
 * Mac OS X Trackable Outline View
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an trackable outline view.
 */

#import <Cocoa/Cocoa.h>

@interface EmulationOutlineView : NSOutlineView
{
	int forcedRow;
	int trackedRow;
}

- (NSInteger)forcedRow;
- (void)setTrackedRow:(NSInteger)row;
- (NSInteger)trackedRow;
- (void)removeTrackingAreas;

@end
