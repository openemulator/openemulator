
/**
 * OpenEmulator
 * Mac OS X Canvas Window
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles canvas window messages.
 */

#import "CanvasWindow.h"
#import "DocumentController.h"

@implementation CanvasWindow

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
		fullscreen = NO;
		
		[self setAcceptsMouseMovedEvents:YES];
	}
	
	return self;
}

- (void)dealloc
{
	if (fullscreen)
		[self toggleFullscreen:self];
	
	[super dealloc];
}

- (void *)canvasComponent
{
	return [[self windowController] canvasComponent];
}

- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen
{
	return frameRect;
}

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
{
	if (!fullscreen)
		return proposedFrameSize;
	else
		return [window frame].size;
}

- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)proposedFrame
{
	return !fullscreen;
}

- (void)setFrameOrigin:(NSPoint)point
{
	if (!fullscreen)
		[super setFrameOrigin:point];
}

- (void)setFrameTopLeftPoint:(NSPoint)point
{
	if (!fullscreen)
		[super setFrameTopLeftPoint:point];
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(toggleFullscreen:))
	{
		NSString *menuTitle;
		if (!fullscreen)
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
		return !fullscreen;
	else if ([item action] == @selector(setActualSize:))
		return !fullscreen;
	else if ([item action] == @selector(setDoubleSize:))
		return !fullscreen;
	else if ([item action] == @selector(fitToScreen:))
		return !fullscreen;
	else if ([item action] == @selector(toggleToolbarShown:))
		return !fullscreen;
	else if ([item action] == @selector(runToolbarCustomizationPalette:))
		return !fullscreen;
	else
		return YES;
}

- (void)setFrameSize:(double)proportion
{
	int canvasDefaultWidth = 640;
	int canvasDefaultHeight = 480;
	
/*	OEComponent *canvasComponent = (OEComponent *)[self canvasComponent];
	if (canvasComponent)
	{
		// Update canvasDefaultWidth and height
	}
*/	
	// To-Do: Ask canvas for default size
	
	NSRect frameRect = [self frame];
	NSView *content = [self contentView];
	NSRect contentRect = [content frame];
	NSScreen *screen = [self screen];
	NSRect screenRect = [screen visibleFrame];
	float scale = [self userSpaceScaleFactor];
	
	contentRect.size.width *= scale;
	contentRect.size.height *= scale;
	
	double deltaWidth = NSWidth(frameRect) - NSWidth(contentRect);
	double deltaHeight = NSHeight(frameRect) - NSHeight(contentRect);
	
	frameRect.origin.x = NSMidX(frameRect);
	frameRect.origin.y = NSMaxY(frameRect);
	frameRect.size.width = scale * proportion * canvasDefaultWidth + deltaWidth;
	frameRect.size.height = scale * proportion * canvasDefaultHeight + deltaHeight;
	frameRect.origin.x -= NSWidth(frameRect) / 2;
	frameRect.origin.y -= NSHeight(frameRect);
	
	if (NSMaxX(frameRect) > NSMaxX(screenRect))
		frameRect.origin.x = NSMaxX(screenRect) - NSWidth(frameRect);
	if (NSMaxY(frameRect) > NSMaxY(screenRect))
		frameRect.origin.y = NSMaxY(screenRect) - NSHeight(frameRect);
	if (NSMinX(frameRect) < NSMinX(screenRect))
		frameRect.origin.x = NSMinX(screenRect);
	if (NSMinY(frameRect) < NSMinY(screenRect))
		frameRect.origin.y = NSMinY(screenRect);
	
	if (NSWidth(frameRect) > NSWidth(screenRect))
		frameRect.size.width = NSWidth(screenRect);
	if (NSHeight(frameRect) > NSHeight(screenRect))
		frameRect.size.height = NSHeight(screenRect);
	
	[self setFrame:frameRect display:YES animate:YES];
}

- (void)toggleFullscreen:(id)sender
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	if (!fullscreen)
	{
		[documentController disableMenuBar];
		
		windowRect = [self frame];
		NSRect contentRect = [[self contentView] frame];
		float titlebarHeight = (NSHeight(windowRect) -
								([self userSpaceScaleFactor] * 
								 NSHeight(contentRect)));
		
		NSRect screenRect = [[self screen] frame];
		screenRect.size.height += titlebarHeight;
		
		[self setFrame:screenRect
			   display:YES
			   animate:YES];
		
		fullscreen = YES;
	}
	else
	{
		[self setFrame:windowRect
			   display:YES
			   animate:YES];
		
		[documentController enableMenuBar];
		
		fullscreen = NO;
	}
}

- (void)setHalfSize:(id)sender
{
	if (!fullscreen)
		[self setFrameSize:0.5];
}

- (void)setActualSize:(id)sender
{
	if (!fullscreen)
		[self setFrameSize:1.0];
}

- (void)setDoubleSize:(id)sender
{
	if (!fullscreen)
		[self setFrameSize:2.0];
}

- (void)fitToScreen:(id)sender
{
	if (!fullscreen)
		[self setFrameSize:1000.0];
}

@end
