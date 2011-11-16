
/**
 * OpenEmulator
 * Mac OS X Canvas Window Controller
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas window
 */

#import "CanvasWindowController.h"
#import "CanvasWindow.h"

@implementation CanvasWindowController

- (id)initWithDevice:(void *)theDevice
               title:(NSString *)theTitle
              canvas:(void *)theCanvas
{
    self = [self initWithWindowNibName:@"Canvas"];
    
    if (self)
    {
        device = theDevice;
        title = [theTitle copy];
        canvas = theCanvas;
    }
    
    return self;
}

- (void)dealloc
{
    [title release];
    
    [super dealloc];
}

- (void *)device
{
    return device;
}

- (void *)canvas
{
    return canvas;
}

- (CanvasView *)canvasView
{
    return fCanvasView;
}

// User Interface

- (void)awakeFromNib
{
    if ([fCanvasView isPaperCanvas])
        [fScrollView setHasVerticalScroller:YES];
    else
    {
        [fToolbarView removeFromSuperview];
        [fScrollView setFrame:[[fScrollView superview] frame]];
    }
}

- (void)windowDidLoad
{
    NSToolbar *toolbar;
    toolbar = [[NSToolbar alloc] initWithIdentifier:@"Canvas Toolbar"];
    [toolbar setDelegate:self];
    [toolbar setAllowsUserCustomization:YES];
    [toolbar setAutosavesConfiguration:YES];
    [[self window] setToolbar:toolbar];
    [toolbar release];
    
    [[self window] setDelegate:self];
    
    CanvasWindow *window = (CanvasWindow *)[self window];
    NSRect windowFrame = [window frame];
    
    NSSize defaultViewSize = [fCanvasView defaultViewSize];
    [window setContentAspectRatio:NSMakeSize(defaultViewSize.width,
                                             defaultViewSize.height)];
    
    NSSize minSize = NSMakeSize(0.5 * defaultViewSize.width,
                                0.5 * defaultViewSize.height);
    [window setContentMinSize:minSize];
    
    [self setActualSize:self];
    [window setFrameTopLeftPoint:NSMakePoint(NSMinX(windowFrame),
                                             NSMaxY(windowFrame))];
}

- (void)showWindow:(id)sender
{
    [super showWindow:sender];
    
    [fCanvasView startDisplayLink];
}

- (void)close
{
    CanvasWindow *window = (CanvasWindow *)[self window];
    
    [window leaveFullscreen];
    
    if ([self isWindowLoaded])
    {
        [fCanvasView stopDisplayLink];
        [fCanvasView freeOpenGL];
    }
    
    isClosing = YES;
    
    [super close];
}

- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName
{
    return [NSString stringWithFormat:@"%@ - %@", displayName, title];
}

- (void)pasteString:(NSString *)text
{
    [fCanvasView pasteString:text];
}

- (BOOL)validateUserInterfaceItem:(id)anItem
{
    CanvasWindow *window = (CanvasWindow *)[self window];
    BOOL fullscreen = [window isFullscreen];
    BOOL isResizable = !fullscreen && [fCanvasView isDisplayCanvas];
    
    if ([anItem action] == @selector(toggleToolbarShown:))
    {
        NSString *theTitle = ([[window toolbar] isVisible] ?
                              NSLocalizedString(@"Hide Toolbar",
                                                @"Main Menu.") :
                              NSLocalizedString(@"Show Toolbar",
                                                @"Main Menu."));
        [anItem setTitle:theTitle];
        
        return YES;
    }
    else if ([anItem action] == @selector(toggleFullscreen:))
    {
        NSString *theTitle = (fullscreen ?
                              NSLocalizedString(@"Exit Fullscreen",
                                                @"Main Menu.") :
                              NSLocalizedString(@"Enter Fullscreen",
                                                @"Main Menu."));
        [anItem setTitle:theTitle];
        
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
    
    return YES;
}

// Toolbar

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
     itemForItemIdentifier:(NSString *)ident
 willBeInsertedIntoToolbar:(BOOL)flag
{
    NSToolbarItem *item;
    item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
    if (!item)
        return nil;
    
    if ([ident isEqualToString:@"Power Down"])
    {
        [item setLabel:NSLocalizedString(@"Power Down",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Power Down",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Initiate power-down.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconPowerDown.png"]];
        [item setAction:@selector(sendPowerDown:)];
    }
    else if ([ident isEqualToString:@"Sleep"])
    {
        [item setLabel:NSLocalizedString(@"Sleep",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Sleep",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Initiate low power mode.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconSleep.png"]];
        [item setAction:@selector(sendSleep:)];
    }
    else if ([ident isEqualToString:@"Wake Up"])
    {
        [item setLabel:NSLocalizedString(@"Wake Up",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Wake Up",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Initiate full power state.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconWakeUp.png"]];
        [item setAction:@selector(sendWakeUp:)];
    }
    else if ([ident isEqualToString:@"Cold Restart"])
    {
        [item setLabel:NSLocalizedString(@"Restart",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Restart",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Restart the system at the most "
                                           "primitive level.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconColdRestart.png"]];
        [item setAction:@selector(sendColdRestart:)];
    }
    else if ([ident isEqualToString:@"Warm Restart"])
    {
        [item setLabel:NSLocalizedString(@"Reset",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Reset",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Restart the operating system.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconWarmRestart.png"]];
        [item setAction:@selector(sendWarmRestart:)];
    }
    else if ([ident isEqualToString:@"Debugger Break"])
    {
        [item setLabel:NSLocalizedString(@"Debugger Break",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Debugger Break",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Break into the operating system debugger.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconDebuggerBreak.png"]];
        [item setAction:@selector(sendDebuggerBreak:)];
    }
    else if ([ident isEqualToString:@"AudioControls"])
    {
        [item setLabel:NSLocalizedString(@"Audio Controls",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Audio Controls",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Show or hide audio controls.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconAudio.png"]];
        [item setAction:@selector(toggleAudioControls:)];
    }
    else if ([ident isEqualToString:@"Devices"])
    {
        [item setLabel:NSLocalizedString(@"Devices",
                                         @"Canvas Toolbar Label.")];
        [item setPaletteLabel:NSLocalizedString(@"Devices",
                                                @"Canvas Toolbar Palette Label.")];
        [item setToolTip:NSLocalizedString(@"Show devices.",
                                           @"Canvas Toolbar Tool Tip.")];
        [item setImage:[NSImage imageNamed:@"IconDevices.png"]];
        [item setAction:@selector(showEmulation:)];
        [item setVisibilityPriority:NSToolbarItemVisibilityPriorityHigh];
    }
    [item autorelease];
    
    return item;
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
    return [NSArray arrayWithObjects:
            @"Power Down",
            @"Sleep",
            @"Wake Up",
            @"Warm Restart",
            NSToolbarFlexibleSpaceItemIdentifier,
            @"AudioControls",
            @"Devices",
            nil];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
    return [NSArray arrayWithObjects:
            @"Power Down",
            @"Sleep",
            @"Wake Up",
            @"Cold Restart",
            @"Warm Restart",
            @"Debugger Break",
            @"AudioControls",
            @"Devices",
            NSToolbarSeparatorItemIdentifier,
            NSToolbarSpaceItemIdentifier,
            NSToolbarFlexibleSpaceItemIdentifier,
            nil];
}

// Delegate

- (void)windowDidResize:(NSNotification *)notification
{
    [fCanvasView windowDidResize];
}

- (void)windowWillClose:(NSNotification *)notification
{
    [fCanvasView stopDisplayLink];
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
    if (isClosing)
        [fCanvasView windowDidBecomeKey];
}

- (void)windowDidResignKey:(NSNotification *)notification
{
    if (!isClosing)
        [fCanvasView windowDidResignKey];
}

- (NSApplicationPresentationOptions)window:(NSWindow *)window
      willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
    return (proposedOptions | NSApplicationPresentationAutoHideToolbar);
}

- (void)scaleFrame:(float)scale
{
    NSSize defaultViewSize = [fCanvasView defaultViewSize];
    float defaultViewRatio = defaultViewSize.width / defaultViewSize.height;
    
    NSWindow *window = [self window];
    NSSize frameSize = [window frame].size;
    NSSize viewSize = [[window contentView] frame].size;
    float userScale = [window userSpaceScaleFactor];
    NSSize titleSize = NSMakeSize(frameSize.width - viewSize.width * userScale,
                                  frameSize.height - viewSize.height * userScale);
    
    NSSize proposedSize = NSMakeSize(scale * userScale * defaultViewSize.width,
                                     scale * userScale * defaultViewSize.height);
    float proposedRatio = proposedSize.width / proposedSize.height;
    
    NSRect screenRect = [[window screen] visibleFrame];
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
    
    NSRect frameRect = [window frame];
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
    
    [window setFrame:frameRect display:YES animate:YES];
    
    if (displayLinkRunning)
        [fCanvasView startDisplayLink];
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
