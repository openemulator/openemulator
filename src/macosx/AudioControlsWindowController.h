
/**
 * OpenEmulator
 * Mac OS X Audio Controls Window Controller
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the audio controls window
 */

#import <Cocoa/Cocoa.h>

@class DocumentController;

@interface AudioControlsWindowController : NSWindowController
<NSWindowDelegate>
{
    IBOutlet DocumentController *fDocumentController;
    
    IBOutlet id fPlayNameLabel;
    IBOutlet id fTogglePlayButton;
    IBOutlet id fPlayPositionLabel;
    IBOutlet id fPlayPosition;
    IBOutlet id fPlayTimeLabel;
    
    IBOutlet id fSaveRecordingAsButton;
    IBOutlet id fToggleRecordingButton;
    IBOutlet id fRecordingTimeLabel;
    IBOutlet id fRecordingSizeLabel;
    
    NSTimer *timer;
    
    NSString *playPath;
    NSString *recordingPath;
}

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
