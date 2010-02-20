
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls emulations.
 */

#import <Cocoa/Cocoa.h>

#import "InspectorController.h"
#import "TemplateChooserController.h"

#define LINK_HOMEPAGE	@"http://www.openemulator.org"
#define LINK_FORUMSURL	@"http://groups.google.com/group/openemulator"
#define LINK_DEVURL		@"http://code.google.com/p/openemulator"
#define LINK_DONATEURL	@"http://www.openemulator.org/donate.html"

@interface DocumentController : NSDocumentController {
	IBOutlet id fTemplateChooserController;
	IBOutlet id fInspectorController;
	
	NSArray *fileTypes;
	
	int disableMenuBarCount;
	
	BOOL fullDuplexState;
}

- (IBAction) newDocumentFromTemplateChooser:(id) sender;
- (id) openUntitledDocumentWithTemplateURL:(NSURL *) absoluteURL
								   display:(BOOL) displayDocument
									 error:(NSError **) outError;
- (id) makeUntitledDocumentWithTemplateURL:(NSURL *) absoluteURL
									 error:(NSError **) outError;

- (void) disableMenuBar;
- (void) enableMenuBar;

- (IBAction) linkHomepage:(id) sender;
- (IBAction) linkForums:(id) sender;
- (IBAction) linkDevelopment:(id) sender;
- (IBAction) linkDonate:(id) sender;

@end
