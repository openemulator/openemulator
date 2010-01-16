
/**
 * OpenEmulator
 * Mac OS X Device Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device chooser view.
 */

#import <Cocoa/Cocoa.h>

#import "ChooserViewController.h"

@interface DeviceChooserViewController : ChooserViewController
{
	NSMutableArray *deviceInfos;
}

- (void) updateForInlets:(NSMutableArray *)inlets;
- (NSMutableArray *)selectedItemOutlets;

@end
