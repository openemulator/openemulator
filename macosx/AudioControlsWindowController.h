
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
	DocumentController *fDocumentController;
	
	id fPlayNameLabel;
	id fTogglePlayButton;
	id fPlayTimeLabel;
	id	fPlayPosition;
	id fPlayDurationLabel;
	
	id fSaveRecordingAsButton;
	id fToggleRecordingButton;
	id fRecordingTimeLabel;
	id fRecordingSizeLabel;
	
	NSTimer *timer;
	
	NSString *playPath;
	NSString *recordingPath;
}

- (IBAction)toggleAudioControls:(id)sender;

- (void) updatePlay;
- (IBAction)openPlay:(id)sender;
- (IBAction)togglePlay:(id)sender;
- (IBAction)playPositionDidChange:(id)sender;
- (void)readFromPath:(NSString *)path;

- (void) updateRecording;
- (IBAction)toggleRecording:(id)sender;
- (IBAction)saveRecording:(id)sender;
- (void)writeToPath:(NSString *)path;

@end
