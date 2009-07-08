
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
#import "NewDocumentWindowController.h"

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
		*out++ = (rand() & 0xffff) / 65535.0f / 16;
		*out++ = (rand() & 0xffff) / 65535.0f / 16;
	}
	
	return paContinue;
}

@implementation DocumentController

- (id)init
{
	if (self = [super init])
	{
		fDefaults = [NSUserDefaults standardUserDefaults];
		fileTypes = [NSArray arrayWithObjects:
					 @"emulation",
					 @"wav", @"aiff", @"aif",
					 @"dsk", @"do", @"d13", @"po", @"img", @"cpm", @"nib", @"v2d",
					 @"vdsk",
					 @"2mg", @"2img", @"hdv", @"sdk",
					 @"shk", @"img",
					 @"fdi",
					 nil];
		[fileTypes retain];
		
		disableMenuBarCount = 0;
		
		if (Pa_Initialize() != paNoError)
			return self;
		
		if (Pa_OpenDefaultStream(&portAudioStream, 2, 2, paFloat32,
								 48000, 512, portAudioCallback, self) != paNoError)
			return self;
		
		if (Pa_StartStream(portAudioStream) != paNoError)
			return self;
	}
	
	return self;
}

- (void)dealloc
{
	[fileTypes release];
	[super dealloc];
}

- (BOOL)application:(NSApplication *)theApplication
		   openFile:(NSString *)filename
{
	printf("openFile\n");
	//	[[NSDocumentController sharedDocumentController]
	//	 noteNewRecentDocumentURL:[NSURL fileURLWithPath:filename]];
	
	// If it is a disk image, mount and return yes
	
	// Otherwise return No, so the default document handler manages this situation
	return NO;
}

- (BOOL)application:(NSApplication *)theApplication
		  openFiles:(NSArray *)filenames
{
	printf("openFile\n");
	//	[[NSDocumentController sharedDocumentController]
	//	 noteNewRecentDocumentURL:[NSURL fileURLWithPath:filename]];
	
	// If it is a disk image, mount and return yes
	
	// Otherwise return No, so the default document handler manages this situation
	return NO;
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
	printf("applicationWillFinishLaunching\n");
	if ([fDefaults boolForKey:@"InspectorPanelIsVisible"])
		[fInspectorPanelController toggleInspectorPanel:self];
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	printf("applicationDidFinishLaunching\n");
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	NSWindow *window;
	
	window = [fInspectorPanelController window];
	[fDefaults setInteger:[window isVisible] forKey:@"InspectorPanelIsVisible"];
	
	Pa_Terminate();
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(newDocument:))
		return !isNewDocumentWindowOpen;
	
	return YES;
}

- (id)openUntitledDocumentAndDisplay:(BOOL)displayDocument error:(NSError **)outError
{
	BOOL isChooseTemplate = YES;
	if (isChooseTemplate)
	{
		isNewDocumentWindowOpen = YES;
		
		NewDocumentWindowController *newDocumentWindowController;
		newDocumentWindowController = [[NewDocumentWindowController alloc] init:self];
		
		[newDocumentWindowController showWindow:self];
		
		*outError = [NSError errorWithDomain:NSCocoaErrorDomain
										code:NSUserCancelledError userInfo:nil];
		return nil;
	}
	else
	{
		return [super openUntitledDocumentAndDisplay:displayDocument error:outError];
	}
}

- (id)openUntitledDocumentWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError
{
	NSDocument *document = [self makeUntitledDocumentWithTemplateURL:templateURL
															   error:outError];
	if (document)
	{
		[self addDocument:document];
		[document makeWindowControllers];
		[document showWindows];
	}
	
	return document;
}

- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError
{
	*outError = [[NSError alloc] init];
	
	return [[Document alloc] initFromTemplate:templateURL];
}

- (void)noteNewDocumentWindowClosed
{
	isNewDocumentWindowOpen = NO;
}

- (void)openDocument:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	if ([panel runModalForTypes:fileTypes] == NSOKButton)
	{
		NSURL *url = [[panel URLs] lastObject];
		NSError *error;
		// if (isDocument)
		[self openDocumentWithContentsOfURL:url display:YES error:&error];
		// else
		// [[NSDocumentController sharedDocumentController] document] mountDiskImage:]:
	}
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

@end
