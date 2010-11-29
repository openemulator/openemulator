
/**
 * OpenEmulator
 * Mac OS X Hardware Chooser View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a hardware chooser view.
 */

#import <Cocoa/Cocoa.h>

#import "ChooserViewController.h"

@interface HardwareChooserViewController : ChooserViewController
{
	NSMutableArray *edlInfos;
}

- (void)updateForPorts:(NSArray *)ports;
- (NSMutableArray *)selectedItemConnectors;

@end
