
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

- (void) windowDidLoad
{
	[super windowDidLoad];
	
	documentController = [NSDocumentController sharedDocumentController];
	document = [self document];
	
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Document Toolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	[[self window] setToolbar:toolbar];
	[toolbar release];
	
/*	[document addObserver:fDocumentView
			   forKeyPath:@"power"
				  options:NSKeyValueObservingOptionNew
				  context:NULL];
*/	
	// To-Do: Improve the view updating code
	[document setPower:[document power]];
}

- (NSToolbarItem *) toolbar:(NSToolbar *) toolbar
	  itemForItemIdentifier:(NSString *) ident
  willBeInsertedIntoToolbar:(BOOL) flag
{
	NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
	
	if (!item)
		return nil;
	
	[item autorelease];
	
	if ([ident isEqualToString:@"Power Off"])
	{
		[item setLabel:NSLocalizedString(@"Power",
										 "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Enable or disable the emulation's power.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBPower.png"]];
		[item setTarget:document];
		[item setAction:@selector(powerButtonPressed:)];
	}
	else if ([ident isEqualToString:@"Reset"])
	{
		[item setLabel:NSLocalizedString(@"Reset", "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Reset the emulation.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBReset.png"]];
		[item setTarget:document];
		[item setAction:@selector(resetButtonPressed:)];
	}
	else if ([ident isEqualToString:@"Pause"])
	{
		[item setLabel:NSLocalizedString(@"Pause", "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Pause or continue the emulation.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBPause.png"]];
		[item setTarget:document];
		[item setAction:@selector(pauseButtonPressed:)];
	}
	else if ([ident isEqualToString:@"Inspector"])
	{
		[item setLabel:NSLocalizedString(@"Inspector", "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Show or hide the inspector window.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBInspector.png"]];
		[item setTarget:self];
		[item setAction:@selector(toggleInspectorPanel:)];
	}
	
	return item;
}

- (NSArray *) toolbarDefaultItemIdentifiers:(NSToolbar *) toolbar
{
	return [self toolbarAllowedItemIdentifiers:toolbar];
}

- (NSArray *) toolbarAllowedItemIdentifiers:(NSToolbar *) toolbar
{
	return [NSArray arrayWithObjects:@"Power Off",
			NSToolbarSpaceItemIdentifier,
			@"Reset",
			@"Pause",
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Inspector",
			nil];
}

- (void) toggleInspectorPanel:(id) sender
{
	[[NSNotificationCenter defaultCenter] postNotificationName:@"toggleInspectorPanel"
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
