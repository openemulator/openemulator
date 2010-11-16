
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls all emulations.
 */

#import "AudioControlsWindowController.h"

#import <Cocoa/Cocoa.h>

@interface DocumentController : NSDocumentController {
	IBOutlet id fInspectorWindowController;
	IBOutlet id fAudioControlsWindowController;
	IBOutlet id fTemplateChooserWindowController;
	
	void *oePortAudio;
	
	NSArray *diskImageFileTypes;
	NSArray *audioFileTypes;
	
	int disableMenuBarCount;
}

- (void)toggleInspector:(id)sender;
- (void)toggleAudioControls:(id)sender;

- (void *)getOEPortAudio;
- (NSArray *)getDiskImageFileTypes;
- (NSArray *)getAudioFileTypes;

- (IBAction)newDocumentFromTemplateChooser:(id)sender;
- (id)openUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
								  display:(BOOL)displayDocument
									error:(NSError **)outError;
- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
									error:(NSError **)outError;

- (BOOL)addEmulation:(void *)emulation;
- (void)removeEmulation:(void *)emulation;

- (void)disableMenuBar;
- (void)enableMenuBar;

- (IBAction)openHomepage:(id)sender;
- (IBAction)openForums:(id)sender;
- (IBAction)openDevelopment:(id)sender;
- (IBAction)openDonate:(id)sender;

@end
