
/**
 * OpenEmulator
 * Mac OS X Document Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import <Carbon/Carbon.h>

#import "DocumentWindowController.h"

#define DEFAULT_FRAME_WIDTH		640
#define DEFAULT_FRAME_HEIGHT	480

@implementation DocumentWindowController

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	isFullscreen = NO;
	
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Document Toolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
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
		[item setLabel:NSLocalizedString(@"Reset", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBPause.png"]];
		[item setTarget:document];
		[item setAction:@selector(togglePause:)];
		[item setAutovalidates:NO];
	}
	else if ([ident isEqualToString:@"Inspector"])
	{
		[item setLabel:NSLocalizedString(@"Inspector", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBInspector.png"]];
		[item setTarget:self];
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
		NSString *menuTitle = nil;
		
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
	
//	[[self document] saveDocumentAsTemplate:sender];
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

- (void)toggleInspectorPanel:(id)sender
{
	[[NSNotificationCenter defaultCenter] postNotification:
	 [NSNotification notificationWithName:@"toggleInspectorPanelNotification"
								   object:self]];
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    if ((NSDragOperationGeneric & [sender draggingSourceOperationMask]) 
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
	
	float deltaWidth = windowFrame.size.width - contentFrame.size.width;
	float deltaHeight = windowFrame.size.height - contentFrame.size.height;
	float scale = [window userSpaceScaleFactor];
	
	windowFrame.origin.x += windowFrame.size.width / 2;
	windowFrame.origin.y += windowFrame.size.height;
	windowFrame.size.width = scale * (proportion * DEFAULT_FRAME_WIDTH + deltaWidth);
	windowFrame.size.height = scale * (proportion * DEFAULT_FRAME_HEIGHT + deltaHeight);
	windowFrame.origin.x -= windowFrame.size.width / 2;
	windowFrame.origin.y -= windowFrame.size.height;
	
	float maxX = NSMaxX(screenFrame) - windowFrame.size.width;
	float maxY = NSMaxY(screenFrame) - windowFrame.size.height;
	float minX = screenFrame.origin.x;
	float minY = screenFrame.origin.y;
	
	if (windowFrame.origin.x > maxX)
		windowFrame.origin.x = maxX;
	if (windowFrame.origin.y > maxY)
		windowFrame.origin.y = maxY;
	if (windowFrame.origin.x < minX)
		windowFrame.origin.x = minX;
	if (windowFrame.origin.y < minY)
		windowFrame.origin.y = minY;
	if (windowFrame.size.width > screenFrame.size.width)
		windowFrame.size.width = screenFrame.size.width;
	if (windowFrame.size.height > screenFrame.size.height)
		windowFrame.size.height = screenFrame.size.height;
	
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
	/*
	 CGDisplayFadeReservationToken tok;
	 
	 CGAcquireDisplayFadeReservation(25, &tok);
	 CGDisplayFade(tok, 0.5, kCGDisplayBlendNormal, kCGDisplayBlendSolidColor, 0, 0, 0, TRUE);
	 
	 CGDisplayFade(tok, 0.5, kCGDisplayBlendSolidColor, kCGDisplayBlendNormal, 0, 0, 0, TRUE);
	 CGReleaseDisplayFadeReservation(tok);
	 */
	
	NSWindow *window = [self window];
	NSRect contentFrame = [window contentRectForFrameRect:[window frame]];
	
	if (!isFullscreen)
	{
		NSView *content = [window contentView];
		NSRect screenFrame = [[window screen] frame];

		fullscreenWindow = [[DocumentWindow alloc] initWithContentRect:contentFrame
															 styleMask:NSBorderlessWindowMask
															   backing:NSBackingStoreBuffered
																 defer:NO];
		[content retain];
		[window setContentView:nil];
		[window setWindowController:nil];
		[fullscreenWindow setContentView:content];
		[fullscreenWindow setWindowController:self];
		[content release];
		
		[[NSNotificationCenter defaultCenter] postNotification:
		 [NSNotification notificationWithName:@"disableMenuBarNotification"
									   object:self]];
		[[NSApplication sharedApplication] addWindowsItem:fullscreenWindow
													title:[window title]
												 filename:NO];
		[fullscreenWindow setDelegate:self];
		[fullscreenWindow makeKeyAndOrderFront:self];
		[fullscreenWindow setFrame:screenFrame display:YES animate:YES];
		[window orderOut:self];
		
		isFullscreen = YES;
	}
	else
	{
		[window makeKeyAndOrderFront:self];
		[fullscreenWindow makeKeyAndOrderFront:self];
		
		[[NSApplication sharedApplication] removeWindowsItem:fullscreenWindow];
		[fullscreenWindow setFrame:contentFrame
						   display:YES
						   animate:YES];
		
		[window makeKeyAndOrderFront:self];
		
		NSView *content = [fullscreenWindow contentView];
		[content retain];
		[fullscreenWindow setContentView:nil];
		[fullscreenWindow setWindowController:nil];
		[window setContentView:content];
		[window setWindowController:self];
		[content release];
		
		[[NSNotificationCenter defaultCenter] postNotification:
		 [NSNotification notificationWithName:@"enableMenuBarNotification"
									   object:self]];
		
		[fullscreenWindow release];
		fullscreenWindow = nil;
		
		isFullscreen = NO;
	}
}

@end
