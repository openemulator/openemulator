
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
#import "TemplateChooser.h"

@interface TemplateChooserController : NSWindowController
{
	id fDocumentController;
	
	IBOutlet id fOutlineView;
	IBOutlet id fChooserView;
	
	TemplateChooser *templateChooser;
}

- (void) updateTemplate;
- (IBAction) chooseTemplate:(id) sender;

@end
