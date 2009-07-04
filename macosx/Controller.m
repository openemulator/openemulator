
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

@implementation Controller

- (id)init
{
	if (self = [super init])
	{
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
	}
	
	return self;
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
