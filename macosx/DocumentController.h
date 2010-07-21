
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls emulations.
 */

#import <Cocoa/Cocoa.h>

@interface DocumentController : NSDocumentController {
	IBOutlet id fTemplateChooserController;
	IBOutlet id fInspectorController;
	
	void *oepa;
	
	NSArray *diskImageFileTypes;
	NSArray *audioFileTypes;
	
	NSURL *audioPlayURL;
	NSURL *audioRecordingURL;
	
	int disableMenuBarCount;
}

- (void *)oepa;

- (IBAction)newDocumentFromTemplateChooser:(id)sender;
- (id)openUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
								  display:(BOOL)displayDocument
									error:(NSError **)outError;
- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
									error:(NSError **)outError;

- (BOOL)addEmulation:(void *)emulation;
- (void)removeEmulation:(void *)emulation;

- (void)setPlayURL:(NSURL *)theURL;
- (void)togglePlay;
- (BOOL)playing;
- (float)playTime;
- (float)playDuration;
- (NSURL *)playURL;

- (void)toggleRecording;
- (void)saveRecordingAs:(NSURL *)theURL;
- (BOOL)recording;
- (float)recordingTime;
- (long long)recordingSize;
- (NSURL *)recordingURL;

- (void)disableMenuBar;
- (void)enableMenuBar;

- (IBAction)openHomepage:(id)sender;
- (IBAction)openForums:(id)sender;
- (IBAction)openDevelopment:(id)sender;
- (IBAction)openDonate:(id)sender;

@end
