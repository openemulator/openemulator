
/**
 * OpenEmulator
 * Mac OS X Template Chooser Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import <Cocoa/Cocoa.h>

#import "DocumentController.h"
#import "TemplateChooserViewController.h"

@interface TemplateChooserController : NSWindowController
{
	IBOutlet id fDocumentController;
	
	IBOutlet id fTemplateChooserView;
	IBOutlet id fChooseButton;
	TemplateChooserViewController *templateChooserViewController;
}

- (IBAction) chooseTemplate:(id) sender;

@end
