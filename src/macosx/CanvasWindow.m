
/**
 * OpenEmulator
 * Mac OS X Canvas Window
 * (C) 2009-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles canvas window messages
 */

#import "CanvasWindow.h"

#import "DocumentController.h"

@implementation CanvasWindow

- (BOOL)isOnLionOrBetter
{
    return [self respondsToSelector:@selector(toggleFullScreen:)];
}

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
        
        if ([self isOnLionOrBetter])
            [self setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    }
    
    return self;
}

- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen *)screen
{
    if (![self isOnLionOrBetter] &&
        [self isFullscreen])
        return frameRect;
    
    return [super constrainFrameRect:frameRect toScreen:screen];
}

- (void)setFrameOrigin:(NSPoint)point
{
    if (![self isOnLionOrBetter] &&
        ![self isFullscreen])
        [super setFrameOrigin:point];
}

- (BOOL)isFullscreen
{
    if ([self isOnLionOrBetter])
        return ([self styleMask] & NSFullScreenWindowMask) == NSFullScreenWindowMask;
    else
        return fullscreen;
}

- (void)toggleFullscreen:(id)sender
{
    if ([self isOnLionOrBetter])
        [self toggleFullScreen:nil];
    else
    {
        DocumentController *documentController;
        documentController = [NSDocumentController sharedDocumentController];
        
        [fCanvasView stopDisplayLink];
        
        if (![self isFullscreen])
        {
            [documentController disableMenuBar];
            
            windowRect = [self frame];
            NSRect contentRect = [[self contentView] frame];
            CGFloat titlebarHeight = (NSHeight(windowRect) -
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
    }
}

- (void)leaveFullscreen
{
    if (![self isOnLionOrBetter])
    {
        if ([self isFullscreen])
        {
            DocumentController *documentController;
            documentController = [NSDocumentController sharedDocumentController];
            
            [documentController enableMenuBar];
            
            fullscreen = NO;
        }
    }
}

@end
