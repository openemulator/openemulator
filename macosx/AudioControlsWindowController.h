
/**
 * OpenEmulator
 * Mac OS X Audio Controls Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the audio controls window.
 */

#import <Cocoa/Cocoa.h>

#import "DocumentController.h"

@class DocumentController;

@interface AudioControlsWindowController : NSWindowController
{
	IBOutlet DocumentController *fDocumentController;
	
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
	
	NSString *playPath;
	NSString *recordingPath;
}

- (IBAction)toggleAudioControls:(id)sender;

- (void) updatePlay;
- (IBAction)openPlay:(id)sender;
- (IBAction)togglePlay:(id)sender;
- (void)readFromPath:(NSString *)path;

- (void) updateRecording;
- (IBAction)toggleRecording:(id)sender;
- (IBAction)saveRecording:(id)sender;
- (void)writeToPath:(NSString *)path;

@end
