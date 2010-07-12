
/**
 * OpenEmulator
 * Mac OS X Document Window
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles document window messages.
 */

#import "DocumentWindow.h"
#import "DocumentController.h"

#define DEFAULT_FRAME_WIDTH		768
#define DEFAULT_FRAME_HEIGHT	576

@implementation DocumentWindow

- (id)initWithContentRect:(NSRect)contentRect
				styleMask:(NSUInteger)windowStyle
				  backing:(NSBackingStoreType)bufferingType 
					defer:(BOOL)deferCreation
{
	if (self = [super initWithContentRect:contentRect
								styleMask:windowStyle
								  backing:bufferingType
									defer:deferCreation])
	{
		videoFullscreen = NO;
		
		[self setAcceptsMouseMovedEvents:YES];
	}
	
	return self;
}

- (void)dealloc
{
	if (videoFullscreen)
		[self toggleFullscreen:self];
	
	[super dealloc];
}

- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen
{
	return frameRect;
}

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
{
	if (!videoFullscreen)
		return proposedFrameSize;
	else
		return [window frame].size;
}

- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)proposedFrame
{
	return !videoFullscreen;
}

- (void)setFrameOrigin:(NSPoint)point
{
	if (!videoFullscreen)
		[super setFrameOrigin:point];
}

- (void)setFrameTopLeftPoint:(NSPoint)point
{
	if (!videoFullscreen)
		[super setFrameTopLeftPoint:point];
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(toggleFullscreen:))
	{
		NSString *menuTitle;
		if (!videoFullscreen)
			menuTitle = NSLocalizedString(@"Enter Fullscreen",
										  @"Title for menu item to enter fullscreen "
										  "(should be the same as the initial menu "
										  "item in the nib).");
		else
			menuTitle = NSLocalizedString(@"Exit Fullscreen",
										  @"Title for menu item to exit fullscreen.");
		[item setTitleWithMnemonic:menuTitle];
		
		return YES;
	}
	else if ([item action] == @selector(setHalfSize:))
		return !videoFullscreen;
	else if ([item action] == @selector(setActualSize:))
		return !videoFullscreen;
	else if ([item action] == @selector(setDoubleSize:))
		return !videoFullscreen;
	else if ([item action] == @selector(fitToScreen:))
		return !videoFullscreen;
	else if ([item action] == @selector(toggleToolbarShown:))
		return !videoFullscreen;
	else if ([item action] == @selector(runToolbarCustomizationPalette:))
		return !videoFullscreen;
	else
		return YES;
}

- (void)setFrameSize:(double)proportion
{
	NSRect windowRect = [self frame];
	NSView *content = [self contentView];
	NSRect contentRect = [content frame];
	NSScreen *screen = [self screen];
	NSRect screenRect = [screen visibleFrame];
	float scale = [self userSpaceScaleFactor];
	
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
	
	[self setFrame:windowRect display:YES animate:YES];
}

- (void)toggleFullscreen:(id)sender
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	if (!videoFullscreen)
	{
		[documentController disableMenuBar];
		
		videoWindowRect = [self frame];
		NSRect contentRect = [[self contentView] frame];
		float titlebarHeight = (NSHeight(videoWindowRect) -
								([self userSpaceScaleFactor] * 
								 NSHeight(contentRect)));
		
		NSRect windowRect = [[self screen] frame];
		windowRect.size.height += titlebarHeight;
		
		[self setFrame:windowRect
			   display:YES
			   animate:YES];
		
		videoFullscreen = YES;
	}
	else
	{
		[self setFrame:videoWindowRect
			   display:YES
			   animate:YES];
		
		[documentController enableMenuBar];
		
		videoFullscreen = NO;
	}
}

- (void)setHalfSize:(id)sender
{
	if (!videoFullscreen)
		[self setFrameSize:0.5];
}

- (void)setActualSize:(id)sender
{
	if (!videoFullscreen)
		[self setFrameSize:1.0];
}

- (void)setDoubleSize:(id)sender
{
	if (!videoFullscreen)
		[self setFrameSize:2.0];
}

- (void)fitToScreen:(id)sender
{
	if (!videoFullscreen)
		[self setFrameSize:256.0];
}

@end
