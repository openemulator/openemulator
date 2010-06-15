
/**
 * OpenEmulator
 * Mac OS X Inspector Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the inspector window.
 */

#import "InspectorController.h"
#import "Document.h"

NSString *itemIdentifiers[] = 
{
	@"Emulation",
	@"Expansions",
	@"Storage",
	@"Peripherals",
	@"Audio",
};

@implementation InspectorController

- (id)init
{
	if (self = [super initWithWindowNibName:@"Inspector"])
	{
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(toggleInspector:)
													 name:@"toggleInspector"
												   object:nil];
		
		selectedViewIndex = -1;
	}
	
	return self;
}

- (void) windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Inspector"];
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	int viewIndex = [defaults integerForKey:@"OEInspectorSelectedViewIndex"];
 	[fTabMatrix selectCellWithTag:viewIndex];
	[self setView:viewIndex];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(inspectorDidResignKey:)
												 name:NSWindowDidResignKeyNotification
											   object:[self window]];
	[self activeDocumentDidChange];
    [NSApp addObserver:self
			forKeyPath:@"mainWindow.windowController.document"
			   options:0
			   context:[InspectorController class]];
	
	[NSTimer scheduledTimerWithTimeInterval:0.25
									 target:self
								   selector:@selector(timerDidExpire:)
								   userInfo:nil
									repeats:YES];
}

- (void) observeValueForKeyPath:(NSString *) keyPath
					   ofObject:(id) object
						 change:(NSDictionary *) change
						context:(void *) context
{
    if (context == [InspectorController class])
		[self activeDocumentDidChange];
	else
		[super observeValueForKeyPath:keyPath
							 ofObject:object
							   change:change
							  context:context];
}

- (void) activeDocumentDidChange
{
    id activeDocument = [[[NSApp mainWindow] windowController] document];
    if (activeDocument != inspectedDocument)
	{
		if (inspectedDocument)
			[fDocumentObjectController commitEditing];
		
		self.inspectedDocument = (activeDocument && [activeDocument
													 isKindOfClass:[Document class]
													 ]) ? activeDocument : nil;   
    }
}

- (void) inspectorDidResignKey:(NSNotification *) notification
{
    [fDocumentObjectController commitEditing];
}

- (void) timerDidExpire:(NSTimer *) theTimer
{
	[self updatePlayback];
	[self updateRecording];
}

- (id) inspectedDocument
{
	return inspectedDocument;
}

- (void) setInspectedDocument:(id) value
{
	inspectedDocument = value;
}

- (void) toggleInspector:(id) sender
{
    NSWindow *window = [self window];
    if ([window isVisible])
		[window orderOut:sender];
	else
		[window orderFront:sender];
}

- (BOOL) validateUserInterfaceItem:(id) item
{
    if ([item action] == @selector(toggleInspector:))
	{  
		NSString *menuTitle;
		if (![[self window] isVisible])
			menuTitle = NSLocalizedString(@"Show Inspector",
										  @"Title for menu item to show the Inspector "
										  "(should be the same as the initial menu item "
										  "in the nib).");
		else
			menuTitle = NSLocalizedString(@"Hide Inspector",
										  @"Title for menu item to hide the Inspector.");
		[item setTitleWithMnemonic:menuTitle];
    }
	
    return YES;
}

- (NSView *) getView:(int) viewIndex
{
	switch (viewIndex)
	{
		case 0:
			return fEmulationView;
		case 1:
			return fExpansionsView;
		case 2:
			return fStorageView;
		case 3:
			return fPeripheralsView;
		case 4:
			return fAudioView;
	}
	
	return fEmulationView;
}

- (void) selectView:(id) sender
{
	[self setView:[fTabMatrix selectedTag]];
}

- (void) setView:(int) viewIndex
{
	if (selectedViewIndex == viewIndex)
		return;
	
	float selectedHeight = 0;
	if (selectedViewIndex >= 0)
	{
		NSView *selectedView = [self getView:selectedViewIndex];
		selectedHeight = [selectedView frame].size.height;
		
		[selectedView setHidden:YES];
		[selectedView removeFromSuperview];
	}
	
	NSWindow *window = [self window];
	NSRect frame = [window frame];
	
	NSView *view = [self getView:viewIndex];
	float difference = ([view frame].size.height - selectedHeight) *
	[window userSpaceScaleFactor];
	if (selectedViewIndex >= 0)
		frame.origin.y -= difference;
	frame.size.height += difference;
	
    [window setFrame:frame display:YES animate:YES];
	[[window contentView] addSubview:view];
    [view setHidden:NO];
    
	[window setTitle:NSLocalizedString(itemIdentifiers[viewIndex], "")];
	
	selectedViewIndex = viewIndex;
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setInteger:selectedViewIndex
				  forKey:@"OEInspectorSelectedViewIndex"];
}

- (void) restoreWindowState:(id) sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	if ([defaults boolForKey:@"OEInspectorIsVisible"])
		[self showWindow:self];	
}

- (void) storeWindowState:(id) sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setBool:[[self window] isVisible]
			   forKey:@"OEInspectorIsVisible"];
}

- (void) addExpansion:(id) sender
{
	[fDeviceChooserController runModal:self forCategory:@"expansion"];
}

- (void) removeExpansion:(id) sender
{
	[self removeDevice:[[fExpansionsController selectedObjects] objectAtIndex:0]];
}

- (void) addStorage:(id) sender
{
	[fDeviceChooserController runModal:self forCategory:@"storage"];
}

- (void) removeStorage:(id) sender
{
	[self removeDevice:[[fStorageController selectedObjects] objectAtIndex:0]];
}

- (void) addPeripheral:(id) sender
{
	[fDeviceChooserController runModal:self forCategory:@"peripheral"];
}

- (void) removePeripheral:(id) sender
{
	[self removeDevice:[[fPeripheralsController selectedObjects] objectAtIndex:0]];
}

- (void) removeDevice:(NSDictionary *) dict 
{
	[inspectedDocument removeDevice:dict];
}

- (NSString *) formatTime:(int) time
{
	return [NSString stringWithFormat:@"%02d:%02d:%02d",
			time / 3600,
			(time / 60) % 60,
			time % 60];
}

- (NSString *) formatSize:(long long) size
{
	if (size < 1e6)
		return [NSString stringWithFormat:@"%3.0f kB", size / 1000.0];
	else if (size < 1e9)
		return [NSString stringWithFormat:@"%3.1f MB", size / 1000000.0];
	else
		return [NSString stringWithFormat:@"%3.1f GB", size / 1000000000.0];
}

- (void) updateState
{
	
	
}

- (void) updatePlayback
{
	NSURL *url = [fDocumentController playbackURL];
	if (!url)
	{
		[fPlaybackNameLabel setStringValue:@""];
		[fPlaybackTimeLabel setToolTip:@"--:--:--"];
		[fPlaybackDurationLabel setToolTip:@"--:--:--"];
	}
	else
	{
		NSString *path = [[url path] lastPathComponent];
		NSString *timeLabel = [self formatTime:[fDocumentController playbackTime]];
		NSString *durationLabel = [self formatTime:
								   [fDocumentController playbackDuration]];
		[fPlaybackNameLabel setStringValue:path];
		[fPlaybackNameLabel setToolTip:path];
		[fPlaybackTimeLabel setStringValue:timeLabel];
		[fPlaybackDurationLabel setStringValue:durationLabel];
	}

	BOOL isPlayback = [fDocumentController playback];
	[fOpenPlaybackButton setEnabled:!isPlayback];
	[fTogglePlaybackButton setEnabled:url ? YES : NO];
	[fTogglePlaybackButton setImage:(isPlayback ?
									 [NSImage imageNamed:@"IPAudioStop.png"] :
									 [NSImage imageNamed:@"IPAudioPlay.png"]
									 )];
}	

- (IBAction) openPlayback:(id) sender
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
		[fDocumentController setPlaybackURL:url];
		
		[self updatePlayback];
	}
}

- (IBAction) togglePlayback:(id) sender
{
	[fDocumentController togglePlayback];
}

- (void) updateRecording
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
									  [NSImage imageNamed:@"IPAudioStop.png"] :
									  [NSImage imageNamed:@"IPAudioRecord.png"]
									  )];
	[fSaveRecordingAsButton setEnabled:(url && !isRecording)];
}

- (IBAction) toggleRecording:(id) sender
{
	[fDocumentController toggleRecording];
}

- (IBAction) saveRecording:(id) sender
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
