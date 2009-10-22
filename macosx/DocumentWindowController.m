
/**
 * OpenEmulator
 * Mac OS X Document Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation's window.
 */

#import <Carbon/Carbon.h>

#import "DocumentWindowController.h"
#import "DocumentWindow.h"

@implementation DocumentWindowController

- (id) init
{
	self = [self initWithWindowNibName:@"Document"];
	
	return self;
}

- (void) windowDidLoad
{
	documentController = [NSDocumentController sharedDocumentController];
	document = [self document];
	
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Document Toolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	[[self window] setToolbar:toolbar];
	[toolbar release];
	
	// To-Do: Improve the view updating code
	[document setPower:[document power]];
}

- (NSToolbarItem *) toolbar:(NSToolbar *) toolbar
	  itemForItemIdentifier:(NSString *) ident
  willBeInsertedIntoToolbar:(BOOL) flag
{
	NSToolbarItem *item;
	item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
	if (!item)
		return nil;
	
	[item autorelease];
	if ([ident isEqualToString:@"Shutdown"])
	{
		[item setLabel:NSLocalizedString(@"Shut Down",
										 "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Shut the emulation on or off.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBShutdown.png"]];
		[item setTarget:document];
		[item setAction:@selector(shutdownButtonPressed:)];
	}
	else if ([ident isEqualToString:@"Restart"])
	{
		[item setLabel:NSLocalizedString(@"Restart", "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Reset or restart the emulation.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBRestart.png"]];
		[item setTarget:document];
		[item setAction:@selector(restartButtonPressed:)];
	}
	else if ([ident isEqualToString:@"Sleep"])
	{
		[item setLabel:NSLocalizedString(@"Sleep", "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Pause or continue the emulation.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBSleep.png"]];
		[item setTarget:document];
		[item setAction:@selector(sleepButtonPressed:)];
	}
	else if ([ident isEqualToString:@"Inspector"])
	{
		[item setLabel:NSLocalizedString(@"Inspector", "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Show or hide the inspector window.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBInspector.png"]];
		[item setTarget:self];
		[item setAction:@selector(toggleInspector:)];
	}
	
	return item;
}

- (NSArray *) toolbarDefaultItemIdentifiers:(NSToolbar *) toolbar
{
	return [self toolbarAllowedItemIdentifiers:toolbar];
}

- (NSArray *) toolbarAllowedItemIdentifiers:(NSToolbar *) toolbar
{
	return [NSArray arrayWithObjects:@"Shutdown",
			NSToolbarSpaceItemIdentifier,
			@"Restart",
			@"Sleep",
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Inspector",
			nil];
}

- (void) toggleInspector:(id) sender
{
	[[NSNotificationCenter defaultCenter] postNotificationName:@"toggleInspector"
														object:self];
}

- (NSDragOperation) draggingEntered:(id <NSDraggingInfo>) sender
{
    if (([sender draggingSourceOperationMask] & NSDragOperationGeneric) 
		== NSDragOperationGeneric)
    {
        //this means that the sender is offering the type of operation we want
        //return that we want the NSDragOperationGeneric operation that they 
		//are offering
        return NSDragOperationGeneric;
    }
    else
    {
        //since they aren't offering the type of operation we want, we have 
		//to tell them we aren't interested
        return NSDragOperationNone;
    }
}

@end
