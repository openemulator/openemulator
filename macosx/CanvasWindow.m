
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
//	NSLog(@"CanvasWindow constrainFrameRect");
	
	return frameRect;
}

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
{
//	NSLog(@"CanvasWindow windowWillResize");
	
	if (!fullscreen)
	{
		NSSize defaultViewSize = [fCanvasView defaultViewSize];
		NSSize frameSize = [self frame].size;
		NSSize viewSize = [[self contentView] frame].size;
		float scale = [self userSpaceScaleFactor];
		
		NSSize titleSize = NSMakeSize(frameSize.width - viewSize.width * scale,
									  frameSize.height - viewSize.height * scale);
		
		float defaultViewRatio = defaultViewSize.width / defaultViewSize.height;
		
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
		
		return NSMakeSize(proposedSize.width + titleSize.width,
						  proposedSize.height + titleSize.height);
	}
	else
		return [window frame].size;
}

/*- (BOOL)windowShouldClose:(id)sender
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
}*/

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
	NSSize defaultViewSize = [fCanvasView defaultViewSize];
	NSRect frameRect = [self frame];
	NSSize viewSize = [[self contentView] frame].size;
	NSRect screenRect = [[self screen] visibleFrame];
	float scale = [self userSpaceScaleFactor];
	
	NSSize titleSize = NSMakeSize(frameRect.size.width - viewSize.width * scale,
								  frameRect.size.height - viewSize.height * scale);
	
	float viewRatio = viewSize.width / viewSize.height;
	
	NSSize proposedViewSize = NSMakeSize(proportion * scale * defaultViewSize.width,
										 proportion * scale * defaultViewSize.height);
	
	NSSize maxSize = NSMakeSize(screenRect.size.width - titleSize.width,
								screenRect.size.height - titleSize.height);
	
	if (proposedViewSize.width > maxSize.width)
	{
		proposedViewSize.width = maxSize.width;
		proposedViewSize.height = proposedViewSize.width / viewRatio;
	}
	if (proposedViewSize.height > maxSize.height)
	{
		proposedViewSize.height = maxSize.height;
		proposedViewSize.width = proposedViewSize.height * viewRatio;
	}
	
	NSSize proposedFrameSize = NSMakeSize(titleSize.width + proposedViewSize.width,
										  titleSize.height + proposedViewSize.height);
	
	frameRect.origin.x = (int) NSMidX(frameRect) - proposedFrameSize.width / 2;
	frameRect.origin.y = NSMaxY(frameRect) - proposedFrameSize.height;
	frameRect.size = proposedFrameSize;
	
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
