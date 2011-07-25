
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

- (BOOL)isFullscreen
{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_7
    return ([self styleMask] & NSFullScreenWindowMask) == NSFullScreenWindowMask;
#else
    return fullscreen;
#endif
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

@end
