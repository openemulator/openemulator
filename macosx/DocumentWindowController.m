
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
	if ([ident isEqualToString:@"Power Down"])
	{
		[item setLabel:NSLocalizedString(@"Power Down",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Power Down",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Initiate power-down.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBPowerDown.png"]];
		[item setTarget:document];
		[item setAction:@selector(powerDownPressed:)];
	}
	else if ([ident isEqualToString:@"Sleep"])
	{
		[item setLabel:NSLocalizedString(@"Sleep",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Sleep",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Initiate low power mode.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBSleep.png"]];
		[item setTarget:document];
		[item setAction:@selector(sleepPressed:)];
	}
	else if ([ident isEqualToString:@"Wake Up"])
	{
		[item setLabel:NSLocalizedString(@"Wake Up",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Wake Up",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Initiate full power state.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBWakeUp.png"]];
		[item setTarget:document];
		[item setAction:@selector(wakeUpPressed:)];
	}
	else if ([ident isEqualToString:@"Restart"])
	{
		[item setLabel:NSLocalizedString(@"Restart",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Restart",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Restart the system at the most "
										   " primitive level.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBRestart.png"]];
		[item setTarget:document];
		[item setAction:@selector(restartPressed:)];
	}
	else if ([ident isEqualToString:@"Debugger Break"])
	{
		[item setLabel:NSLocalizedString(@"Debugger Break",
										 "Document toolbar item label")];
		[item setPaletteLabel:NSLocalizedString(@"Debugger Break",
												"Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Break into the operating system debugger.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBDebuggerBreak.png"]];
		[item setTarget:document];
		[item setAction:@selector(interruptPressed:)];
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
			@"Power Down",
			@"Sleep",
			@"Wake Up",
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Info",
			nil];
}

- (NSArray *) toolbarAllowedItemIdentifiers:(NSToolbar *) toolbar
{
	return [NSArray arrayWithObjects:
			@"Power Down",
			@"Sleep",
			@"Wake Up",
			@"Restart",
			@"Debugger Break",
			@"Info",
			NSToolbarSeparatorItemIdentifier,
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
