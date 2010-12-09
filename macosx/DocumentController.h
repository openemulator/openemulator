
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls all emulations.
 */

#import <Cocoa/Cocoa.h>

#import "InspectorWindowController.h"
#import "AudioControlsWindowController.h"
#import "TemplateChooserWindowController.h"

@class AudioControlsWindowController;
@class TemplateChooserWindowController;

@interface DocumentController : NSDocumentController {
	IBOutlet InspectorWindowController *fInspectorWindowController;
	IBOutlet AudioControlsWindowController *fAudioControlsWindowController;
	IBOutlet TemplateChooserWindowController *fTemplateChooserWindowController;
	
	NSArray *diskImagePathExtensions;
	NSArray *audioPathExtensions;
	
	void *oePortAudio;
	
	int disableMenuBarCount;
}

- (NSArray *)diskImagePathExtensions;
- (NSArray *)audioPathExtensions;
- (void *)oePortAudio;

- (void)toggleInspector:(id)sender;
- (void)toggleAudioControls:(id)sender;
- (IBAction)newDocumentFromTemplateChooser:(id)sender;

- (id)openUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
								  display:(BOOL)displayDocument
									error:(NSError **)outError;
- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
									error:(NSError **)outError;

- (void)disableMenuBar;
- (void)enableMenuBar;

- (IBAction)openHomepage:(id)sender;
- (IBAction)openForums:(id)sender;
- (IBAction)openDevelopment:(id)sender;
- (IBAction)openDonate:(id)sender;

@end
