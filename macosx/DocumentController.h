
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
#import "InspectorPanelController.h"

@interface DocumentController : NSDocumentController {
	NSUserDefaults *fDefaults;
	
	IBOutlet InspectorPanelController *fInspectorPanelController;
	
	NSArray *fileTypes;
	
	PaStream *portAudioStream;
	
	bool isTemplateChooserWindowOpen;
	int disableMenuBarCount;
}

- (id)openUntitledDocumentFromTemplateURL:(NSURL *)absoluteURL
								  display:(BOOL)displayDocument
									error:(NSError **)outError;
- (id)makeUntitledDocumentFromTemplateURL:(NSURL *)absoluteURL
									error:(NSError **)outError;

- (IBAction)newDocumentFromTemplateChooser:(id)sender;
- (void)noteTemplateChooserWindowClosed;

- (void)disableMenuBar;
- (void)enableMenuBar;

@end
