
/**
 * OpenEmulator
 * Mac OS X Device Chooser Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the device chooser window.
 */

#import <Cocoa/Cocoa.h>

#import "DeviceChooserViewController.h"
#import "ConnectorViewController.h"

@interface DeviceChooserController : NSWindowController
{
	IBOutlet id fMessage;
	IBOutlet id fView;
	IBOutlet id fPreviousButton;
	IBOutlet id fNextButton;
	
	IBOutlet id fDocumentController;
	
	DeviceChooserViewController *deviceChooserViewController;
	ConnectorViewController *connectorViewController;
	
	NSArray *selectedItemOutlets;
	NSMutableArray *selectedItemInlets;
	
	id currentView;
	int currentStep;
}

- (void) runModal:(id) sender;

- (IBAction) performCancel:(id) sender;
- (IBAction) performPrevious:(id) sender;
- (IBAction) performNext:(id) sender;

@end
