
/**
 * OpenEmulator
 * Mac OS X Audio Controls Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the audio controls window.
 */

#import "AudioControlsWindowController.h"

@implementation AudioControlsWindowController

- (id)init
{
	self = [self initWithWindowNibName:@"AudioControls"];
	
	return self;
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"AudioControls"];
	
	timer = [NSTimer scheduledTimerWithTimeInterval:0.25
											 target:self
										   selector:@selector(timerDidExpire:)
										   userInfo:nil
											repeats:YES];
}

- (void)dealloc
{
	[super dealloc];

	[timer release];
}

- (BOOL)validateUserInterfaceItem:(id)item
{
    if ([item action] == @selector(toggleAudioControls:))
	{  
		NSString *menuTitle;
		if (![[self window] isVisible])
			menuTitle = NSLocalizedString(@"Show Audio Controls",
										  @"Title for menu item to show Audio Controls.");
		else
			menuTitle = NSLocalizedString(@"Hide Audio Controls",
										  @"Title for menu item to hide Audio Controls.");
		[item setTitleWithMnemonic:menuTitle];
    }
	
    return YES;
}

- (void)toggleAudioControls:(id)sender
{
	if ([[self window] isVisible])
		[[self window] orderOut:self];
	else
		[[self window] orderFront:self];
}

- (void)timerDidExpire:(NSTimer *)theTimer
{
	[self updatePlay];
	[self updateRecording];
}

- (IBAction)showAudioControls:(id)sender
{
    NSWindow *window = [self window];
	[window orderFront:sender];
}

- (NSString *)formatTime:(int)time
{
	return [NSString stringWithFormat:@"%02d:%02d:%02d",
			time / 3600,
			(time / 60) % 60,
			time % 60];
}

- (NSString *)formatSize:(long long)size
{
	if (size < 1e6)
		return [NSString stringWithFormat:@"%3.0f kB", size / 1E3];
	else if (size < 1e9)
		return [NSString stringWithFormat:@"%3.1f MB", size / 1E6];
	else
		return [NSString stringWithFormat:@"%3.1f GB", size / 1E9];
}

- (void)updatePlay
{
	NSURL *url = [fDocumentController playURL];
	if (!url)
	{
		[fPlayNameLabel setStringValue:@""];
		[fPlayTimeLabel setToolTip:@"--:--:--"];
		[fPlayDurationLabel setToolTip:@"--:--:--"];
	}
	else
	{
		NSString *path = [[url path] lastPathComponent];
		NSString *timeLabel = [self formatTime:[fDocumentController playTime]];
		NSString *durationLabel = [self formatTime:
								   [fDocumentController playDuration]];
		[fPlayNameLabel setStringValue:path];
		[fPlayNameLabel setToolTip:path];
		[fPlayTimeLabel setStringValue:timeLabel];
		[fPlayDurationLabel setStringValue:durationLabel];
	}
	
	BOOL isPlaying = [fDocumentController playing];
	[fOpenPlayButton setEnabled:!isPlaying];
	[fTogglePlayButton setEnabled:url ? YES : NO];
	[fTogglePlayButton setImage:(isPlaying ?
								 [NSImage imageNamed:@"AudioStop.png"] :
								 [NSImage imageNamed:@"AudioPlay.png"]
								 )];
}

- (IBAction)openPlay:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSArray *fileTypes = [[NSArray alloc] initWithObjects:
						  @"wav",
						  @"aiff", @"aif", @"aifc",
						  @"au",
						  @"flac",
						  @"caf",
						  @"ogg", @"oga",
						  nil];
	
	if ([panel runModalForTypes:fileTypes] == NSOKButton)
	{
		NSURL *url = [panel URL];
		[fDocumentController setPlayURL:url];
		
		[self updatePlay];
	}
}

- (IBAction)togglePlay:(id)sender
{
	[fDocumentController togglePlay];
}

- (void)updateRecording
{
	NSURL *url = [fDocumentController recordingURL];
	if (!url)
	{
		[fRecordingTimeLabel setStringValue:@"--:--:--"];
		[fRecordingSizeLabel setStringValue:@"- kB"];
		[fSaveRecordingAsButton setEnabled:NO];
	}
	else
	{
		NSString *timeLabel = [self formatTime:[fDocumentController recordingTime]];
		NSString *sizeLabel = [self formatSize:[fDocumentController recordingSize]];
		[fRecordingTimeLabel setStringValue:timeLabel];
		[fRecordingSizeLabel setStringValue:sizeLabel];
	}
	
	BOOL isRecording = [fDocumentController recording];
	[fToggleRecordingButton setImage:(isRecording ?
									  [NSImage imageNamed:@"AudioStop.png"] :
									  [NSImage imageNamed:@"AudioRecord.png"]
									  )];
	[fSaveRecordingAsButton setEnabled:(url && !isRecording)];
}

- (IBAction)toggleRecording:(id)sender
{
	[fDocumentController toggleRecording];
}

- (IBAction)saveRecording:(id)sender
{
	NSSavePanel *panel = [NSSavePanel savePanel];
	[panel setAllowedFileTypes:[NSArray arrayWithObject:@"wav"]];
	[panel setAllowsOtherFileTypes:NO];
	
	if ([panel runModal] == NSOKButton)
	{
		NSURL *url = [panel URL];
		[fDocumentController saveRecordingAs:url];
		
		[self updateRecording];
	}
}

@end
