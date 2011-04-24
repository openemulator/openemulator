
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
#import "TemplateChooserWindowController.h"
#import "AudioControlsWindowController.h"
#import "LibraryWindowController.h"
#import "CanvasWindow.h"
#import "StringConversion.h"

#import "PAAudio.h"

#define LINK_HOMEPAGE	@"http://www.openemulator.org"
#define LINK_FORUMSURL	@"http://groups.google.com/group/openemulator"
#define LINK_DEVURL		@"http://code.google.com/p/openemulator"
#define LINK_DONATEURL	@"http://www.openemulator.org/donate"

@implementation DocumentController

- (id)init
{
	if (self = [super init])
	{
		paAudio = new PAAudio();
		
		diskImagePathExtensions = [[NSArray alloc] initWithObjects:
								   @"bin",
								   @"dsk", @"do", @"d13", @"po", @"cpm", @"nib", @"v2d", @"vdsk",
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
		
		textPathExtensions = [[NSArray alloc] initWithObjects:
							  @"txt",
							  nil];
	}
	
	return self;
}

- (void)dealloc
{
	[diskImagePathExtensions release];
	[audioPathExtensions release];
	[textPathExtensions release];
	
	delete (PAAudio *)paAudio;
	
	[super dealloc];
}



- (NSArray *)diskImagePathExtensions
{
	return diskImagePathExtensions;
}

- (NSArray *)audioPathExtensions
{
	return audioPathExtensions;
}

- (NSArray *)textPathExtensions
{
	return textPathExtensions;
}

- (void *)paAudio
{
	return paAudio;
}



- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	
	NSDictionary *defaults = [NSDictionary dictionaryWithObjectsAndKeys:
							  [NSNumber numberWithBool:NO], @"OEAudioControlsVisible",
							  [NSNumber numberWithBool:NO], @"OEAudioFullDuplex",
							  [NSNumber numberWithFloat:1.0], @"OEAudioPlayVolume",
							  [NSNumber numberWithBool:YES], @"OEAudioPlayThrough",
							  [NSNumber numberWithBool:YES], @"OEVideoEnableShader",
							  nil
							  ];
	[userDefaults registerDefaults:defaults]; 
	
	if ([userDefaults boolForKey:@"OEAudioControlsVisible"])
		[fAudioControlsWindowController showWindow:self];
	if ([userDefaults boolForKey:@"OELibraryIsVisible"])
		[fLibraryWindowController showWindow:self];
	
	[userDefaults addObserver:self
				   forKeyPath:@"OEAudioFullDuplex"
					  options:NSKeyValueObservingOptionNew
					  context:nil];
	[userDefaults addObserver:self
				   forKeyPath:@"OEAudioPlayVolume"
					  options:NSKeyValueObservingOptionNew
					  context:nil];
	[userDefaults addObserver:self
				   forKeyPath:@"OEAudioPlayThrough"
					  options:NSKeyValueObservingOptionNew
					  context:nil];
	
	((PAAudio *)paAudio)->setFullDuplex([userDefaults
												   boolForKey:@"OEAudioFullDuplex"]);
	((PAAudio *)paAudio)->setPlayVolume([userDefaults
												   floatForKey:@"OEAudioPlayVolume"]);
	((PAAudio *)paAudio)->setPlayThrough([userDefaults
													boolForKey:@"OEAudioPlayThrough"]);
	
	((PAAudio *)paAudio)->open();
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
}

- (void)applicationWillTerminate:(NSNotification *)sender
{
	((PAAudio *)paAudio)->close();
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	[userDefaults setBool:[[fAudioControlsWindowController window] isVisible]
				   forKey:@"OEAudioControlsVisible"];
	[userDefaults setBool:[[fLibraryWindowController window] isVisible]
				   forKey:@"OELibraryIsVisible"];
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
}

- (void)applicationDidResignActive:(NSNotification *)notification
{
	// To-Do: dezoom fullscreen window?
}

- (BOOL)application:(NSApplication *)theApplication
		   openFile:(NSString *)path
{
	NSString *pathExtension = [[path pathExtension] lowercaseString];
	
	// Open emulation through the Cocoa interface
	if ([pathExtension compare:@OE_PACKAGE_PATH_EXTENSION] == NSOrderedSame)
		return NO;
	
	// Open audio
	if ([audioPathExtensions containsObject:pathExtension])
	{
		[fAudioControlsWindowController readFromPath:path];
		
		return YES;
	}
	
	// Paste text
	if ([textPathExtensions containsObject:pathExtension])
	{
		NSString *clipboard = [NSString stringWithContentsOfFile:path
													usedEncoding:nil
														   error:nil];
		
		[[NSApp mainWindow] performSelector:@selector(pasteText:)
								 withObject:clipboard];
		
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
		if ([[self currentDocument] mount:path])
			return YES;
		
		if ([[self currentDocument] testMount:path])
		{
			NSAlert *alert = [[NSAlert alloc] init];
			[alert setMessageText:[NSString localizedStringWithFormat:
								   @"The document \u201C%@\u201D can't be mounted "
								   "in this emulation.",
								   [path lastPathComponent]]];
			[alert setInformativeText:[NSString localizedStringWithFormat:
									   @"The storage devices compatible with this "
									   "document are busy. "
									   "Try unmounting a storage device."]];
			[alert runModal];
			[alert release];
			
			return YES;
		}
		
		NSAlert *alert = [[NSAlert alloc] init];
		[alert setMessageText:[NSString localizedStringWithFormat:
							   @"The document \u201C%@\u201D can't be mounted "
							   "in this emulation.",
							   [path lastPathComponent]]];
		[alert setInformativeText:[NSString localizedStringWithFormat:
								   @"The document is not compatible with "
								   "any storage device in this emulation. "
								   "Try mounting the document in another emulation."]];
		[alert runModal];
		[alert release];
		
		return YES;
	}
	
	return YES;
}



- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
						change:(NSDictionary *)change
					   context:(void *)context
{
	id theObject = [change objectForKey:NSKeyValueChangeNewKey];
	
	if ([keyPath isEqualToString:@"OEAudioFullDuplex"])
		((PAAudio *)paAudio)->setFullDuplex([theObject boolValue]);
	else if ([keyPath isEqualToString:@"OEAudioPlayVolume"])
		((PAAudio *)paAudio)->setPlayVolume([theObject floatValue]);
	else if ([keyPath isEqualToString:@"OEAudioPlayThrough"])
		((PAAudio *)paAudio)->setPlayThrough([theObject boolValue]);
}

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)anItem
{
    SEL action = [anItem action];
    
	if (action == @selector(newDocument:))
		return ![[fTemplateChooserWindowController window] isVisible];
	else if (action == @selector(newDocumentFromTemplateChooser:))
		return ![[fTemplateChooserWindowController window] isVisible];
	else if (action == @selector(toggleAudioControls:))
	{
		NSString *title = ([[fAudioControlsWindowController window] isVisible] ?
						   NSLocalizedString(@"Hide Audio Controls",
											 @"Main Menu.") :
						   NSLocalizedString(@"Show Audio Controls",
											 @"Main Menu."));
		[fAudioControlsMenuItem setTitle:title];
		
		return YES;
	}
	else if (action == @selector(toggleLibrary:))
	{
		NSString *title = ([[fLibraryWindowController window] isVisible] ?
						   NSLocalizedString(@"Hide Hardware Library",
											 @"Main Menu.") :
						   NSLocalizedString(@"Show Hardware Library",
											 @"Main Menu."));
		[fLibraryMenuItem setTitle:title];
		
		return YES;
	}
	
	return [super validateUserInterfaceItem:anItem];
}

- (IBAction)toggleAudioControls:(id)sender
{
	if ([[fAudioControlsWindowController window] isVisible])
		[[fAudioControlsWindowController window] orderOut:self];
	else
		[[fAudioControlsWindowController window] orderFront:self];
}

- (IBAction)toggleLibrary:(id)sender
{
	if ([[fLibraryWindowController window] isVisible])
		[[fLibraryWindowController window] orderOut:self];
	else
		[[fLibraryWindowController window] orderFront:self];
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
	[fileTypes addObjectsFromArray:textPathExtensions];
	
	if ([panel runModalForTypes:fileTypes] == NSOKButton)
	{
		NSURL *url = [panel URL];
		if ([self application:NSApp openFile:[url path]])
			return;
		
		NSError *error;
		if (![self openDocumentWithContentsOfURL:url
										 display:YES
										   error:&error])
			[[NSAlert alertWithError:error] runModal];
	}
}

- (id)openUntitledDocumentAndDisplay:(BOOL)displayDocument
							   error:(NSError **)outError
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	if (![defaults boolForKey:@"OEDefaultTemplateEnabled"])
	{
		[self newDocumentFromTemplateChooser:self];
		
		*outError = [NSError errorWithDomain:NSCocoaErrorDomain
										code:NSUserCancelledError
									userInfo:nil];
		return nil;
	}
	else
	{
		NSString *path = [defaults stringForKey:@"OEDefaultTemplatePath"];
		id document = nil;
		
		if (!path)
		{
			*outError = [NSError errorWithDomain:@"libemulator"
											code:0
										userInfo:nil];
			return nil;
		}
		
		NSURL *url = [NSURL fileURLWithPath:path];
		document = [self openUntitledDocumentWithTemplateURL:url
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
