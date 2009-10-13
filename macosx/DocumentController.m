
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Manages documents.
 */

#import <Carbon/Carbon.h>

#import "Document.h"
#import "DocumentController.h"
#import "TemplateChooserWindowController.h"

static int portAudioCallback(const void *inputBuffer, void *outputBuffer,
							 unsigned long framesPerBuffer,
							 const PaStreamCallbackTimeInfo* timeInfo,
							 PaStreamCallbackFlags statusFlags,
							 void *userData)
{
	float *in = (float *)inputBuffer;
	float *out = (float *)outputBuffer;
	unsigned int i;
	
	for(i = 0; i < framesPerBuffer; i++)
	{
		*out++ = (rand() & 0xffff) / 65535.0f / 256;
		*out++ = (rand() & 0xffff) / 65535.0f / 256;
	}
	
	return paContinue;
}

@implementation DocumentController

- (id) init
{
	if (self = [super init])
	{
		fileTypes = [[NSArray alloc] initWithObjects:
					 @"emulation",
					 @"wav", @"aiff", @"aif",
					 @"dsk", @"do", @"d13", @"po", @"img", @"cpm", @"nib", @"v2d",
					 @"vdsk",
					 @"2mg", @"2img", @"hdv", @"sdk",
					 @"fdi",
					 @"d64", @"g64", @"d64", @"d71", @"d81", @"t64",
					 @"tap", @"prg", @"p00", @"crt",
					 @"iso", @"cdr",
					 nil];
		
		isTemplateChooserWindowOpen = NO;
		disableMenuBarCount = 0;
	}
	
	return self;
}

- (void) dealloc
{
	[fileTypes release];
	
	[super dealloc];
}

- (void) applicationWillFinishLaunching:(NSNotification *) notification
{
//	printf("applicationWillFinishLaunching\n");
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	if ([userDefaults boolForKey:@"OEInspectorPanelVisible"])
		[fInspectorPanelController toggleInspectorPanel:self];
}

- (BOOL) application:(NSApplication *) theApplication
			openFile:(NSString *) filename
{
//	printf("openFile\n");
	if ([[filename pathExtension] compare:@"emulation"] == NSOrderedSame)
		return NO;
	
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
	
	// To-Do: Mount disk image
	
	NSAlert *alert = [[NSAlert alloc] init];
	[alert setMessageText:[NSString localizedStringWithFormat:
						   @"The document \u201C%@\u201D could not be opened. "
						   "This emulation cannot open files in this format.",
						   [filename lastPathComponent]]];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert runModal];
	[alert release];
	
	return YES;
}

- (void) applicationDidFinishLaunching:(NSNotification *) notification
{
//	printf("applicationDidFinishLaunching\n");
	
	int error;
	if ((error = Pa_Initialize()) == paNoError)
		if ((error = Pa_OpenDefaultStream(&portAudioStream, 
										  0, 2, paFloat32,
										  48000, 512,
										  portAudioCallback, self)) == paNoError)
			if ((error = Pa_StartStream(portAudioStream)) == paNoError)
				return;
	
	fprintf(stderr, "portaudio: error %d\n", error);
	
	return;
}

- (void) applicationWillTerminate:(NSNotification *) notification
{
//	printf("applicationWillTerminate\n");
	
	Pa_Terminate();
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	NSWindow *window = [fInspectorPanelController window];
	[userDefaults setBool:[window isVisible] forKey:@"OEInspectorPanelVisible"];
}

- (BOOL) validateUserInterfaceItem:(id) item
{
	if ([item action] == @selector(newDocument:))
		return !isTemplateChooserWindowOpen;
	else if ([item action] == @selector(newDocumentFromTemplateChooser:))
		return !isTemplateChooserWindowOpen;
	else
		return YES;
}

- (IBAction) openDocument:(id) sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
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
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	if (![userDefaults boolForKey:@"OEUseDefaultTemplate"])
	{
		[self newDocumentFromTemplateChooser:self];
		
		*outError = [NSError errorWithDomain:NSCocoaErrorDomain
										code:NSUserCancelledError
									userInfo:nil];
		return nil;
	}
	else
	{
		NSString *defaultTemplate = [userDefaults stringForKey:@"OEDefaultTemplate"];
		id document = nil;
		
		if (!defaultTemplate)
		{
			*outError = [NSError errorWithDomain:@"libemulator"
											code:0
										userInfo:nil];
			return nil;
		}
		
		NSURL *absoluteURL = [NSURL fileURLWithPath:defaultTemplate];
		document = [self openUntitledDocumentFromTemplateURL:absoluteURL
													 display:displayDocument
													   error:outError];
		return document;
	}
}

- (id) openUntitledDocumentFromTemplateURL:(NSURL *) absoluteURL
								   display:(BOOL) displayDocument
									 error:(NSError **) outError
{
	NSDocument *document;
	
	document = [self makeUntitledDocumentFromTemplateURL:absoluteURL
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

- (id) makeUntitledDocumentFromTemplateURL:(NSURL *) absoluteURL
									 error:(NSError **) outError
{
	Document *document = [[Document alloc] initFromTemplateURL:absoluteURL
														 error:outError];
	if (!document)
		return nil;
	
	return [document autorelease];
}

- (IBAction) newDocumentFromTemplateChooser:(id) sender
{
	if (isTemplateChooserWindowOpen)
		return;
	
	isTemplateChooserWindowOpen = YES;
	
	TemplateChooserWindowController *templateChooserWindowController;
	templateChooserWindowController = [[TemplateChooserWindowController alloc] init:self];
	[templateChooserWindowController showWindow:self];
}

- (void) noteTemplateChooserWindowClosed
{
	isTemplateChooserWindowOpen = NO;
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

@end
