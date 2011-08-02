
/**
 * OpenEmulator
 * Mac OS X Canvas Window Controller
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas window.
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

- (void *)canvas
{
	return canvas;
}

- (void)closeWindow
{
	[fCanvasView freeOpenGL];
    
	CanvasWindow *window = (CanvasWindow *)[self window];
    [window leaveFullscreen];
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
	
	CanvasWindow *window = (CanvasWindow *)[self window];
	NSRect windowFrame = [window frame];
	[self setActualSize:self];
	[window setFrameTopLeftPoint:NSMakePoint(NSMinX(windowFrame),
											 NSMaxY(windowFrame))];
    
    [[self window] setDelegate:self];
}

- (void)showWindow:(id)sender
{
	[super showWindow:sender];
	
	[fCanvasView startDisplayLink];
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

- (NSSize)windowWillResize:(NSWindow *)window toSize:(NSSize)proposedFrameSize
{
	if ([fCanvasView isDisplayCanvas])
	{
		NSSize defaultViewSize = [fCanvasView defaultViewSize];
		float defaultViewRatio = defaultViewSize.width / defaultViewSize.height;
		
        NSWindow *window = [self window];
		NSSize frameSize = [window frame].size;
		NSSize viewSize = [[window contentView] frame].size;
		float userScale = [window userSpaceScaleFactor];
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

- (void)windowDidResize:(NSNotification *)notification
{
    [fCanvasView windowDidResize:notification];
}

- (void)windowWillClose:(NSNotification *)notification
{
    [fCanvasView windowWillClose:notification];
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
		[item setAction:@selector(systemPowerDown:)];
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
		[item setAction:@selector(systemSleep:)];
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
		[item setAction:@selector(systemWakeUp:)];
	}
	else if ([ident isEqualToString:@"Cold Restart"])
	{
		[item setLabel:NSLocalizedString(@"Cold Restart",
										 @"Canvas Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Cold Restart",
												@"Canvas Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Restart the system at the most "
										   "primitive level.",
										   @"Canvas Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconColdRestart.png"]];
		[item setAction:@selector(systemColdRestart:)];
	}
	else if ([ident isEqualToString:@"Warm Restart"])
	{
		[item setLabel:NSLocalizedString(@"Warm Restart",
										 @"Canvas Toolbar Label.")];
		[item setPaletteLabel:NSLocalizedString(@"Warm Restart",
												@"Canvas Toolbar Palette Label.")];
		[item setToolTip:NSLocalizedString(@"Restart the operating system.",
										   @"Canvas Toolbar Tool Tip.")];
		[item setImage:[NSImage imageNamed:@"IconWarmRestart.png"]];
		[item setAction:@selector(systemWarmRestart:)];
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
		[item setAction:@selector(systemDebuggerBreak:)];
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

// System events

- (void)systemPowerDown:(id)sender
{
	[[self document] sendSystemEvent:DOCUMENT_POWERDOWN
							toDevice:device];
}

- (void)systemSleep:(id)sender
{
	[[self document] sendSystemEvent:DOCUMENT_SLEEP
							toDevice:device];
}

- (void)systemWakeUp:(id)sender
{
	[[self document] sendSystemEvent:DOCUMENT_WAKEUP
							toDevice:device];
}

- (void)systemColdRestart:(id)sender
{
	[[self document] sendSystemEvent:DOCUMENT_COLDRESTART
							toDevice:device];
}

- (void)systemWarmRestart:(id)sender
{
	[[self document] sendSystemEvent:DOCUMENT_WARMRESTART
							toDevice:device];
}

- (void)systemDebuggerBreak:(id)sender
{
	[[self document] sendSystemEvent:DOCUMENT_DEBUGGERBREAK
							toDevice:device];
}

@end
