
/**
 * OpenEmulator
 * Mac OS X Document Window
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Overrides document window messages.
 */

#import "DocumentWindow.h"
#import "DocumentWindowController.h"

@implementation DocumentWindow

- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen
{
	return frameRect;
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	DocumentWindowController *documentWindowController = [self windowController];
	
	if ([item action] == @selector(toggleToolbarShown:))
		return ![documentWindowController fullscreen];
	else if ([item action] == @selector(runToolbarCustomizationPalette:))
		return ![documentWindowController fullscreen];
	else
		return YES;
}

@end
