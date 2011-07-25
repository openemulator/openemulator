
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
    self = [super initWithContentRect:contentRect
                            styleMask:windowStyle
                              backing:bufferingType
                                defer:deferCreation];
    
	if (self)
    {
		fullscreen = NO;
        
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_7
        [self setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
#endif
    }
	
	return self;
}

- (BOOL)validateUserInterfaceItem:(id)anItem
{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_7
    fullscreen = ([self styleMask] & NSFullScreenWindowMask) == NSFullScreenWindowMask;
#endif
	BOOL isResizable = !fullscreen && [fCanvasView isDisplayCanvas];
	
	if ([anItem action] == @selector(toggleFullscreen:))
	{
		NSString *title = (fullscreen ?
						   NSLocalizedString(@"Exit Fullscreen",
											 @"Main Menu.") :
						   NSLocalizedString(@"Enter Fullscreen",
											 @"Main Menu."));
		[anItem setTitle:title];
		
		return [fCanvasView isDisplayCanvas];
	}
	else if ([anItem action] == @selector(setHalfSize:))
		return isResizable;
	else if ([anItem action] == @selector(setActualSize:))
		return isResizable;
	else if ([anItem action] == @selector(setDoubleSize:))
		return isResizable;
	else if ([anItem action] == @selector(fitToScreen:))
		return isResizable;
	else if ([anItem action] == @selector(performZoom:))
		return !fullscreen;
	else if ([anItem action] == @selector(toggleToolbarShown:))
		return !fullscreen;
	else if ([anItem action] == @selector(runToolbarCustomizationPalette:))
		return !fullscreen;
	
	return [super validateUserInterfaceItem:anItem];
}

#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_7
- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen
{
	if (fullscreen)
		return frameRect;
	
	return [super constrainFrameRect:frameRect toScreen:screen];
}

- (void)setFrameOrigin:(NSPoint)point
{
	if (!fullscreen)
		[super setFrameOrigin:point];
}
#endif

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
{
	if ([fCanvasView isDisplayCanvas])
	{
		NSSize defaultViewSize = [fCanvasView defaultViewSize];
		float defaultViewRatio = defaultViewSize.width / defaultViewSize.height;
		
		NSSize frameSize = [self frame].size;
		NSSize viewSize = [[self contentView] frame].size;
		float userScale = [self userSpaceScaleFactor];
		NSSize titleSize = NSMakeSize(frameSize.width - viewSize.width * userScale,
									  frameSize.height - viewSize.height * userScale);
		
		proposedFrameSize = NSMakeSize(proposedFrameSize.width - titleSize.width,
									   proposedFrameSize.height - titleSize.height);
		float proposedFrameRatio = proposedFrameSize.width / proposedFrameSize.height;
		
		if (defaultViewRatio > proposedFrameRatio)
		{
			float minWidth = 0.5 * defaultViewSize.width;
			if (proposedFrameSize.width < minWidth)
				proposedFrameSize.width = minWidth;
			proposedFrameSize.height = proposedFrameSize.width / defaultViewRatio;
		}
		else
		{
			float minHeight = 0.5 * defaultViewSize.height;
			if (proposedFrameSize.height < minHeight)
				proposedFrameSize.height = minHeight;
			proposedFrameSize.width = proposedFrameSize.height * defaultViewRatio;
		}
		
		proposedFrameSize = NSMakeSize(round(proposedFrameSize.width + titleSize.width),
									   round(proposedFrameSize.height + titleSize.height));
	}
	else if ([fCanvasView isPaperCanvas])
	{
		if (proposedFrameSize.width < 512)
			proposedFrameSize.width = 512;
		if (proposedFrameSize.height < 384)
			proposedFrameSize.height = 384;
	}
	
	return proposedFrameSize;
}

- (BOOL)windowShouldClose:(id)sender
{
#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_7
	if (fullscreen)
	{
		DocumentController *documentController;
		documentController = [NSDocumentController sharedDocumentController];
		
		[documentController enableMenuBar];
		
		fullscreen = NO;
	}
#endif
	return YES;
}

- (void)scaleFrame:(float)scale
{
	NSSize defaultViewSize = [fCanvasView defaultViewSize];
	float defaultViewRatio = defaultViewSize.width / defaultViewSize.height;
	
	NSSize frameSize = [self frame].size;
	NSSize viewSize = [[self contentView] frame].size;
	float userScale = [self userSpaceScaleFactor];
	NSSize titleSize = NSMakeSize(frameSize.width - viewSize.width * userScale,
								  frameSize.height - viewSize.height * userScale);
	
	NSSize proposedSize = NSMakeSize(scale * userScale * defaultViewSize.width,
									 scale * userScale * defaultViewSize.height);
	float proposedRatio = proposedSize.width / proposedSize.height;
	
	NSRect screenRect = [[self screen] visibleFrame];
	NSSize maxSize = NSMakeSize(screenRect.size.width - titleSize.width,
								screenRect.size.height - titleSize.height);
	
	if (defaultViewRatio > proposedRatio)
	{
		if (proposedSize.width > maxSize.width)
			proposedSize.width = maxSize.width;
		proposedSize.height = proposedSize.width / defaultViewRatio;
	}
	else
	{
		if (proposedSize.height > maxSize.height)
			proposedSize.height = maxSize.height;
		proposedSize.width = proposedSize.height * defaultViewRatio;
	}
	
	proposedSize.width += titleSize.width;
	proposedSize.height += titleSize.height;
	
	NSRect frameRect = [self frame];
	frameRect.origin.x = round(NSMidX(frameRect) - proposedSize.width / 2);
	frameRect.origin.y = round(NSMaxY(frameRect) - proposedSize.height);
	frameRect.size = proposedSize;
	
	if (NSMaxX(frameRect) > NSMaxX(screenRect))
		frameRect.origin.x = NSMaxX(screenRect) - NSWidth(frameRect);
	if (NSMaxY(frameRect) > NSMaxY(screenRect))
		frameRect.origin.y = NSMaxY(screenRect) - NSHeight(frameRect);
	if (NSMinX(frameRect) < NSMinX(screenRect))
		frameRect.origin.x = NSMinX(screenRect);
	if (NSMinY(frameRect) < NSMinY(screenRect))
		frameRect.origin.y = NSMinY(screenRect);
	
	BOOL displayLinkRunning = [fCanvasView displayLinkRunning];
	if (displayLinkRunning)
		[fCanvasView stopDisplayLink];
	
	[self setFrame:frameRect display:YES animate:YES];
	
	if (displayLinkRunning)
		[fCanvasView startDisplayLink];
}

- (void)toggleFullscreen:(id)sender
{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_7
    [self toggleFullScreen:nil];
#else
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	[fCanvasView stopDisplayLink];
	
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
		
		fullscreen = YES;
		
		[self setFrame:screenRect
			   display:YES
			   animate:YES];
	}
	else
	{
		[self setFrame:windowRect
			   display:YES
			   animate:YES];
		
		[documentController enableMenuBar];
		
		fullscreen = NO;
	}
	
	[fCanvasView startDisplayLink];
#endif
}

- (void)setHalfSize:(id)sender
{
	[self scaleFrame:0.5];
}

- (void)setActualSize:(id)sender
{
	[self scaleFrame:1.0];
}

- (void)setDoubleSize:(id)sender
{
	[self scaleFrame:2.0];
}

- (void)fitToScreen:(id)sender
{
	[self scaleFrame:10000.0];
}

@end
