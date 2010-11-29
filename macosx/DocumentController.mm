
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls emulations.
 */

#import <Carbon/Carbon.h>

#import "DocumentController.h"
#import "Document.h"
#import "StringConversion.h"

#import "OEPortAudio.h"

#define LINK_HOMEPAGE	@"http://www.openemulator.org"
#define LINK_FORUMSURL	@"http://groups.google.com/group/openemulator"
#define LINK_DEVURL		@"http://code.google.com/p/openemulator"
#define LINK_DONATEURL	@"http://www.openemulator.org/donate"

@implementation DocumentController

- (id)init
{
	if (self = [super init])
	{
		oePortAudio = new OEPortAudio();
		
		diskImagePathExtensions = [[NSArray alloc] initWithObjects:
								   @"dsk", @"do", @"d13", @"po", @"cpm", @"nib", @"v2d",
								   @"vdsk",
								   @"2mg", @"2img",
								   @"t64", @"tap", @"prg", @"p00",
								   @"d64", @"d71", @"d80", @"d81", @"d82", @"x64", @"g64",
								   @"crt",
								   @"uef",
								   @"fdi",
								   @"img", @"dmg", @"hdv", @"hfv",
								   @"iso", @"cdr",
								   nil];
		
		audioPathExtensions = [[NSArray alloc] initWithObjects:
							   @"wav",
							   @"aiff", @"aif", @"aifc",
							   @"au",
							   @"flac",
							   @"caf",
							   @"ogg", @"oga",
							   nil];
		
		disableMenuBarCount = 0;
	}
	
	return self;
}

- (void)dealloc
{
	[diskImagePathExtensions release];
	[audioPathExtensions release];
	
	delete (OEPortAudio *)oePortAudio;
	
	[super dealloc];
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	if ([defaults boolForKey:@"OEInspectorVisible"])
		[fInspectorWindowController showWindow:self];
	if ([defaults boolForKey:@"OEAudioControlsVisible"])
		[fAudioControlsWindowController showWindow:self];

	if (![defaults valueForKey:@"OEFullDuplex"])
		[defaults setBool:YES forKey:@"OEFullDuplex"];
	if (![defaults valueForKey:@"OEPlayVolume"])
		[defaults setFloat:1.0 forKey:@"OEPlayVolume"];
	if (![defaults valueForKey:@"OEPlayThrough"])
		[defaults setBool:YES forKey:@"OEPlayThrough"];
	
	[defaults addObserver:self
			   forKeyPath:@"OEFullDuplex"
				  options:NSKeyValueObservingOptionNew
				  context:nil];
	[defaults addObserver:self
			   forKeyPath:@"OEPlayVolume"
				  options:NSKeyValueObservingOptionNew
				  context:nil];
	[defaults addObserver:self
			   forKeyPath:@"OEPlayThrough"
				  options:NSKeyValueObservingOptionNew
				  context:nil];
	
	((OEPortAudio *)oePortAudio)->setFullDuplex([defaults boolForKey:@"OEFullDuplex"]);
	((OEPortAudio *)oePortAudio)->setPlayVolume([defaults floatForKey:@"OEPlayVolume"]);
	((OEPortAudio *)oePortAudio)->setPlayThrough([defaults boolForKey:@"OEPlayThrough"]);
	
	((OEPortAudio *)oePortAudio)->open();
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	((OEPortAudio *)oePortAudio)->close();
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setBool:[[fInspectorWindowController window] isVisible]
			   forKey:@"OEInspectorVisible"];
	[defaults setBool:[[fAudioControlsWindowController window] isVisible]
			   forKey:@"OEAudioControlsVisible"];
	
	return NSTerminateNow;
}

- (void)applicationDidTerminate:(NSNotification *)notification
{
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
}

- (void)applicationDidResignActive:(NSNotification *)notification
{
	// To-Do: zoom fullscreen window
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
						change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"OEFullDuplex"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		BOOL value = [object intValue];
		((OEPortAudio *)oePortAudio)->setFullDuplex(value);
	}
	else if ([keyPath isEqualToString:@"OEPlayVolume"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		float value = [object floatValue];
		((OEPortAudio *)oePortAudio)->setPlayVolume(value);
	}
	else if ([keyPath isEqualToString:@"OEPlayThrough"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		BOOL value = [object intValue];
		((OEPortAudio *)oePortAudio)->setPlayThrough(value);
	}
}

- (BOOL)application:(NSApplication *)theApplication
		   openFile:(NSString *)path
{
	NSString *pathExtension = [[path pathExtension] lowercaseString];
	
	// Open emulation (through the Cocoa interface)
	if ([pathExtension compare:@OE_PACKAGE_PATH_EXTENSION] == NSOrderedSame)
		return NO;
	
	// Open audio
	if ([audioPathExtensions containsObject:pathExtension])
	{
		[fAudioControlsWindowController readFromPath:path];
		return YES;
	}
	
	// Paste text
	if ([pathExtension compare:@"txt"] == NSOrderedSame)
	{
		// To-Do: Find current canvas
		// To-Do: Paste
		return YES;
	}
	
	// Open new untitled document if no document is open
	if (![self currentDocument])
	{
		NSError *error;
		if (![self openUntitledDocumentAndDisplay:YES
											error:&error])
		{
			if (([[error domain] compare:NSCocoaErrorDomain] != NSOrderedSame) ||
				([error code] != NSUserCancelledError))
				[[NSAlert alertWithError:error] runModal];
			
			return YES;
		}
	}
	
	// Mount disk image
	if ([diskImagePathExtensions containsObject:pathExtension])
	{
		// To-Do:
	}
	
	// Display error
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert setMessageText:[NSString localizedStringWithFormat:
						   @"The document \u201C%@\u201D could not be opened. "
						   "This emulation cannot open files in this format.",
						   [path lastPathComponent]]];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert runModal];
	
	return YES;
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(newDocument:))
		return ![[fTemplateChooserWindowController window] isVisible];
	else if ([item action] == @selector(newDocumentFromTemplateChooser:))
		return ![[fTemplateChooserWindowController window] isVisible];
	else
		return YES;
}

- (NSArray *)diskImagePathExtensions
{
	return diskImagePathExtensions;
}

- (NSArray *)audioPathExtensions
{
	return audioPathExtensions;
}

- (void *)oePortAudio
{
	return oePortAudio;
}

- (void)toggleInspector:(id)sender
{
	[fInspectorWindowController toggleInspector:sender];
}

- (void)toggleAudioControls:(id)sender
{
	[fAudioControlsWindowController toggleAudioControls:sender];
}

- (IBAction)newDocumentFromTemplateChooser:(id)sender
{
	[fTemplateChooserWindowController run];
}

- (IBAction)openDocument:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSMutableArray *fileTypes = [NSMutableArray array];
	[fileTypes addObject:@OE_PACKAGE_PATH_EXTENSION];
	[fileTypes addObjectsFromArray:audioPathExtensions];
	[fileTypes addObjectsFromArray:diskImagePathExtensions];
	[fileTypes addObject:@"txt"];
	
	if ([panel runModalForTypes:fileTypes] == NSOKButton)
	{
		NSURL *url = [panel URL];
		if ([self application:NSApp openFile:[url path]])
			return;
		
		NSError *error;
		if (![self openDocumentWithContentsOfURL:url display:YES error:&error])
			[[NSAlert alertWithError:error] runModal];
	}
}

- (id)openUntitledDocumentAndDisplay:(BOOL)displayDocument
							   error:(NSError **)outError
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	if (![defaults boolForKey:@"OEUseDefaultTemplate"])
	{
		[self newDocumentFromTemplateChooser:self];
		
		*outError = [NSError errorWithDomain:NSCocoaErrorDomain
										code:NSUserCancelledError
									userInfo:nil];
		return nil;
	}
	else
	{
		NSString *defaultTemplate = [defaults stringForKey:@"OEDefaultTemplate"];
		id document = nil;
		
		if (!defaultTemplate)
		{
			*outError = [NSError errorWithDomain:@"libemulator"
											code:0
										userInfo:nil];
			return nil;
		}
		
		NSURL *absoluteURL = [NSURL fileURLWithPath:defaultTemplate];
		document = [self openUntitledDocumentWithTemplateURL:absoluteURL
													 display:displayDocument
													   error:outError];
		return document;
	}
}

- (id)openUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
								  display:(BOOL)displayDocument
									error:(NSError **)outError
{
	NSDocument *document;
	
	document = [self makeUntitledDocumentWithTemplateURL:absoluteURL
												   error:outError];
	if (document)
	{
		[self addDocument:document];
		
		if (displayDocument)
		{
			[document makeWindowControllers];
			[document showWindows];
		}
	}
	
	return document;
}

- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
									error:(NSError **)outError
{
	Document *document = [[Document alloc] initWithTemplateURL:absoluteURL
														 error:outError];
	if (document)
		return [document autorelease];
	
	return nil;
}

- (BOOL)addEmulation:(void *)emulation
{
	return ((OEPortAudio *)oePortAudio)->addEmulation((OEEmulation *)emulation);
}

- (void)removeEmulation:(void *)emulation
{
	((OEPortAudio *)oePortAudio)->removeEmulation((OEEmulation *)emulation);
}

- (void)openPlayer:(NSString *)path
{
	((OEPortAudio *)oePortAudio)->openPlayer(getCString(path));
}

- (void)closePlayer
{
	return ((OEPortAudio *)oePortAudio)->closePlayer();
}

- (void)setPlayPosition:(float)time
{
	return ((OEPortAudio *)oePortAudio)->setPlayPosition(time);
}

- (void)play
{
	return ((OEPortAudio *)oePortAudio)->play();
}

- (void)pause
{
	return ((OEPortAudio *)oePortAudio)->pause();
}

- (BOOL)isPlaying
{
	return ((OEPortAudio *)oePortAudio)->isPlaying();
}

- (float)getPlayTime
{
	return ((OEPortAudio *)oePortAudio)->getPlayTime();
}

- (float)getPlayDuration
{
	return ((OEPortAudio *)oePortAudio)->getPlayDuration();
}

- (void)openRecorder:(NSString *)path
{
	((OEPortAudio *)oePortAudio)->openRecorder(getCString(path));
}

- (void)closeRecorder
{
	return ((OEPortAudio *)oePortAudio)->closeRecorder();
}

- (void)record
{
	return ((OEPortAudio *)oePortAudio)->record();
}

- (void)stop
{
	return ((OEPortAudio *)oePortAudio)->stop();
}

- (BOOL)isRecording
{
	return ((OEPortAudio *)oePortAudio)->isRecording();
}

- (float)getRecordingTime
{
	return ((OEPortAudio *)oePortAudio)->getRecordingTime();
}

- (long long)getRecordingSize
{
	return ((OEPortAudio *)oePortAudio)->getRecordingSize();
}

- (void)disableMenuBar
{
	disableMenuBarCount++;
	
	if (disableMenuBarCount == 1)
		SetSystemUIMode(kUIModeAllHidden, kUIOptionAutoShowMenuBar);
}

- (void)enableMenuBar
{
	disableMenuBarCount--;
	
	if (disableMenuBarCount == 0)
		SetSystemUIMode(kUIModeNormal, 0);
}

- (void)openHomepage:(id)sender
{
	NSURL *url = [NSURL	URLWithString:LINK_HOMEPAGE];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void)openForums:(id)sender
{
	NSURL *url = [NSURL	URLWithString:LINK_FORUMSURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void)openDevelopment:(id)sender
{
	NSURL *url = [NSURL	URLWithString:LINK_DEVURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void)openDonate:(id)sender
{
	NSURL *url = [NSURL	URLWithString:LINK_DONATEURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

@end
