
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

#define DEFAULT_FRAME_WIDTH		640
#define DEFAULT_FRAME_HEIGHT	480

@implementation DocumentWindowController

- (void)windowDidLoad
{
	[super windowDidLoad];

	documentController = [NSDocumentController sharedDocumentController];
	document = [self document];
	
	fullscreen = NO;
	
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Document Toolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	[[self window] setToolbar:toolbar];
	[toolbar release];
	
	[document addObserver:fDocumentView
			   forKeyPath:@"power"
				  options:NSKeyValueObservingOptionNew
				  context:NULL];
	[document addObserver:fDocumentView
			   forKeyPath:@"pause"
				  options:NSKeyValueObservingOptionNew
				  context:NULL];
	
	// To-Do: Improve the view updating code
	[document setPower:[document power]];
	[document setPause:[document pause]];
}

- (BOOL)fullscreen
{
	return fullscreen;
}

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
{
	if (fullscreen)
		return [window frame].size;
	else
		return proposedFrameSize;
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
	 itemForItemIdentifier:(NSString *)ident
 willBeInsertedIntoToolbar:(BOOL)flag
{
	NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
	
	if ([ident isEqualToString:@"Power Off"])
	{
		[item setLabel:NSLocalizedString(@"Power",
										 "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Enable or disable the emulation's power.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBPower.png"]];
		[item setTarget:document];
		[item setAction:@selector(togglePower:)];
	}
	else if ([ident isEqualToString:@"Reset"])
	{
		[item setLabel:NSLocalizedString(@"Reset", "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Reset the emulation.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBReset.png"]];
		[item setTarget:document];
		[item setAction:@selector(resetEmulation:)];
	}
	else if ([ident isEqualToString:@"Pause"])
	{
		[item setLabel:NSLocalizedString(@"Pause", "Document toolbar item label")];
		[item setToolTip:NSLocalizedString(@"Pause or continue the emulation.",
										   "Document toolbar item label")];
		[item setImage:[NSImage imageNamed:@"TBPause.png"]];
		[item setTarget:document];
		[item setAction:@selector(togglePause:)];
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
	else
	{
		[item release];
		return nil;
	}
	
	return [item autorelease];
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
	return [self toolbarAllowedItemIdentifiers:toolbar];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:@"Power Off",
			NSToolbarSpaceItemIdentifier,
			@"Reset",
			@"Pause",
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Inspector",
			nil];
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(toggleFullscreen:))
	{
		NSString *menuTitle;
		if (!fullscreen)
			menuTitle = NSLocalizedString(@"Enter Fullscreen",
										  @"Title for menu item to enter fullscreen"
										  "(should be the same as the initial menu item in the nib).");
		else
			menuTitle = NSLocalizedString(@"Exit Fullscreen",
										  @"Title for menu item to exit fullscreen.");
		[item setTitleWithMnemonic:menuTitle];
	}
	
	return YES;
}

- (void)toggleInspectorPanel:(id)sender
{
	[[NSNotificationCenter defaultCenter] postNotificationName:@"toggleInspectorPanel"
														object:self];
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
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

- (void)setFrameSize:(double)proportion
{
	NSWindow *window = [self window];
	NSRect windowRect = [window frame];
	NSView *content = [window contentView];
	NSRect contentRect = [content frame];
	NSScreen *screen = [window screen];
	NSRect screenRect = [screen visibleFrame];
	float scale = [window userSpaceScaleFactor];
	
	contentRect.size.width *= scale;
	contentRect.size.height *= scale;
	
	double deltaWidth = NSWidth(windowRect) - NSWidth(contentRect);
	double deltaHeight = NSHeight(windowRect) - NSHeight(contentRect);
	
	windowRect.origin.x = NSMidX(windowRect);
	windowRect.origin.y = NSMaxY(windowRect);
	windowRect.size.width = scale * proportion * DEFAULT_FRAME_WIDTH + deltaWidth;
	windowRect.size.height = scale * proportion * DEFAULT_FRAME_HEIGHT + deltaHeight;
	windowRect.origin.x -= NSWidth(windowRect) / 2;
	windowRect.origin.y -= NSHeight(windowRect);
	
	if (NSMaxX(windowRect) > NSMaxX(screenRect))
		windowRect.origin.x = NSMaxX(screenRect) - NSWidth(windowRect);
	if (NSMaxY(windowRect) > NSMaxY(screenRect))
		windowRect.origin.y = NSMaxY(screenRect) - NSHeight(windowRect);
	if (NSMinX(windowRect) < NSMinX(screenRect))
		windowRect.origin.x = NSMinX(screenRect);
	if (NSMinY(windowRect) < NSMinY(screenRect))
		windowRect.origin.y = NSMinY(screenRect);
	
	if (NSWidth(windowRect) > NSWidth(screenRect))
		windowRect.size.width = NSWidth(screenRect);
	if (NSHeight(windowRect) > NSHeight(screenRect))
		windowRect.size.height = NSHeight(screenRect);
	
	[window setFrame:windowRect display:YES animate:YES];
	
	fullscreen = NO;
}

- (void)setHalfSize:(id)sender
{
	[self setFrameSize:0.5];
}

- (void)setActualSize:(id)sender
{
	[self setFrameSize:1.0];
}

- (void)setDoubleSize:(id)sender
{
	[self setFrameSize:2.0];
}

- (void)fitToScreen:(id)sender
{
	[self setFrameSize:256.0];
}

- (void)toggleFullscreen:(id)sender
{
	NSWindow *window = [self window];
	
	if (!fullscreen)
	{
		[documentController disableMenuBar];
		
		fullscreenExitRect = [window frame];
		NSRect contentRect = [[window contentView] frame];
		float titlebarHeight = NSHeight(fullscreenExitRect) - ([window userSpaceScaleFactor] *
															   NSHeight(contentRect));
		
		NSRect windowRect = [[window screen] frame];
		windowRect.size.height += titlebarHeight;
		
		[window setFrame:windowRect
				 display:YES
				 animate:YES];
		
		fullscreen = YES;
	}
	else
	{
		[window setFrame:fullscreenExitRect
				 display:YES
				 animate:YES];
		
		[documentController enableMenuBar];
		
		fullscreen = NO;
	}
}

@end
