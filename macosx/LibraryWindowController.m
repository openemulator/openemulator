
/**
 * OpenEmulator
 * Mac OS X Library Window Controller
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the library window.
 */

#import "LibraryWindowController.h"

@implementation LibraryWindowController

- (id)init
{
	self = [self initWithWindowNibName:@"Library"];
	
	return self;
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Library"];
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(toggleLibrary:))
	{  
		NSString *menuTitle;
		if (![[self window] isVisible])
			menuTitle = NSLocalizedString(@"Show Hardware Library",
										  @"Main Menu.");
		else
			menuTitle = NSLocalizedString(@"Hide Hardware Library",
										  @"Main Menu.");
		[item setTitleWithMnemonic:menuTitle];
	}
	
	return YES;
}

- (void)toggleLibrary:(id)sender
{
	if ([[self window] isVisible])
		[[self window] orderOut:self];
	else
		[[self window] orderFront:self];
}

@end
