
/**
 * OpenEmulator
 * Mac OS X Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the emulator's main instance.
 */

#import <Carbon/Carbon.h>

#import "Controller.h"

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

@implementation Controller

- (id)init
{
	if (self = [super init])
	{
        fDefaults = [NSUserDefaults standardUserDefaults];
		fWorkspace = [NSWorkspace sharedWorkspace];
		
		disableMenuBarCount = 0;
		NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
		[nc addObserver:self
			   selector:@selector(disableMenuBar:)
				   name:@"disableMenuBarNotification"
				 object:nil];
		[nc addObserver:self
			   selector:@selector(enableMenuBar:)
				   name:@"enableMenuBarNotification"
				 object:nil];
		
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

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	if ([fDefaults boolForKey:@"InspectorPanelIsVisible"])
		[fInspectorPanelController toggleInspectorPanel:nil];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	NSWindow *window;
	
	window = [fInspectorPanelController window];
    [fDefaults setInteger:[window isVisible] forKey:@"InspectorPanelIsVisible"];
	
	Pa_Terminate();
}

- (void)disableMenuBar:(NSNotification *)notification
{
	disableMenuBarCount++;

	if (disableMenuBarCount >= 1)
		SetSystemUIMode(kUIModeAllHidden, kUIOptionAutoShowMenuBar);
}

- (void)enableMenuBar:(NSNotification *)notification
{
	disableMenuBarCount--;
	
	if (disableMenuBarCount <= 0)
		SetSystemUIMode(kUIModeNormal, 0);
}

/*- (BOOL)application:(NSApplication *)theApplication
			openFile:(NSString *)filename
{
	[[NSDocumentController sharedDocumentController]
	 noteNewRecentDocumentURL:[NSURL fileURLWithPath:filename]];
	
	return YES;
}

- (void)openDocument:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSArray *fileTypes = [NSArray arrayWithObjects:
						  @"emulation",
						  @"wav", @"aiff", @"aif",
						  @"dsk", @"do", @"po", @"nib",
						  @"vdsk",
						  @"2mg", @"2img", @"hdv", @"img",
						  @"fdi",
						  nil];
	
	[panel setAllowsMultipleSelection:YES];
	[panel setCanChooseFiles:YES];
	[panel setCanChooseDirectories:NO];
	
	[panel beginSheetForDirectory:nil
							 file:nil
							types:fileTypes
				   modalForWindow:fWindow
					modalDelegate:self
				   didEndSelector:@selector(openSheetClosed:returnCode:
											contextInfo:)
					  contextInfo:nil];
}
*/

- (void)linkHomepage:(id)sender
{
	NSURL *url = [NSURL	URLWithString:@LINK_HOMEPAGE];
	[fWorkspace openURL:url];
}

- (void)linkForums:(id)sender
{
	NSURL *url = [NSURL	URLWithString:@LINK_FORUMSURL];
	[fWorkspace openURL:url];
}

- (void)linkDevelopment:(id)sender
{
	NSURL *url = [NSURL	URLWithString:@LINK_DEVURL];
	[fWorkspace openURL:url];
}

- (void)linkDonate:(id)sender
{
	NSURL *url = [NSURL	URLWithString:@LINK_DONATEURL];
	[fWorkspace openURL:url];
}

@end
