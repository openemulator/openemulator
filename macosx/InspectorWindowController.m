
/**
 * OpenEmulator
 * Mac OS X Inspector Window Controller
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the inspector window.
 */

#import "InspectorWindowController.h"

@implementation InspectorWindowController

- (id)init
{
	self = [self initWithWindowNibName:@"Inspector"];
	
	return self;
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Inspector"];
}

- (BOOL)validateUserInterfaceItem:(id)item
{
    if ([item action] == @selector(toggleInspector:))
	{  
		NSString *menuTitle;
		if (![[self window] isVisible])
			menuTitle = NSLocalizedString(@"Show Inspector",
										  @"Title for menu item to show the Inspector.");
		else
			menuTitle = NSLocalizedString(@"Hide Inspector",
										  @"Title for menu item to hide the Inspector.");
		[item setTitleWithMnemonic:menuTitle];
    }
	
    return YES;
}

- (IBAction)toggleInspector:(id)sender
{
	if ([[self window] isVisible])
		[[self window] orderOut:self];
	else
		[[self window] orderFront:self];
}

@end
