
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

- (NSString *)windowNibName
{
    return @"Document";
}

- (void)windowDidLoad
{
	[self setDocumentEdited:TRUE];
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
	NSWindow * window = [self window];
	NSRect windowFrame = [window frame];
	NSView * view = [window contentView];
	NSRect viewFrame = [view frame];
	NSScreen * screen = [window screen];
	NSRect screenFrame = [screen visibleFrame];
	
	float deltaWidth = windowFrame.size.width - viewFrame.size.width;
	float deltaHeight = windowFrame.size.height - viewFrame.size.height;
	float scale = [window userSpaceScaleFactor];
	
	windowFrame.origin.x += windowFrame.size.width / 2;
	windowFrame.origin.y += windowFrame.size.height;
	windowFrame.size.width = scale * (proportion * DEFAULT_FRAME_WIDTH + deltaWidth);
	windowFrame.size.height = scale * (proportion * DEFAULT_FRAME_HEIGHT + deltaHeight);
	windowFrame.origin.x -= windowFrame.size.width / 2;
	windowFrame.origin.y -= windowFrame.size.height;
	
	float maxX = screenFrame.origin.x + screenFrame.size.width - windowFrame.size.width;
	float maxY = screenFrame.origin.y + screenFrame.size.height - windowFrame.size.height;
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
	NSWindow *window = [self window];
	NSRect windowFrame = [window frame];
	NSView *view = [window contentView];
	NSRect viewFrame = [view frame];
	NSScreen *screen = [window screen];
	NSRect screenFrame = [screen frame];
	
	NSRect frame;
	frame.origin.x = windowFrame.origin.x;
	frame.origin.y = windowFrame.origin.y;
	frame.size.width = viewFrame.size.width;
	frame.size.height = viewFrame.size.height;
	
	NSWindow *fullscreenWindow = [[NSWindow alloc] initWithContentRect:frame
															 styleMask:NSBorderlessWindowMask
															   backing:NSBackingStoreBuffered
																 defer:YES];
    SetSystemUIMode(kUIModeAllHidden, kUIOptionAutoShowMenuBar);
	[fullscreenWindow setContentView:view];
    [fullscreenWindow makeKeyAndOrderFront:nil];
	[fullscreenWindow setFrame:screenFrame display:YES animate:YES];
	
//	[window setContentView:nil];
	
/*	NSWindow * window = [view window];
	
//    normalFrame = [self frame];
    [window setFrame:[[window screen] frame] display:YES animate:YES];

//	[self fitToScreen:nil];

/*    CGAcquireDisplayFadeReservation(25, &tok);
    CGDisplayFade(tok, 0.5, kCGDisplayBlendNormal, kCGDisplayBlendSolidColor, 0, 0, 0, TRUE);
	
/*	NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
							 [NSNumber numberWithBool:NO],
							 NSFullScreenModeAllScreens, nil];
	NSWindow *window = [self window];
	NSView *view = [window contentView];
*/	
	
	/*	if ([view isInFullScreenMode])
		[view exitFullScreenModeWithOptions:options];
	else
		[view enterFullScreenMode:[window screen] withOptions:options];*/
	
/*	CGDisplayFade(tok, 0.5, kCGDisplayBlendSolidColor, kCGDisplayBlendNormal, 0, 0, 0, TRUE);
	CGReleaseDisplayFadeReservation(tok);
 */
}

@end
