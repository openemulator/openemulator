
/**
 * OpenEmulator
 * Mac OS X Audio Controls Window Controller
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the audio controls window
 */

#import "AudioControlsWindowController.h"
#import "DocumentController.h"
#import "StringConversion.h"

#import "PAAudio.h"

@implementation AudioControlsWindowController

- (id)init
{
    self = [super initWithWindowNibName:@"AudioControls"];
    
    return self;
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



- (void)windowDidLoad
{
    [self setWindowFrameAutosaveName:@"AudioControls"];
    
    [[self window] registerForDraggedTypes:[NSArray arrayWithObjects:
                                            NSFilenamesPboardType,
                                            nil]];
    [[self window] setDelegate:self];
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.25
                                             target:self
                                           selector:@selector(timerDidExpire:)
                                           userInfo:nil
                                            repeats:YES];
}

- (void)timerDidExpire:(NSTimer *)theTimer
{
    [self updatePlay];
    [self updateRecording];
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
    PAAudio *paAudio = (PAAudio *)[fDocumentController paAudio];
    
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
        
        float playTime = paAudio->getPlayTime();
        float playDuration = paAudio->getPlayDuration();
        NSString *timeLabel = [self formatTime:playTime];
        NSString *durationLabel = [self formatTime:playDuration];
        [fPlayPosition setFloatValue:playTime / playDuration];
        [fPlayTimeLabel setStringValue:timeLabel];
        [fPlayDurationLabel setStringValue:durationLabel];
    }
    
    BOOL isPlaying = paAudio->isPlaying();
    [fTogglePlayButton setEnabled:playPath ? YES : NO];
    [fTogglePlayButton setImage:(isPlaying ?
                                 [NSImage imageNamed:@"AudioPause.png"] :
                                 [NSImage imageNamed:@"AudioPlay.png"]
                                 )];
}

- (IBAction)openPlay:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    
    [panel setAllowedFileTypes:[fDocumentController audioPathExtensions]];
    if ([panel runModal] == NSOKButton)
    {
        NSString *path = [[panel URL] path];
        [self readFromPath:path];
    }
}

- (IBAction)togglePlay:(id)sender
{
    PAAudio *paAudio = (PAAudio *)[fDocumentController paAudio];
    
    if (!paAudio->isPlaying())
        paAudio->play();
    else
        paAudio->pause();
}

- (IBAction)playPositionDidChange:(id)sender
{
    if (!playPath)
        return;
    
    PAAudio *paAudio = (PAAudio *)[fDocumentController paAudio];
    
    float time = [sender floatValue] * paAudio->getPlayDuration();
    paAudio->setPlayPosition(time);
}

- (void)readFromPath:(NSString *)path
{
    [playPath release];
    
    PAAudio *paAudio = (PAAudio *)[fDocumentController paAudio];
    
    playPath = [path copy];
    if (playPath)
    {
        paAudio->openPlayer(getCPPString(playPath));
        paAudio->play();
        
        if (!paAudio->getPlayDuration())
        {
            paAudio->closePlayer();
            
            [playPath release];
            playPath = nil;
        }
    }
    
    [self updatePlay];
}

- (void)updateRecording
{
    PAAudio *paAudio = (PAAudio *)[fDocumentController paAudio];
    
    if (!recordingPath)
    {
        [fRecordingTimeLabel setStringValue:@"--:--:--"];
        [fRecordingSizeLabel setStringValue:@"- kB"];
        [fSaveRecordingAsButton setEnabled:NO];
    }
    else
    {
        float recordingTime = paAudio->getRecordingTime();
        long long recordingSize = paAudio->getRecordingSize();
        NSString *timeLabel = [self formatTime:recordingTime];
        NSString *sizeLabel = [self formatSize:recordingSize];
        [fRecordingTimeLabel setStringValue:timeLabel];
        [fRecordingSizeLabel setStringValue:sizeLabel];
    }
    
    BOOL isRecording = paAudio->isRecording();
    [fToggleRecordingButton setImage:(isRecording ?
                                      [NSImage imageNamed:@"AudioStop.png"] :
                                      [NSImage imageNamed:@"AudioRecord.png"]
                                      )];
    [fSaveRecordingAsButton setEnabled:(recordingPath && !isRecording)];
}

- (IBAction)toggleRecording:(id)sender
{
    PAAudio *paAudio = (PAAudio *)[fDocumentController paAudio];
    
    if (!paAudio->isRecording())
    {
        NSString *path = [NSTemporaryDirectory()
                          stringByAppendingPathComponent:@"oerecording"];
        recordingPath = [path copy];
        
        paAudio->openRecorder(getCPPString(recordingPath));
        paAudio->record();
    }
    else
        paAudio->closeRecorder();
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
                                                  @"Audio Controls.");
        NSString *informativeText = NSLocalizedString(@"Try saving the document to another volume.",
                                                      @"Audio Controls.");
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
