
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
	NSLog(@"CanvasWindow init");
	
	if (self = [super initWithContentRect:contentRect
								styleMask:windowStyle
								  backing:bufferingType
									defer:deferCreation])
		fullscreen = NO;
	
	return self;
}

- (void)dealloc
{
	NSLog(@"CanvasWindow dealloc");
	
	[super dealloc];
}

- (BOOL)validateUserInterfaceItem:(id)anItem
{
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
		return isResizable;
	else if ([anItem action] == @selector(toggleToolbarShown:))
		return isResizable;
	else if ([anItem action] == @selector(runToolbarCustomizationPalette:))
		return isResizable;
	
	return [super validateUserInterfaceItem:anItem];
}

- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen
{
//	NSLog(@"CanvasWindow constrainFrameRect");
	
	if (fullscreen)
		return frameRect;
	
	return [super constrainFrameRect:frameRect toScreen:screen];
}

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
{
	NSLog(@"CanvasWindow windowWillResize");
	
	if (fullscreen)
		return proposedFrameSize;
	
	if ([fCanvasView isDisplayCanvas])
	{
		NSSize defaultViewSize = [fCanvasView defaultViewSize];
		float defaultViewRatio = defaultViewSize.width / defaultViewSize.height;
		
		NSSize frameSize = [self frame].size;
		NSSize viewSize = [[self contentView] frame].size;
		float userScale = [self userSpaceScaleFactor];
		NSSize titleSize = NSMakeSize(frameSize.width - viewSize.width * userScale,
									  frameSize.height - viewSize.height * userScale);
		
		NSSize proposedSize = NSMakeSize(proposedFrameSize.width - titleSize.width,
										 proposedFrameSize.height - titleSize.height);
		float proposedRatio = proposedSize.width / proposedSize.height;
		
		if (defaultViewRatio > proposedRatio)
		{
			float minWidth = 0.5 * defaultViewSize.width;
			if (proposedSize.width < minWidth)
				proposedSize.width = minWidth;
			proposedSize.height = proposedSize.width / defaultViewRatio;
		}
		else
		{
			float minHeight = 0.5 * defaultViewSize.height;
			if (proposedSize.height < minHeight)
				proposedSize.height = minHeight;
			proposedSize.width = proposedSize.height * defaultViewRatio;
		}
		
		return NSMakeSize((int) (proposedSize.width + titleSize.width),
						  (int) (proposedSize.height + titleSize.height));
	}
	else
		return proposedFrameSize;
}

- (BOOL)windowShouldClose:(id)sender
{
	NSLog(@"CanvasWindow windowShouldClose");
	
	if (fullscreen)
	{
		DocumentController *documentController;
		documentController = [NSDocumentController sharedDocumentController];
		
		[documentController enableMenuBar];
		
		fullscreen = NO;
	}
	
	return YES;
}

- (void)setFrameOrigin:(NSPoint)point
{
	if (!fullscreen)
		[super setFrameOrigin:point];
}

- (void)setFrameScale:(float)scale
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
	frameRect.origin.x = (int) (NSMidX(frameRect) - proposedSize.width / 2);
	frameRect.origin.y = (int) (NSMaxY(frameRect) - proposedSize.height);
	frameRect.size = proposedSize;
	
	if (NSMaxX(frameRect) > NSMaxX(screenRect))
		frameRect.origin.x = NSMaxX(screenRect) - NSWidth(frameRect);
	if (NSMaxY(frameRect) > NSMaxY(screenRect))
		frameRect.origin.y = NSMaxY(screenRect) - NSHeight(frameRect);
	if (NSMinX(frameRect) < NSMinX(screenRect))
		frameRect.origin.x = NSMinX(screenRect);
	if (NSMinY(frameRect) < NSMinY(screenRect))
		frameRect.origin.y = NSMinY(screenRect);
	
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
}

- (void)setHalfSize:(id)sender
{
	[self setFrameScale:0.5];
}

- (void)setActualSize:(id)sender
{
	[self setFrameScale:1.0];
}

- (void)setDoubleSize:(id)sender
{
	[self setFrameScale:2.0];
}

- (void)fitToScreen:(id)sender
{
	[self setFrameScale:10000.0];
}

@end
