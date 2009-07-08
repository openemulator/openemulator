
/**
 * OpenEmulator
 * Mac OS X Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the emulator's main instance.
 */

#import "Controller.h"

@implementation Controller

- (void)linkHomepage:(id)sender
{
	NSURL *url = [NSURL	URLWithString:@LINK_HOMEPAGE];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void)linkForums:(id)sender
{
	NSURL *url = [NSURL	URLWithString:@LINK_FORUMSURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void)linkDevelopment:(id)sender
{
	NSURL *url = [NSURL	URLWithString:@LINK_DEVURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void)linkDonate:(id)sender
{
	NSURL *url = [NSURL	URLWithString:@LINK_DONATEURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

@end
