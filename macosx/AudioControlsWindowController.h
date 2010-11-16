
/**
 * OpenEmulator
 * Mac OS X Audio Controls Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the audio controls window.
 */

#import <Cocoa/Cocoa.h>

@interface AudioControlsWindowController : NSWindowController
{
	IBOutlet id fDocumentController;
	
	IBOutlet id fPlayNameLabel;
	IBOutlet id fPlayTimeLabel;
	IBOutlet id fPlayDurationLabel;
	IBOutlet id fOpenPlayButton;
	IBOutlet id fTogglePlayButton;
	
	IBOutlet id fRecordingTimeLabel;
	IBOutlet id fRecordingSizeLabel;
	IBOutlet id fToggleRecordingButton;
	IBOutlet id fSaveRecordingAsButton;
	
	NSTimer *timer;
	
	void *oePortAudio;
	
	NSURL *playURL;
	NSURL *recordingURL;
}

- (IBAction)toggleAudioControls:(id)sender;

- (void) updatePlay;
- (IBAction)openPlay:(id)sender;
- (IBAction)togglePlay:(id)sender;

- (void) updateRecording;
- (IBAction)toggleRecording:(id)sender;
- (IBAction)saveRecording:(id)sender;

- (void)readFromURL:(NSURL *)theURL;
- (void)writeToURL:(NSURL *)theURL;

@end
