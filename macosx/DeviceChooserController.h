
/**
 * OpenEmulator
 * Mac OS X Device Chooser Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the device chooser window.
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
	
	IBOutlet DocumentController *fDocumentController;
	IBOutlet id fConnectView;
	
	DeviceChooserViewController *deviceChooserViewController;
}

- (void) runModal:(id) sender;

- (IBAction) performCancel:(id) sender;
- (IBAction) performPrevious:(id) sender;
- (IBAction) performNext:(id) sender;

@end
