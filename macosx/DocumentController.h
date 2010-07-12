
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
	
	NSArray *diskImageFileTypes;
	NSArray *audioFileTypes;
	
	NSURL *audioPlaybackURL;
	NSURL *audioRecordingURL;
	
	int disableMenuBarCount;
}

- (IBAction)newDocumentFromTemplateChooser:(id)sender;
- (id)openUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
								  display:(BOOL)displayDocument
									error:(NSError **)outError;
- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
									error:(NSError **)outError;

- (void)setPlaybackURL:(NSURL *)theURL;
- (void)togglePlayback;
- (BOOL)playback;
- (float)playbackTime;
- (float)playbackDuration;
- (NSURL *)playbackURL;

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
