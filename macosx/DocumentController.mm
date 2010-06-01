
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls emulations.
 */

#import <Carbon/Carbon.h>
#import <QuartzCore/QuartzCore.h>

#import "Document.h"
#import "DocumentController.h"
#import "InspectorController.h"
#import "TemplateChooserController.h"

#import "oepa.h"

#define LINK_HOMEPAGE	@"http://www.openemulator.org"
#define LINK_FORUMSURL	@"http://groups.google.com/group/openemulator"
#define LINK_DEVURL		@"http://code.google.com/p/openemulator"
#define LINK_DONATEURL	@"http://www.openemulator.org/donate.html"

#define TIMER_FREQUENCY	10.0

@implementation DocumentController

- (id) init
{
	if (self = [super init])
	{
		diskImageFileTypes = [[NSArray alloc] initWithObjects:
							  @"dsk", @"do", @"d13", @"po", @"cpm", @"nib", @"v2d",
							  @"vdsk", @"2mg", @"2img", @"hdv", @"sdk",
							  @"d64", @"g64", @"d71", @"d81", @"t64",
							  @"tap", @"prg", @"p00", @"crt",
							  @"fdi",
							  @"img", @"hdv", @"hfv",
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
		
		audioPlaybackURL = nil;
		audioRecordingURL = nil;
		
		disableMenuBarCount = 0;
		
/*		CVReturn ret;
		CGOpenGLDisplayMask	totalDisplayMask = 0;
		CVDisplayLinkRef	displayLink;
		ret = CVDisplayLinkCreateWithOpenGLDisplayMask(totalDisplayMask, &displayLink);
		CVDisplayLinkSetOutputCallback(displayLink, renderCallback, self);
*/	}
	
	return self;
}

- (void) dealloc
{
	[diskImageFileTypes release];
	[audioFileTypes release];
	
	if (audioRecordingURL)
	{
		NSFileManager *fileManager = [NSFileManager defaultManager];
		NSError *error;
		
		[fileManager removeItemAtPath:[audioRecordingURL path]
								error:&error];
		
		[audioRecordingURL release];
	}
	
	[super dealloc];
}

- (void) applicationWillFinishLaunching:(NSNotification *) notification
{
	[fInspectorController restoreWindowState:self];
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	if (![defaults valueForKey:@"OEFullDuplex"])
		[defaults setBool:YES forKey:@"OEFullDuplex"];
	if (![defaults valueForKey:@"OEPlayThrough"])
		[defaults setBool:YES forKey:@"OEPlayThrough"];
	if (![defaults valueForKey:@"OEVolume"])
		[defaults setFloat:1.0 forKey:@"OEVolume"];
	
	oepaSetFullDuplex([defaults boolForKey:@"OEFullDuplex"]);
	oepaSetPlayThrough([defaults floatForKey:@"OEPlayThrough"]);
	oepaSetVolume([defaults floatForKey:@"OEVolume"]);
	oepaOpen();
	
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

- (BOOL) application:(NSApplication *) theApplication
			openFile:(NSString *) filename
{
	NSString *extension = [[filename pathExtension] lowercaseString];
	
	// Open an emulation through standard interface
	if ([extension compare:@OE_EXTENSION] == NSOrderedSame)
		return NO;
	
	// Open audio files
	if ([audioFileTypes containsObject:extension])
	{
		[self setPlaybackURL:[NSURL fileURLWithPath:filename]];
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

- (void) applicationDidFinishLaunching:(NSNotification *) notification
{
}

- (void) applicationWillTerminate:(NSNotification *) notification
{
	oepaClose();
	
	[fInspectorController storeWindowState:self];
}

/*static CVReturn renderCallback(CVDisplayLinkRef displayLink, 
							   const CVTimeStamp *inNow, 
							   const CVTimeStamp *inOutputTime, 
							   CVOptionFlags flagsIn, 
							   CVOptionFlags *flagsOut, 
							   void *displayLinkContext)
{
	NSLog(@"Oh no");
    return [(VideoView*)displayLinkContext renderTime:inOutputTime];
}*/

- (void) observeValueForKeyPath:(NSString *) keyPath
					   ofObject:(id) object
						 change:(NSDictionary *) change
                        context:(void *) context
{
	if ([keyPath isEqualToString:@"OEFullDuplex"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		int value = [object intValue];
		oepaSetFullDuplex(value);
	}
	else if ([keyPath isEqualToString:@"OEPlayThrough"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		int value = [object intValue];
		oepaSetPlayThrough(value);
	}
	else if ([keyPath isEqualToString:@"OEVolume"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		float value = [object floatValue];
		oepaSetVolume(value);
	}
}

- (BOOL) validateUserInterfaceItem:(id) item
{
	if ([item action] == @selector(newDocument:))
		return ![[fTemplateChooserController window] isVisible];
	else if ([item action] == @selector(newDocumentFromTemplateChooser:))
		return ![[fTemplateChooserController window] isVisible];
	else
		return YES;
}

- (IBAction) newDocumentFromTemplateChooser:(id) sender
{
	[fTemplateChooserController run];
}

- (IBAction) openDocument:(id) sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSMutableArray *fileTypes = [NSMutableArray array];
	[fileTypes addObject:@OE_EXTENSION];
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

- (id) openUntitledDocumentAndDisplay:(BOOL) displayDocument error:(NSError **) outError
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

- (id) openUntitledDocumentWithTemplateURL:(NSURL *) absoluteURL
								   display:(BOOL) displayDocument
									 error:(NSError **) outError
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

- (id) makeUntitledDocumentWithTemplateURL:(NSURL *) absoluteURL
									 error:(NSError **) outError
{
	Document *document = [[Document alloc] initWithTemplateURL:absoluteURL
														 error:outError];
	if (document)
		return [document autorelease];
	
	return nil;
}

- (void) setPlaybackURL:(NSURL *) theURL
{
	if (audioPlaybackURL)
		[audioPlaybackURL release];
	
	audioPlaybackURL = [theURL copy];
	if (audioPlaybackURL)
		oepaStartPlayback([[audioPlaybackURL path] UTF8String]);
}

- (void) togglePlayback
{
	if (!oepaIsPlayback())
		oepaStartPlayback([[audioPlaybackURL path] UTF8String]);
	else
		oepaStopPlayback();
}

- (BOOL) playback
{
	return oepaIsPlayback();
}

- (float) playbackTime
{
	return oepaGetPlaybackTime();
}

- (float) playbackDuration
{
	return oepaGetPlaybackDuration();
}

- (NSURL *) playbackURL
{
	if (audioPlaybackURL)
		return [[audioPlaybackURL copy] autorelease];
	else
		return nil;
}

- (void) toggleRecording
{
	if (!oepaIsRecording())
	{
		NSString *thePath = [NSTemporaryDirectory()
							 stringByAppendingPathComponent:@"oerecording"];
		audioRecordingURL = [[NSURL alloc] initFileURLWithPath:thePath];
		
		oepaStartRecording([[audioRecordingURL path] UTF8String]);
	}
	else
		oepaStopRecording();
}

- (void) saveRecordingAs:(NSURL *) theURL
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

- (BOOL) recording
{
	return oepaIsRecording();
}

- (float) recordingTime
{
	return oepaGetRecordingTime();
}

- (long long) recordingSize
{
	return oepaGetRecordingSize();
}

- (NSURL *) recordingURL
{
	if (audioRecordingURL)
		return [[audioRecordingURL copy] autorelease];
	else
		return nil;
}

- (void) disableMenuBar
{
	disableMenuBarCount++;
	
	if (disableMenuBarCount == 1)
		SetSystemUIMode(kUIModeAllHidden, kUIOptionAutoShowMenuBar);
}

- (void) enableMenuBar
{
	disableMenuBarCount--;
	
	if (disableMenuBarCount == 0)
		SetSystemUIMode(kUIModeNormal, 0);
}

- (void) openHomepage:(id) sender
{
	NSURL *url = [NSURL	URLWithString:LINK_HOMEPAGE];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void) openForums:(id) sender
{
	NSURL *url = [NSURL	URLWithString:LINK_FORUMSURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void) openDevelopment:(id) sender
{
	NSURL *url = [NSURL	URLWithString:LINK_DEVURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void) openDonate:(id) sender
{
	NSURL *url = [NSURL	URLWithString:LINK_DONATEURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

@end
