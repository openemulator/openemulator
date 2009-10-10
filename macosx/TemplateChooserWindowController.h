
/**
 * OpenEmulator
 * Mac OS X Template Chooser Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import <Cocoa/Cocoa.h>

#import "DocumentController.h"

@interface TemplateChooserWindowController : NSWindowController
{
	IBOutlet id outlineView;
	IBOutlet id imageBrowserView;
	
	DocumentController *documentController;
	
	NSMutableArray *groups;
}

- (id)init:(DocumentController *)theDocumentController;
- (void)performChoose:(id)sender;

@end
