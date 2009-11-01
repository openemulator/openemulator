
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
	IBOutlet id fPreviousButton;
	IBOutlet id fNextButton;
	
	DeviceChooserViewController *deviceChooserViewController;
	IBOutlet id fConnectView;
}

- (void) runModal:(id) sender;

- (IBAction) performCancel:(id) sender;
- (IBAction) performPrevious:(id) sender;
- (IBAction) performNext:(id) sender;

@end
