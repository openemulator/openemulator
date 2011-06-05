
/**
 * OpenEmulator
 * Mac OS X Template Chooser Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import <Cocoa/Cocoa.h>

@class TemplateChooserViewController;

@interface TemplateChooserWindowController : NSWindowController
<NSWindowDelegate>
{
	IBOutlet id fDocumentController;
	
	IBOutlet id fTemplateChooserView;
	IBOutlet id fChooseButton;
	
	TemplateChooserViewController *templateChooserViewController;
}

- (void)run;
- (IBAction)chooseTemplate:(id)sender;

@end
