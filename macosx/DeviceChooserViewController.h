
/**
 * OpenEmulator
 * Mac OS X Device Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a device chooser view controller.
 */

#import <Cocoa/Cocoa.h>

#import "ChooserViewController.h"

@interface DeviceChooserViewController : ChooserViewController
{

}

- (void) loadDevicesFromPath:(NSString *) path;

@end
