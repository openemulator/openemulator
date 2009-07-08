
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Manages documents.
 */

#import <Cocoa/Cocoa.h>

#import "portaudio/include/portaudio.h"
#import "InspectorPanelController.h"

@interface DocumentController : NSDocumentController {
	NSUserDefaults *fDefaults;
	IBOutlet InspectorPanelController *fInspectorPanelController;

	NSArray *fileTypes;

	PaStream *portAudioStream;
	
	bool isNewDocumentWindowOpen;
	
	int disableMenuBarCount;
}

- (id)openUntitledDocumentWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;
- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;
- (void)noteNewDocumentWindowClosed;
- (void)disableMenuBar;
- (void)enableMenuBar;

@end
