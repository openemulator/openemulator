
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Manages documents.
 */

#import <Cocoa/Cocoa.h>

#import "portaudio.h"
#import "TemplateChooserController.h"
#import "InspectorController.h"

#define LINK_HOMEPAGE	@"http://www.openemulator.org"
#define LINK_FORUMSURL	@"http://groups.google.com/group/openemulator"
#define LINK_DEVURL		@"http://code.google.com/p/openemulator"
#define LINK_DONATEURL	@"http://www.openemulator.org/donate.html"

@interface DocumentController : NSDocumentController {
	IBOutlet id fTemplateChooserController;
	IBOutlet id fInspectorController;
	
	NSArray *fileTypes;
	
	PaStream *portAudioStream;
	
	int disableMenuBarCount;
}

- (IBAction) newDocumentFromTemplateChooser:(id) sender;
- (id) openUntitledDocumentFromTemplateURL:(NSURL *) absoluteURL
								   display:(BOOL) displayDocument
									 error:(NSError **) outError;
- (id) makeUntitledDocumentFromTemplateURL:(NSURL *) absoluteURL
									 error:(NSError **) outError;

- (void) disableMenuBar;
- (void) enableMenuBar;

- (IBAction) linkHomepage:(id) sender;
- (IBAction) linkForums:(id) sender;
- (IBAction) linkDevelopment:(id) sender;
- (IBAction) linkDonate:(id) sender;

@end
