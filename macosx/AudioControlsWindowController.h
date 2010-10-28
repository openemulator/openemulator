
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

@interface AudioControlsWindowController : NSWindowController
{
	IBOutlet DocumentController *fDocumentController;
	
	IBOutlet NSTextField *fPlayNameLabel;
	IBOutlet NSTextField *fPlayTimeLabel;
	IBOutlet NSTextField *fPlayDurationLabel;
	IBOutlet NSButton *fOpenPlayButton;
	IBOutlet NSButton *fTogglePlayButton;
	
	IBOutlet NSTextField *fRecordingTimeLabel;
	IBOutlet NSTextField *fRecordingSizeLabel;
	IBOutlet NSButton *fToggleRecordingButton;
	IBOutlet NSButton *fSaveRecordingAsButton;
	
	NSTimer *timer;
}

- (IBAction)toggleAudioControls:(id)sender;

- (void)updatePlay;
- (IBAction)openPlay:(id)sender;
- (IBAction)togglePlay:(id)sender;

- (void)updateRecording;
- (IBAction)toggleRecording:(id)sender;
- (IBAction)saveRecording:(id)sender;

@end
