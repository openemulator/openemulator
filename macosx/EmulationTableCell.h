
/**
 * OpenEmulator
 * Mac OS X Emulation Table Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation table cell.
 */

#import <Cocoa/Cocoa.h>

@interface EmulationTableCell : NSActionCell
{
	void *devicesInfo;
}

- (void)setDevicesInfo:(void *)theDevicesInfo;

@end
