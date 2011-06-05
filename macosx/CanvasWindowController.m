
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
	[window scaleFrame:1.0];
	[window setFrameTopLeftPoint:NSMakePoint(NSMinX(windowFrame),
											 NSMaxY(windowFrame))];
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
