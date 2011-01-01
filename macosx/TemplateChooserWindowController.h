
/**
 * OpenEmulator
 * Mac OS X Template Chooser Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import <Cocoa/Cocoa.h>

#import "DocumentController.h"
#import "TemplateChooserViewController.h"

@interface TemplateChooserWindowController : NSWindowController
{
	id fDocumentController;
	
	id fTemplateChooserView;
	id fChooseButton;
	TemplateChooserViewController *templateChooserViewController;
}

- (void)run;
- (IBAction)chooseTemplate:(id)sender;

@end
