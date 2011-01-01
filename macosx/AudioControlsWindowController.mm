
/**
 * OpenEmulator
 * Mac OS X Audio Controls Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the audio controls window.
 */

#import "AudioControlsWindowController.h"
#import "StringConversion.h"

#import "OEPortAudio.h"

@implementation AudioControlsWindowController

- (id)init
{
	self = [self initWithWindowNibName:@"AudioControls"];
	
	return self;
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"AudioControls"];
	
	[[self window] registerForDraggedTypes:[NSArray arrayWithObjects:
											NSFilenamesPboardType,
											nil]];
	
	timer = [NSTimer scheduledTimerWithTimeInterval:0.25
											 target:self
										   selector:@selector(timerDidExpire:)
										   userInfo:nil
											repeats:YES];
}

- (void)dealloc
{
	[playPath release];
	
	if (recordingPath)
	{
		NSError *error;
		
		[[NSFileManager defaultManager] removeItemAtPath:recordingPath
												   error:&error];
	}
	[recordingPath release];
	
	[timer release];
	
	[[self window] unregisterDraggedTypes];
	
	[super dealloc];
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

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
	NSPasteboard *pasteboard = [sender draggingPasteboard];
	
	if ([[pasteboard types] containsObject:NSFilenamesPboardType])
	{
		NSString *path = [[pasteboard propertyListForType:NSFilenamesPboardType]
						  objectAtIndex:0];
		NSString *pathExtension = [[path pathExtension] lowercaseString];
		
		if ([[fDocumentController audioPathExtensions] containsObject:pathExtension])
			return NSDragOperationCopy;
	}
	return NSDragOperationNone;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
	NSPasteboard *pasteboard = [sender draggingPasteboard];
	if ([[pasteboard types] containsObject:NSFilenamesPboardType])
	{
		NSString *path = [[pasteboard propertyListForType:NSFilenamesPboardType]
						  objectAtIndex:0];
		NSString *pathExtension = [[path pathExtension] lowercaseString];
		
		if ([[fDocumentController audioPathExtensions] containsObject:pathExtension])
		{
			[self readFromPath:path];
			
			return YES;
		}
	}
	
	return NO;
}

- (void)timerDidExpire:(NSTimer *)theTimer
{
	[self updatePlay];
	[self updateRecording];
}

- (NSString *)formatTime:(NSInteger)time
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
	OEPortAudio *oePortAudio = (OEPortAudio *)[fDocumentController oePortAudio];
	
	if (!playPath)
	{
		[fPlayNameLabel setStringValue:@""];
		[fPlayPosition setFloatValue:0.0];
		[fPlayTimeLabel setStringValue:@"--:--:--"];
		[fPlayDurationLabel setStringValue:@"--:--:--"];
	}
	else
	{
		NSString *path = [playPath lastPathComponent];
		[fPlayNameLabel setStringValue:path];
		[fPlayNameLabel setToolTip:path];
		
		float playTime = oePortAudio->getPlayTime();
		float playDuration = oePortAudio->getPlayDuration();
		NSString *timeLabel = [self formatTime:playTime];
		NSString *durationLabel = [self formatTime:playDuration];
		[fPlayPosition setFloatValue:playTime / playDuration];
		[fPlayTimeLabel setStringValue:timeLabel];
		[fPlayDurationLabel setStringValue:durationLabel];
	}
	
	BOOL isPlaying = oePortAudio->isPlaying();
	[fTogglePlayButton setEnabled:playPath ? YES : NO];
	[fTogglePlayButton setImage:(isPlaying ?
								 [NSImage imageNamed:@"AudioPause.png"] :
								 [NSImage imageNamed:@"AudioPlay.png"]
								 )];
}

- (IBAction)openPlay:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	if ([panel runModalForTypes:[fDocumentController audioPathExtensions]] ==
		NSOKButton)
	{
		NSString *path = [[panel URL] path];
		[self readFromPath:path];
	}
}

- (IBAction)togglePlay:(id)sender
{
	OEPortAudio *oePortAudio = (OEPortAudio *)[fDocumentController oePortAudio];
	
	if (!oePortAudio->isPlaying())
		oePortAudio->play();
	else
		oePortAudio->pause();
}

- (IBAction)playPositionDidChange:(id)sender
{
	if (!playPath)
		return;
	
	OEPortAudio *oePortAudio = (OEPortAudio *)[fDocumentController oePortAudio];
	
	float time = [sender floatValue] * oePortAudio->getPlayDuration();
	oePortAudio->setPlayPosition(time);
}

- (void)readFromPath:(NSString *)path
{
	[playPath release];
	
	OEPortAudio *oePortAudio = (OEPortAudio *)[fDocumentController oePortAudio];
	
	playPath = [path copy];
	if (playPath)
	{
		oePortAudio->openPlayer(getCPPString(playPath));
		oePortAudio->play();
		
		if (!oePortAudio->getPlayDuration())
		{
			oePortAudio->closePlayer();
			
			[playPath release];
			playPath = nil;
		}
	}
	
	[self updatePlay];
}

- (void)updateRecording
{
	OEPortAudio *oePortAudio = (OEPortAudio *)[fDocumentController oePortAudio];
	
	if (!recordingPath)
	{
		[fRecordingTimeLabel setStringValue:@"--:--:--"];
		[fRecordingSizeLabel setStringValue:@"- kB"];
		[fSaveRecordingAsButton setEnabled:NO];
	}
	else
	{
		float recordingTime = oePortAudio->getRecordingTime();
		long long recordingSize = oePortAudio->getRecordingSize();
		NSString *timeLabel = [self formatTime:recordingTime];
		NSString *sizeLabel = [self formatSize:recordingSize];
		[fRecordingTimeLabel setStringValue:timeLabel];
		[fRecordingSizeLabel setStringValue:sizeLabel];
	}
	
	BOOL isRecording = oePortAudio->isRecording();
	[fToggleRecordingButton setImage:(isRecording ?
									  [NSImage imageNamed:@"AudioStop.png"] :
									  [NSImage imageNamed:@"AudioRecord.png"]
									  )];
	[fSaveRecordingAsButton setEnabled:(recordingPath && !isRecording)];
}

- (IBAction)toggleRecording:(id)sender
{
	OEPortAudio *oePortAudio = (OEPortAudio *)[fDocumentController oePortAudio];
	
	if (!oePortAudio->isRecording())
	{
		NSString *path = [NSTemporaryDirectory()
						  stringByAppendingPathComponent:@"oerecording"];
		recordingPath = [path copy];
		
		oePortAudio->openRecorder(getCPPString(recordingPath));
		oePortAudio->record();
	}
	else
		oePortAudio->closeRecorder();
}

- (IBAction)saveRecording:(id)sender
{
	NSSavePanel *panel = [NSSavePanel savePanel];
	[panel setAllowedFileTypes:[NSArray arrayWithObject:@"wav"]];
	[panel setAllowsOtherFileTypes:NO];
	
	if ([panel runModal] == NSOKButton)
	{
		NSString *path = [[panel URL] path];
		[self writeToPath:path];
	}
}

- (void)writeToPath:(NSString *)path
{
	if (!recordingPath)
		return;
	
	NSError *error;
	
	[[NSFileManager defaultManager] removeItemAtPath:path
											   error:&error];
	if (![[NSFileManager defaultManager] moveItemAtPath:recordingPath
												 toPath:path
												  error:&error])
	{
		NSString *messageText = NSLocalizedString(@"The document could not be saved.",
												  @"The document could not be saved.");
		NSString *informativeText = NSLocalizedString(@"Try saving the file to another volume.",
													  @"Try saving the file to another volume.");
		NSAlert *alert = [NSAlert alertWithMessageText:messageText
										 defaultButton:nil
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:informativeText];
		[alert runModal];
		
		return;
	}
	
	[recordingPath release];
	recordingPath = nil;
}

@end
