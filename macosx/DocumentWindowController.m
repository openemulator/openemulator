
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
	
//    [mImageBrowser setDraggingDestinationDelegate:self];
	
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
	if ([ident isEqualToString:@"Power"])
	{
		[item setLabel:NSLocalizedString(@"Power",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Power",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Power the emulation on or off.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBShutdown.png"]];
		[item setTarget:document];
		[item setAction:@selector(powerButtonPressedAndReleased:)];
	}
	else if ([ident isEqualToString:@"Reset"])
	{
		[item setLabel:NSLocalizedString(@"Reset",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Reset",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Reset the emulation.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBRestart.png"]];
		[item setTarget:document];
		[item setAction:@selector(resetButtonPressedAndReleased:)];
	}
	else if ([ident isEqualToString:@"Pause"])
	{
		[item setLabel:NSLocalizedString(@"Pause",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Pause",
												"Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Pause",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Pause or continue the emulation.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBSleep.png"]];
		[item setTarget:document];
		[item setAction:@selector(pauseButtonPressedAndReleased:)];
	}
	else if ([ident isEqualToString:@"Interrupt"])
	{
		[item setLabel:NSLocalizedString(@"Interrupt",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Interrupt",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Interrupt the emulation.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBInterrupt.png"]];
		[item setTarget:document];
		[item setAction:@selector(interruptButtonPressedAndReleased:)];
	}
	else if ([ident isEqualToString:@"Info"])
	{
		[item setLabel:NSLocalizedString(@"Info",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Info",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Get Info.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBInspector.png"]];
		[item setTarget:self];
		[item setAction:@selector(toggleInspector:)];
	}
	
	return item;
}

- (NSArray *) toolbarDefaultItemIdentifiers:(NSToolbar *) toolbar
{
	return [NSArray arrayWithObjects:
			@"Power",
			NSToolbarSpaceItemIdentifier,
			@"Reset",
			@"Pause",
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Info",
			nil];
}

- (NSArray *) toolbarAllowedItemIdentifiers:(NSToolbar *) toolbar
{
	return [NSArray arrayWithObjects:
			@"Power",
			@"Reset",
			@"Pause",
			@"Interrupt",
			@"Info",
			NSToolbarSpaceItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
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
