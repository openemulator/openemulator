
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import "Document.h"
#import "DocumentWindowController.h"
#import "DocumentWindow.h"

#define DEFAULT_FRAME_WIDTH		640
#define DEFAULT_FRAME_HEIGHT	480

@implementation Document

- (id)init
{
	if (self = [super init])
	{
		pasteboard = [NSPasteboard generalPasteboard];
		[pasteboard retain];
		
		pasteboardTypes = [NSArray arrayWithObjects:NSStringPboardType, nil];
		[pasteboardTypes retain];
		
		isFullscreen = NO;
		fullscreenWindow = nil;
	}
	
	return self;
}

- (void)dealloc
{
	[pasteboardTypes release];
	[pasteboard release];
	
	[super dealloc];
}

- (NSString *)windowNibName
{
	return @"Document";
}

- (void)awakeFromNib
{
	NSToolbar *toolbar;
	toolbar = [[NSToolbar alloc] initWithIdentifier:@"Document Toolbar"];
	[toolbar setDelegate:self];
	[toolbar setAllowsUserCustomization:YES];
	[fWindow setToolbar:toolbar];
	[toolbar release];
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
	 itemForItemIdentifier:(NSString *)ident
 willBeInsertedIntoToolbar:(BOOL)flag
{
	NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
	
	if ([ident isEqualToString:@"Power Off"])
	{
		[item setLabel:NSLocalizedString(@"Power Off", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBPower.png"]];
		[item setTarget:self];
		[item setAction:@selector(togglePower:)];
		[item setAutovalidates:NO];
	}
	else if ([ident isEqualToString:@"Reset"])
	{
		[item setLabel:NSLocalizedString(@"Reset", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBReset.png"]];
		[item setTarget:self];
		[item setAction:@selector(resetEmulation:)];
		[item setAutovalidates:NO];
	}
	else if ([ident isEqualToString:@"Pause"])
	{
		[item setLabel:NSLocalizedString(@"Reset", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBPause.png"]];
		[item setTarget:self];
		[item setAction:@selector(togglePause:)];
		[item setAutovalidates:NO];
	}
	else if ([ident isEqualToString:@"Inspector"])
	{
		[item setLabel:NSLocalizedString(@"Inspector", "Preferences -> toolbar item title")];
		[item setImage:[NSImage imageNamed:@"TBInspector.png"]];
		[item setTarget:self];
		[item setAction:@selector(toggleInspectorPanel:)];
		[item setAutovalidates:NO];
	}
	else
	{
		[item release];
		return nil;
	}
	
	return [item autorelease];
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
	return [self toolbarAllowedItemIdentifiers:toolbar];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
	return [NSArray arrayWithObjects:@"Power Off",
			NSToolbarSpaceItemIdentifier,
			@"Reset",
			@"Pause",
			NSToolbarFlexibleSpaceItemIdentifier,
			@"Inspector",
			nil];
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(copy:))
		return [self validateCopy];
    else if ([item action] == @selector(paste:))
		return [self validatePaste];
    else if ([item action] == @selector(startSpeaking:))
		return [self validateCopy];
	else if ([item action] == @selector(setHalfSize:))
		return !isFullscreen;
	else if ([item action] == @selector(setActualSize:))
		return !isFullscreen;
	else if ([item action] == @selector(setDoubleSize:))
		return !isFullscreen;
	else if ([item action] == @selector(fitToScreen:))
		return !isFullscreen;
	else if ([item action] == @selector(toggleFullscreen:))
	{
		NSString *menuTitle = nil;
		
		if (!isFullscreen)
			menuTitle = NSLocalizedString(@"Enter Fullscreen",
										  @"Title for menu item to enter fullscreen"
										  "(should be the same as the initial menu item in the nib).");
		else
			menuTitle = NSLocalizedString(@"Exit Fullscreen",
										  @"Title for menu item to exit fullscreen.");
		
		[item setTitleWithMnemonic:menuTitle];
	}
	
    return YES;
}

- (BOOL)validateCopy
{
	return YES; // To-Do: libemulation
}

- (BOOL)validatePaste
{
	return [pasteboard availableTypeFromArray:pasteboardTypes] != nil;
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
	NSRect windowFrame = [fWindow frame];
	NSView *view = [fWindow contentView];
	NSRect viewFrame = [view frame];
	NSScreen *screen = [fWindow screen];
	NSRect screenFrame = [screen visibleFrame];
	
	float deltaWidth = windowFrame.size.width - viewFrame.size.width;
	float deltaHeight = windowFrame.size.height - viewFrame.size.height;
	float scale = [fWindow userSpaceScaleFactor];
	
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
	
	[fWindow setFrame:windowFrame display:YES animate:YES];
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
	NSView *content = [fWindow contentView];
	NSRect contentFrame = [fWindow contentRectForFrameRect:[fWindow frame]];

	if (!isFullscreen)
	{
		fullscreenWindow = [[DocumentWindow alloc] initWithContentRect:contentFrame
															 styleMask:NSBorderlessWindowMask
															   backing:NSBackingStoreBuffered
																 defer:NO];
		
		[content retain];
		[fWindow setContentView:nil];
		[fullscreenWindow setContentView:content];
		[content release];
		
		[[NSNotificationCenter defaultCenter] postNotification:
		 [NSNotification notificationWithName:@"disableMenuBarNotification"
									   object:self]];
		[fullscreenWindow makeKeyAndOrderFront:self];
		[fullscreenWindow setDelegate:self];
		[fullscreenWindow setFrame:[[fWindow screen] frame]
						   display:YES
						   animate:YES];
		[fWindow orderOut:self];
		
		isFullscreen = YES;
	}
	else
	{
		[fWindow makeKeyAndOrderFront:self];
		[fullscreenWindow makeKeyAndOrderFront:self];
		
		[fullscreenWindow setFrame:contentFrame
						   display:YES
						   animate:YES];
		
		[fWindow makeKeyAndOrderFront:self];
		
		NSView *content = [fullscreenWindow contentView];
		[content retain];
		[fullscreenWindow setContentView:nil];
		[fWindow setContentView:content];
		[content release];
		
		[[NSNotificationCenter defaultCenter] postNotification:
		 [NSNotification notificationWithName:@"enableMenuBarNotification"
									   object:self]];
		
		[fullscreenWindow release];
		fullscreenWindow = nil;
		
		isFullscreen = NO;
	}
}

- (BOOL)readFromFileWrapper:(NSFileWrapper *)fileWrapper
					 ofType:(NSString *)typeName
					  error:(NSError **)outError
{
	return YES;
}

- (NSFileWrapper *)fileWrapperOfType:(NSString *)typeName error:(NSError **)outError {
	NSFileWrapper *fileWrapper = [[[NSFileWrapper alloc] initDirectoryWithFileWrappers:nil]
								  autorelease];
	return fileWrapper;
}

- (void)togglePower:(id)sender
{
	[self updateChangeCount:NSChangeDone];
}

- (void)resetEmulation:(id)sender
{
}

- (void)togglePause:(id)sender
{
}

- (void)toggleInspectorPanel:(id)sender
{
	[[NSNotificationCenter defaultCenter] postNotification:
	 [NSNotification notificationWithName:@"toggleInspectorPanelNotification"
								   object:self]];
}

- (NSString *)getDocumentText
{
	// To-Do: libemulation
	return @"This is a meticulously designed test of the speech synthesizing system.";  
}

- (void)copy:(id)sender
{
	if ([self validateCopy])
	{
		[pasteboard declareTypes:pasteboardTypes owner:self];
		[pasteboard setString:[self getDocumentText] forType:NSStringPboardType];
	}
}

- (void)paste:(id)sender
{
	if ([self validatePaste])
	{
		NSString *text = [pasteboard stringForType:NSStringPboardType];
		NSMutableString *mutableText = [NSMutableString 
										stringWithString:text];
		
		// Convert newlines
		[mutableText replaceOccurrencesOfString:@"\n"
									 withString:@"\r"
										options:NSLiteralSearch
										  range:NSMakeRange(0, [text length])];
		
		// To-do: Send to libemulation with [mutableText cStringUsingEncoding:NSASCIIStringEncoding]
	}
}

- (void)startSpeaking:(id)sender
{
	NSTextView *dummy = [[NSTextView alloc] init];
	[dummy insertText:[self getDocumentText]];
	[dummy startSpeaking:nil];
}

@end
