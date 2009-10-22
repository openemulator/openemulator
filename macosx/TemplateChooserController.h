
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
#import "ChooserController.h"

@interface TemplateChooserController : NSWindowController
{
	IBOutlet id fDocumentController;
	
	IBOutlet id fChooserView;
	ChooserController *chooserController;
}

- (void) updateTemplates;
- (IBAction) chooseTemplate:(id) sender;

@end
