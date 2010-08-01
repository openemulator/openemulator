
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
#import "InspectorController.h"
#import "TemplateChooserController.h"

#import "OEPA.h"

#define LINK_HOMEPAGE	@"http://www.openemulator.org"
#define LINK_FORUMSURL	@"http://groups.google.com/group/openemulator"
#define LINK_DEVURL		@"http://code.google.com/p/openemulator"
#define LINK_DONATEURL	@"http://www.openemulator.org/donate.html"

@implementation DocumentController

- (id)init
{
	if (self = [super init])
	{
		oepa = new OEPA();
		
		diskImageFileTypes = [[NSArray alloc] initWithObjects:
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
		
		audioFileTypes = [[NSArray alloc] initWithObjects:
						  @"wav",
						  @"aiff", @"aif", @"aifc",
						  @"au",
						  @"flac",
						  @"caf",
						  @"ogg", @"oga",
						  nil];
		
		audioPlayURL = nil;
		audioRecordingURL = nil;
		
		disableMenuBarCount = 0;
	}
	
	return self;
}

- (void)dealloc
{
	if (audioRecordingURL)
	{
		NSFileManager *fileManager = [NSFileManager defaultManager];
		NSError *error;
		
		[fileManager removeItemAtPath:[audioRecordingURL path]
								error:&error];
		
		[audioRecordingURL release];
	}
	
	[diskImageFileTypes release];
	[audioFileTypes release];
	
	delete (OEPA *)oepa;
	
	[super dealloc];
}

- (void *)oepa
{
	return oepa;
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
	[fInspectorController restoreWindowState:self];
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	if (![defaults valueForKey:@"OEFullDuplex"])
		[defaults setBool:YES forKey:@"OEFullDuplex"];
	if (![defaults valueForKey:@"OEPlayThrough"])
		[defaults setBool:YES forKey:@"OEPlayThrough"];
	if (![defaults valueForKey:@"OEVolume"])
		[defaults setFloat:1.0 forKey:@"OEVolume"];
	
	((OEPA *)oepa)->setFullDuplex([defaults boolForKey:@"OEFullDuplex"]);
	((OEPA *)oepa)->setPlayThrough([defaults floatForKey:@"OEPlayThrough"]);
	((OEPA *)oepa)->setVolume([defaults floatForKey:@"OEVolume"]);
	
//	((OEPA *)oepa)->open();
	
	[defaults addObserver:self
			   forKeyPath:@"OEFullDuplex"
				  options:NSKeyValueObservingOptionNew
				  context:nil];
	[defaults addObserver:self
			   forKeyPath:@"OEPlayThrough"
				  options:NSKeyValueObservingOptionNew
				  context:nil];
	[defaults addObserver:self
			   forKeyPath:@"OEVolume"
				  options:NSKeyValueObservingOptionNew
				  context:nil];
}

- (BOOL)application:(NSApplication *)theApplication
		   openFile:(NSString *)filename
{
	NSString *extension = [[filename pathExtension] lowercaseString];
	
	// Open an emulation through standard interface
	if ([extension compare:@OE_PACKAGE_EXTENSION] == NSOrderedSame)
		return NO;
	
	// Open audio files
	if ([audioFileTypes containsObject:extension])
	{
		[self setPlayURL:[NSURL fileURLWithPath:filename]];
		return YES;
	}
	
	// Open default document if other filetype
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
	
	// Mount disk images
	if ([diskImageFileTypes containsObject:extension])
	{
		
	}
	
	// Otherwise display error
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert setMessageText:[NSString localizedStringWithFormat:
						   @"The document \u201C%@\u201D could not be opened. "
						   "This emulation cannot open files in this format.",
						   [filename lastPathComponent]]];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert runModal];
	
	return YES;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	((OEPA *)oepa)->close();
	
	[fInspectorController storeWindowState:self];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
						change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"OEFullDuplex"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		int value = [object intValue];
		((OEPA *)oepa)->setFullDuplex(value);
	}
	else if ([keyPath isEqualToString:@"OEPlayThrough"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		int value = [object intValue];
		((OEPA *)oepa)->setPlayThrough(value);
	}
	else if ([keyPath isEqualToString:@"OEVolume"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		float value = [object floatValue];
		((OEPA *)oepa)->setVolume(value);
	}
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(newDocument:))
		return ![[fTemplateChooserController window] isVisible];
	else if ([item action] == @selector(newDocumentFromTemplateChooser:))
		return ![[fTemplateChooserController window] isVisible];
	else
		return YES;
}

- (IBAction)newDocumentFromTemplateChooser:(id)sender
{
	[fTemplateChooserController run];
}

- (IBAction)openDocument:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSMutableArray *fileTypes = [NSMutableArray array];
	[fileTypes addObject:@OE_PACKAGE_EXTENSION];
	[fileTypes addObjectsFromArray:audioFileTypes];
	[fileTypes addObjectsFromArray:diskImageFileTypes];
	
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
	return ((OEPA *)oepa)->addEmulation((OEPAEmulation *)emulation);
}

- (void)removeEmulation:(void *)emulation
{
	((OEPA *)oepa)->removeEmulation((OEPAEmulation *)emulation);
}

- (void)setPlayURL:(NSURL *)theURL
{
	if (audioPlayURL)
		[audioPlayURL release];
	
	audioPlayURL = [theURL copy];
	if (audioPlayURL)
		((OEPA *)oepa)->startPlaying([[audioPlayURL path] UTF8String]);
}

- (void)togglePlay
{
	if (!((OEPA *)oepa)->isPlaying())
		((OEPA *)oepa)->startPlaying([[audioPlayURL path] UTF8String]);
	else
		((OEPA *)oepa)->stopPlaying();
}

- (BOOL)playing
{
	return ((OEPA *)oepa)->isPlaying();
}

- (float)playTime
{
	return ((OEPA *)oepa)->getPlayTime();
}

- (float)playDuration
{
	return ((OEPA *)oepa)->getPlayDuration();
}

- (NSURL *)playURL
{
	if (audioPlayURL)
		return [[audioPlayURL copy] autorelease];
	else
		return nil;
}

- (void)toggleRecording
{
	if (!((OEPA *)oepa)->isRecording())
	{
		NSString *thePath = [NSTemporaryDirectory()
							 stringByAppendingPathComponent:@"oerecording"];
		audioRecordingURL = [[NSURL alloc] initFileURLWithPath:thePath];
		
		((OEPA *)oepa)->startRecording([[audioRecordingURL path] UTF8String]);
	}
	else
		((OEPA *)oepa)->stopRecording();
}

- (void)saveRecordingAs:(NSURL *)theURL
{
	if (!audioRecordingURL)
		return;
	
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSError *error;
	
	[fileManager moveItemAtPath:[audioRecordingURL path]
						 toPath:[theURL path]
						  error:&error];
	
	[audioRecordingURL release];
	
	audioRecordingURL = nil;
}

- (BOOL)recording
{
	return ((OEPA *)oepa)->isRecording();
}

- (float)recordingTime
{
	return ((OEPA *)oepa)->getRecordingTime();
}

- (long long)recordingSize
{
	return ((OEPA *)oepa)->getRecordingSize();
}

- (NSURL *)recordingURL
{
	if (audioRecordingURL)
		return [[audioRecordingURL copy] autorelease];
	else
		return nil;
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
