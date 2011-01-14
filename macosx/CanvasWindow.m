
/**
 * OpenEmulator
 * Mac OS X Canvas Window
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
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
		fullscreen = NO;
	
	return self;
}

- (BOOL)validateUserInterfaceItem:(id)anItem
{
	if ([anItem action] == @selector(toggleFullscreen:))
	{
		NSString *title = (fullscreen ?
						   NSLocalizedString(@"Exit Fullscreen",
											 @"Main Menu.") :
						   NSLocalizedString(@"Enter Fullscreen",
											 @"Main Menu."));
		[anItem setTitle:title];
		
		return YES;
	}
	else if ([anItem action] == @selector(setHalfSize:))
		return !fullscreen;
	else if ([anItem action] == @selector(setActualSize:))
		return !fullscreen;
	else if ([anItem action] == @selector(setDoubleSize:))
		return !fullscreen;
	else if ([anItem action] == @selector(fitToScreen:))
		return !fullscreen;
	else if ([anItem action] == @selector(performZoom:))
		return !fullscreen;
	else if ([anItem action] == @selector(toggleToolbarShown:))
		return !fullscreen;
	else if ([anItem action] == @selector(runToolbarCustomizationPalette:))
		return !fullscreen;
	
	return [super validateUserInterfaceItem:anItem];
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

- (BOOL)windowShouldClose:(id)sender
{
	if (fullscreen)
		[self toggleFullscreen:self];
	
	return YES;
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

- (void)setFrameSize:(double)proportion
{
	NSSize canvasSize = [fCanvasView canvasSize];
	
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
	frameRect.size.width = scale * proportion * canvasSize.width + deltaWidth;
	frameRect.size.height = scale * proportion * canvasSize.height + deltaHeight;
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
	[self setFrameSize:1000.0];
}

@end
