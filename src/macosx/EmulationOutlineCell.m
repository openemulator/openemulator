
/**
 * OpenEmulator
 * Mac OS X Emulation Outline Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation outline cell
 */

#import "EmulationOutlineCell.h"
#import "EmulationOutlineView.h"
#import "EmulationItem.h"

#define CELL_RIGHT_PADDING      10
#define IMAGE_SIZE				16
#define IMAGE_RIGHT_PADDING		3
#define BUTTON_LEFT_PADDING		2
#define BUTTON_SIZE				18

@implementation EmulationOutlineCell

- (void)setButtonRollover:(BOOL)rollover
{
    buttonRollover = rollover;
}

- (NSRect)imageRectForBounds:(NSRect)bounds
{
    EmulationItem *item = [self representedObject];
    if (![item image])
        return NSZeroRect;
    
    NSRect rect = bounds;
    NSRect paddingRect;
    NSDivideRect(rect, &rect, &paddingRect, IMAGE_SIZE, NSMinXEdge);
    return NSInsetRect(rect, 0, (NSHeight(bounds) - IMAGE_SIZE) / 2);
}

- (NSRect)textRectForBounds:(NSRect)bounds
{
    EmulationItem *item = [self representedObject];
    
    NSRect rect = bounds;
    NSRect paddingRect;
    
    
    NSDivideRect(rect, &paddingRect, &rect, CELL_RIGHT_PADDING, NSMaxXEdge);
    if ([item image])
        NSDivideRect(rect, &paddingRect, &rect, IMAGE_SIZE + IMAGE_RIGHT_PADDING, NSMinXEdge);
    if ([item hasCanvases] || [item isMount])
        NSDivideRect(rect, &paddingRect, &rect, BUTTON_LEFT_PADDING + BUTTON_SIZE, NSMaxXEdge);
    
    return NSInsetRect(rect, 0, (NSHeight(bounds) - [self cellSize].height) / 2);
}

- (NSRect)buttonRectForBounds:(NSRect)bounds
{
    EmulationItem *item = [self representedObject];
    
    if (![item hasCanvases] && ![item isMount])
        return NSZeroRect;
    
    NSRect rect = bounds;
    NSRect paddingRect;
    
    NSDivideRect(rect, &paddingRect, &rect, CELL_RIGHT_PADDING, NSMaxXEdge);
    NSDivideRect(rect, &rect, &paddingRect, BUTTON_SIZE, NSMaxXEdge);
    
    return NSInsetRect(rect, 0, (NSHeight(bounds) - BUTTON_SIZE) / 2);
}

- (void)drawImage:(NSImage *)theImage inRect:(NSRect)rect isFlipped:(BOOL)flipped
{
    BOOL wasFlipped = [theImage isFlipped];
    
    [theImage setFlipped:flipped];
    [theImage drawInRect:rect
                fromRect:NSZeroRect
               operation:NSCompositeSourceOver
                fraction:1];
    
    [theImage setFlipped:wasFlipped];
}

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
    EmulationItem *item = [self representedObject];
    
    [self drawImage:[item image]
             inRect:[self imageRectForBounds:cellFrame]
          isFlipped:[controlView isFlipped]];
    
    NSString *name = @"";
    if ([item hasCanvases])
        name = @"EmulationShow";
    else if ([item isMount])
        name = @"EmulationUnmount";
    
    if (buttonPressed)
        name = [name stringByAppendingString:@"Pressed"];
    else if (buttonRollover)
        name = [name stringByAppendingString:@"Rollover"];
    else if ([self backgroundStyle] == NSBackgroundStyleDark)
        name = [name stringByAppendingString:@"Selected"];
    
    [self drawImage:[NSImage imageNamed:name]
             inRect:[self buttonRectForBounds:cellFrame]
          isFlipped:[controlView isFlipped]];
    
    [super drawWithFrame:[self textRectForBounds:cellFrame]
                  inView:controlView];
}

- (NSUInteger)hitTestForEvent:(NSEvent *)event
                       inRect:(NSRect)cellFrame
                       ofView:(NSView *)controlView
{
    NSPoint point = [controlView convertPoint:[event locationInWindow]
                                     fromView:nil];
    
    if (NSMouseInRect(point, [self buttonRectForBounds:cellFrame],
                      [controlView isFlipped]))
        return NSCellHitContentArea | NSCellHitTrackableArea;
    
    return NSCellHitContentArea;
}

+ (BOOL)prefersTrackingUntilMouseUp
{
    return YES;
}

- (BOOL)trackMouse:(NSEvent *)theEvent
            inRect:(NSRect)cellFrame
            ofView:(NSView *)controlView
      untilMouseUp:(BOOL)flag
{
    [self setControlView:controlView];
    
    [(EmulationOutlineView *)controlView removeTrackingAreas];
    [self setButtonRollover:NO];
    
    NSRect buttonRect = [self buttonRectForBounds:cellFrame];
    while ([theEvent type] != NSLeftMouseUp)
    {
        NSPoint mouseLocation = [controlView convertPoint:[theEvent locationInWindow]
                                                 fromView:nil];
        BOOL theButtonPressed = NSMouseInRect(mouseLocation, buttonRect,
                                              [controlView isFlipped]);
        if (buttonPressed != theButtonPressed)
        {
            buttonPressed = theButtonPressed;
            [controlView setNeedsDisplayInRect:cellFrame];
        }
        if ([theEvent type] == NSMouseEntered || [theEvent type] == NSMouseExited)
            [NSApp sendEvent:theEvent];
        
        theEvent = [[controlView window] nextEventMatchingMask:(NSLeftMouseUpMask |
                                                                NSLeftMouseDraggedMask |
                                                                NSMouseEnteredMask |
                                                                NSMouseExitedMask)];
    }
    
    if (buttonPressed)
    {
        buttonPressed = NO;
        [controlView setNeedsDisplayInRect:cellFrame];
        if (!buttonPressed)
            [NSApp sendAction:[self action] to:[self target] from:[self controlView]];
    }
    
    [controlView updateTrackingAreas];
    
    return YES;
}

- (void)addTrackingAreasForView:(NSView *)controlView
                         inRect:(NSRect)cellFrame
                   withUserInfo:(NSDictionary *)userInfo
                  mouseLocation:(NSPoint)mouseLocation
{
    NSRect buttonRect = [self buttonRectForBounds:cellFrame];
    
    NSTrackingAreaOptions options = (NSTrackingEnabledDuringMouseDrag |
                                     NSTrackingMouseEnteredAndExited |
                                     NSTrackingActiveInActiveApp);
    
    if (NSMouseInRect(mouseLocation, buttonRect, [controlView isFlipped]))
    {
        options |= NSTrackingAssumeInside;
        NSInteger row = [[userInfo objectForKey:@"row"] integerValue];
        [(EmulationOutlineView *)controlView setTrackedRow:row];
        [controlView setNeedsDisplayInRect:cellFrame];
    }
    
    NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:buttonRect
                                                        options:options
                                                          owner:controlView
                                                       userInfo:userInfo];
    [controlView addTrackingArea:area];
    [area release];
}

@end
