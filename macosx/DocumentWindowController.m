
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
	
	isFullscreen = NO;
	
	documentController = [NSDocumentController sharedDocumentController];
	
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Document Toolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setAutosavesConfiguration:YES];
	[[self window] setToolbar:toolbar];
	[toolbar release];
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
	 itemForItemIdentifier:(NSString *)ident
 willBeInsertedIntoToolbar:(BOOL)flag
{
	NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
	NSDocument *document = [self document];
	
	if ([ident isEqualToString:@"Power Off"])
	{
		[item setLabel:NSLocalizedString(@"Power Off", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBPower.png"]];
		[item setTarget:document];
		[item setAction:@selector(togglePower:)];
		[item setAutovalidates:NO];
	}
	else if ([ident isEqualToString:@"Reset"])
	{
		[item setLabel:NSLocalizedString(@"Reset", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBReset.png"]];
		[item setTarget:document];
		[item setAction:@selector(resetEmulation:)];
		[item setAutovalidates:NO];
	}
	else if ([ident isEqualToString:@"Pause"])
	{
		[item setLabel:NSLocalizedString(@"Pause", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBPause.png"]];
		[item setTarget:document];
		[item setAction:@selector(togglePause:)];
		[item setAutovalidates:NO];
	}
	else if ([ident isEqualToString:@"Inspector"])
	{
		[item setLabel:NSLocalizedString(@"Inspector", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBInspector.png"]];
		[item setTarget:[documentController inspectorPanelController]];
		[item setAction:@selector(toggleInspectorPanel:)];
		[item setAutovalidates:NO];
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
	if ([item action] == @selector(setHalfSize:))
		return !isFullscreen;
	else if ([item action] == @selector(setActualSize:))
		return !isFullscreen;
	else if ([item action] == @selector(setDoubleSize:))
		return !isFullscreen;
	else if ([item action] == @selector(fitToScreen:))
		return !isFullscreen;
	else if ([item action] == @selector(toggleFullscreen:))
	{
		NSString *menuTitle;
		if (!isFullscreen)
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

/*
 * See if we can replace the following with an interception of the document close sheet
 */
- (void)performClose:(id)sender
{
	if (isFullscreen)
		[self toggleFullscreen:sender];
	
	[[self window] performClose:self];
}

- (void)saveDocument:(id)sender
{
	if (isFullscreen)
		[self toggleFullscreen:sender];
	
	[[self document] saveDocument:sender];
}

- (void)saveDocumentAs:(id)sender
{
	if (isFullscreen)
		[self toggleFullscreen:sender];
	
	[[self document] saveDocumentAs:sender];
}

- (void)revertDocumentToSaved:(id)sender
{
	if (isFullscreen)
		[self toggleFullscreen:sender];
	
	[[self document] revertDocumentToSaved:sender];
}

- (void)saveDocumentAsTemplate:(id)sender
{
	if (isFullscreen)
		[self toggleFullscreen:sender];
	
	[[self document] saveDocumentAsTemplate:sender];
}

- (void)runPageLayout:(id)sender
{
	if (isFullscreen)
		[self toggleFullscreen:sender];
	
	[[self document] runPageLayout:sender];
}

- (void)printDocument:(id)sender
{
	if (isFullscreen)
		[self toggleFullscreen:sender];
	
	[[self document] printDocument:sender];
}
/*
 * See end
 */

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
	NSRect windowFrame = [window frame];
	NSView *content = [window contentView];
	NSRect contentFrame = [content frame];
	NSScreen *screen = [window screen];
	NSRect screenFrame = [screen visibleFrame];
	
	double scale = [window userSpaceScaleFactor];
	contentFrame.size.width *= scale;
	contentFrame.size.height *= scale;
	double deltaWidth = NSWidth(windowFrame) - NSWidth(contentFrame);
	double deltaHeight = NSHeight(windowFrame) - NSHeight(contentFrame);
	
	windowFrame.origin.x = NSMidX(windowFrame);
	windowFrame.origin.y = NSMaxY(windowFrame);
	windowFrame.size.width = scale * proportion * DEFAULT_FRAME_WIDTH + deltaWidth;
	windowFrame.size.height = scale * proportion * DEFAULT_FRAME_HEIGHT + deltaHeight;
	windowFrame.origin.x -= NSWidth(windowFrame) / 2;
	windowFrame.origin.y -= NSHeight(windowFrame);
	
	if (NSMaxX(windowFrame) > NSMaxX(screenFrame))
		windowFrame.origin.x = NSMaxX(screenFrame) - NSWidth(windowFrame);
	if (NSMaxY(windowFrame) > NSMaxY(screenFrame))
		windowFrame.origin.y = NSMaxY(screenFrame) - NSHeight(windowFrame);
	if (NSMinX(windowFrame) < NSMinX(screenFrame))
		windowFrame.origin.x = NSMinX(screenFrame);
	if (NSMinY(windowFrame) < NSMinY(screenFrame))
		windowFrame.origin.y = NSMinY(screenFrame);
	
	if (NSWidth(windowFrame) > NSWidth(screenFrame))
		windowFrame.size.width = NSWidth(screenFrame);
	if (NSHeight(windowFrame) > NSHeight(screenFrame))
		windowFrame.size.height = NSHeight(screenFrame);
	
	[window setFrame:windowFrame display:YES animate:YES];
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
	NSRect contentFrame = [window contentRectForFrameRect:[window frame]];
	
	if (!isFullscreen)
	{
		[documentController disableMenuBar];
		
        DisableScreenUpdates();
		fullscreenWindow = [[DocumentWindow alloc] initWithContentRect:contentFrame
															 styleMask:NSBorderlessWindowMask
															   backing:NSBackingStoreBuffered
																 defer:YES];
		[window orderOut:self];
		[fullscreenWindow setContentView:[window contentView]];
		[fullscreenWindow setWindowController:self];
		[fullscreenWindow setDelegate:self];
		[fullscreenWindow makeKeyAndOrderFront:self];
        EnableScreenUpdates();
		
		[fullscreenWindow setFrame:[[window screen] frame]
						   display:YES
						   animate:YES];
		
		[NSApp addWindowsItem:fullscreenWindow
						title:[window title]
					 filename:NO];
		
		isFullscreen = YES;
	}
	else
	{
		float scale = [window userSpaceScaleFactor];
		contentFrame.size.width *= scale;
		contentFrame.size.height *= scale;
		
		[NSApp removeWindowsItem:fullscreenWindow];
		
		[fullscreenWindow setFrame:contentFrame display:YES animate:YES];
		
        DisableScreenUpdates();
		[window setContentView:[fullscreenWindow contentView]];
		[window setWindowController:self];
		[window setDelegate:self];
		[window makeKeyAndOrderFront:self];
		[fullscreenWindow release];
        EnableScreenUpdates();
		
		[documentController enableMenuBar];
		
		isFullscreen = NO;
	}
}

@end
