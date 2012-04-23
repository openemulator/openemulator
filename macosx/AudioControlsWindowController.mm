
/**
 * OpenEmulator
 * Mac OS X Audio Controls Window Controller
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the audio controls window
 */

#import "AudioControlsWindowController.h"

#import "NSStringAdditions.h"

#import "DocumentController.h"

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
        [fPlayPositionLabel setStringValue:@"--:--:--"];
        [fPlayTimeLabel setStringValue:@"--:--:--"];
    }
    else
    {
        NSString *path = [playPath lastPathComponent];
        [fPlayNameLabel setStringValue:path];
        [fPlayNameLabel setToolTip:path];
        
        float playPosition = paAudio->getPlayerPosition();
        float playTime = paAudio->getPlayerTime();
        NSString *positionLabel = [self formatTime:playPosition];
        NSString *timeLabel = [self formatTime:playTime];
        [fPlayPositionLabel setStringValue:positionLabel];
        [fPlayPosition setFloatValue:playPosition / playTime];
        [fPlayTimeLabel setStringValue:timeLabel];
    }
    
    BOOL isPlaying = paAudio->isPlayerPlaying();
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
    
    if (!paAudio->isPlayerPlaying())
        paAudio->startPlayer();
    else
        paAudio->pausePlayer();
}

- (IBAction)playPositionDidChange:(id)sender
{
    if (!playPath)
        return;
    
    PAAudio *paAudio = (PAAudio *)[fDocumentController paAudio];
    
    float time = [sender floatValue] * paAudio->getPlayerTime();
    paAudio->setPlayerPosition(time);
}

- (void)readFromPath:(NSString *)path
{
    [playPath release];
    
    PAAudio *paAudio = (PAAudio *)[fDocumentController paAudio];
    
    playPath = [path copy];
    if (playPath)
    {
        paAudio->openPlayer([playPath cppString]);
        paAudio->startPlayer();
        
        if (!paAudio->getPlayerTime())
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
        float recordingTime = paAudio->getRecorderTime();
        long long recordingSize = paAudio->getRecorderSize();
        NSString *timeLabel = [self formatTime:recordingTime];
        NSString *sizeLabel = [self formatSize:recordingSize];
        [fRecordingTimeLabel setStringValue:timeLabel];
        [fRecordingSizeLabel setStringValue:sizeLabel];
    }
    
    BOOL isRecording = paAudio->isRecorderRecording();
    [fToggleRecordingButton setImage:(isRecording ?
                                      [NSImage imageNamed:@"AudioStop.png"] :
                                      [NSImage imageNamed:@"AudioRecord.png"]
                                      )];
    [fSaveRecordingAsButton setEnabled:(recordingPath && !isRecording)];
}

- (IBAction)toggleRecording:(id)sender
{
    PAAudio *paAudio = (PAAudio *)[fDocumentController paAudio];
    
    if (!paAudio->isRecorderRecording())
    {
        NSString *path = [NSTemporaryDirectory()
                          stringByAppendingPathComponent:@"oerecording"];
        recordingPath = [path copy];
        
        paAudio->openRecorder([recordingPath cppString]);
        paAudio->startRecorder();
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
