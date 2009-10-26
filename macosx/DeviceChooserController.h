
/**
 * OpenEmulator
 * Mac OS X Template Chooser Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser.
 */

#import <Cocoa/Cocoa.h>

#import "DocumentController.h"
#import "DeviceChooserViewController.h"

@interface DeviceChooserController : NSWindowController
{
	IBOutlet id fMessage;
	IBOutlet id fChooserView;
	DeviceChooserViewController *deviceChooserViewController;
	IBOutlet id fConnectView;
}

- (IBAction) chooseTemplate:(id) sender;

@end
