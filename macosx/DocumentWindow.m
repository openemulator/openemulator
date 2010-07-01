
/**
 * OpenEmulator
 * Mac OS X Document Window
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles document window messages.
 */

#import "DocumentWindow.h"
#import "DocumentWindowController.h"

#import "HostHID.h"

#define DEFAULT_FRAME_WIDTH		768
#define DEFAULT_FRAME_HEIGHT	576

#define COCOA_LCTRL		(1 << 0)
#define COCOA_LSHIFT	(1 << 1)
#define COCOA_LALT		(1 << 3)
#define COCOA_LGUI		(1 << 5)
#define COCOA_RCTRL		(1 << 13)
#define COCOA_RSHIFT	(1 << 2)
#define COCOA_RALT		(1 << 6)
#define COCOA_RGUI		(1 << 4)

typedef struct
{
	int keyCode;
	int usageId;
} CocoaKeyMapEntry;

// From:
// http://stuff.mit.edu/afs/sipb/project/darwin/src/
// modules/AppleADBKeyboard/AppleADBKeyboard.cpp
CocoaKeyMapEntry cocoaKeyMap[] = 
{
	{0x00, HOST_HID_K_A},
	{0x0b, HOST_HID_K_B},
	{0x08, HOST_HID_K_C},
	{0x02, HOST_HID_K_D},
	{0x0e, HOST_HID_K_E},
	{0x03, HOST_HID_K_F},
	{0x05, HOST_HID_K_G},
	{0x04, HOST_HID_K_H},
	{0x22, HOST_HID_K_I},
	{0x26, HOST_HID_K_J},
	{0x28, HOST_HID_K_K},
	{0x25, HOST_HID_K_L},
	{0x2e, HOST_HID_K_M},
	{0x2d, HOST_HID_K_N},
	{0x1f, HOST_HID_K_O},
	{0x23, HOST_HID_K_P},
	{0x0c, HOST_HID_K_Q},
	{0x0f, HOST_HID_K_R},
	{0x01, HOST_HID_K_S},
	{0x11, HOST_HID_K_T},
	{0x20, HOST_HID_K_U},
	{0x09, HOST_HID_K_V},
	{0x0d, HOST_HID_K_W},
	{0x07, HOST_HID_K_X},
	{0x10, HOST_HID_K_Y},
	{0x06, HOST_HID_K_Z},
	{0x12, HOST_HID_K_1},
	{0x13, HOST_HID_K_2},
	{0x14, HOST_HID_K_3},
	{0x15, HOST_HID_K_4},
	{0x17, HOST_HID_K_5},
	{0x16, HOST_HID_K_6},
	{0x1a, HOST_HID_K_7},
	{0x1c, HOST_HID_K_8},
	{0x19, HOST_HID_K_9},
	{0x1d, HOST_HID_K_0},
	{0x24, HOST_HID_K_ENTER},
	{0x35, HOST_HID_K_ESCAPE},
	{0x33, HOST_HID_K_BACKSPACE},
	{0x30, HOST_HID_K_TAB},
	{0x31, HOST_HID_K_SPACE},
	{0x1b, HOST_HID_K_MINUS},
	{0x18, HOST_HID_K_EQUAL},
	{0x21, HOST_HID_K_LEFTBRACKET},
	{0x1e, HOST_HID_K_RIGHTBRACKET},
	{0x2a, HOST_HID_K_BACKSLASH},
	{0x56, HOST_HID_K_NON_US1},
	{0x29, HOST_HID_K_SEMICOLON},
	{0x27, HOST_HID_K_QUOTE},
	{0x32, HOST_HID_K_GRAVEACCENT},
	{0x2b, HOST_HID_K_COMMA},
	{0x2f, HOST_HID_K_PERIOD},
	{0x2c, HOST_HID_K_SLASH},
	{0x39, HOST_HID_K_CAPSLOCK},
	{0x7a, HOST_HID_K_F1},
	{0x78, HOST_HID_K_F2},
	{0x63, HOST_HID_K_F3},
	{0x76, HOST_HID_K_F4},
	{0x60, HOST_HID_K_F5},
	{0x61, HOST_HID_K_F6},
	{0x62, HOST_HID_K_F7},
	{0x64, HOST_HID_K_F8},
	{0x65, HOST_HID_K_F9},
	{0x6d, HOST_HID_K_F10},
	{0x67, HOST_HID_K_F11},
	{0x6f, HOST_HID_K_F12},
	{0x69, HOST_HID_K_PRINTSCREEN},
	{0x6b, HOST_HID_K_SCROLLLOCK},
	{0x71, HOST_HID_K_PAUSE},
	{0x72, HOST_HID_K_INSERT},
	{0x73, HOST_HID_K_HOME},
	{0x74, HOST_HID_K_PAGEUP},
	{0x75, HOST_HID_K_DELETE},
	{0x77, HOST_HID_K_END},
	{0x79, HOST_HID_K_PAGEDOWN},
	{0x7c, HOST_HID_K_RIGHT},
	{0x7b, HOST_HID_K_LEFT},
	{0x7d, HOST_HID_K_DOWN},
	{0x7e, HOST_HID_K_UP},
	{0x47, HOST_HID_KP_NUMLOCK},
	{0x4b, HOST_HID_KP_SLASH},
	{0x43, HOST_HID_KP_STAR},
	{0x4e, HOST_HID_KP_MINUS},
	{0x45, HOST_HID_KP_PLUS},
	{0x4c, HOST_HID_KP_ENTER},
	{0x53, HOST_HID_KP_1},
	{0x54, HOST_HID_KP_2},
	{0x55, HOST_HID_KP_3},
	{0x56, HOST_HID_KP_4},
	{0x57, HOST_HID_KP_5},
	{0x58, HOST_HID_KP_6},
	{0x59, HOST_HID_KP_7},
	{0x5b, HOST_HID_KP_8},
	{0x5c, HOST_HID_KP_9},
	{0x52, HOST_HID_KP_0},
	{0x41, HOST_HID_KP_PERIOD},
	{0x7f, HOST_HID_K_POWER},

	{0x3b, HOST_HID_K_LEFTCONTROL},
	{0x38, HOST_HID_K_LEFTSHIFT},
	{0x3a, HOST_HID_K_LEFTALT},
	{0x37, HOST_HID_K_LEFTGUI},
	{0x3e, HOST_HID_K_RIGHTCONTROL},
	{0x3c, HOST_HID_K_RIGHTSHIFT},
	{0x3d, HOST_HID_K_RIGHTALT},
	{0x36, HOST_HID_K_RIGHTGUI},
};

int cocoaInverseKeyMap[512];

@implementation DocumentWindow

- (id) initWithContentRect:(NSRect) contentRect
				 styleMask:(NSUInteger) windowStyle
				   backing:(NSBackingStoreType) bufferingType 
					 defer:(BOOL) deferCreation
{
	self = [super initWithContentRect:contentRect
							styleMask:windowStyle
							  backing:bufferingType
								defer:deferCreation];
	if (self)
	{
		for (int i = 0; i < sizeof(cocoaKeyMap) / sizeof(CocoaKeyMapEntry); i++)
			cocoaInverseKeyMap[cocoaKeyMap[i].keyCode] = cocoaKeyMap[i].usageId;
	}
		
	return self;
}

- (void) dealloc
{
	if (fullscreen)
		[self toggleFullscreen:self];
	
	[super dealloc];
}

- (int) translateKeyCode:(int) keyCode
{
	if (keyCode < 256)
		return cocoaInverseKeyMap[keyCode];
	
	return 0;
}

- (void) keyDown:(NSEvent *) theEvent
{
	Document *document = [[self windowController] document];
	NSString *characters = [theEvent characters];
	
	if (![theEvent isARepeat])
		[document keyDown:[self translateKeyCode:[theEvent keyCode]]];
	
	for (int i = 0; i < [characters length]; i++)
	{
		int unicodeChar = [characters characterAtIndex:i];
		if ((unicodeChar < 0xf700) && (unicodeChar >= 0xf900))
		[document sendUnicodeChar:unicodeChar];
	}
}

- (void) keyUp:(NSEvent *) theEvent
{
	Document *document = [[self windowController] document];
	[document keyUp:[self translateKeyCode:[theEvent keyCode]]];
}

- (void) updateFlags:(int) flags
			 forMask:(int) mask
			 usageId:(int) usageId
{
	if ((flags & mask) == (modifierFlags & mask))
		return;
	
	Document *document = [[self windowController] document];
	if (flags & mask)
		[document keyDown:usageId];
	else
		[document keyUp:usageId];
}

- (void) updateCapsLock:(int) flags
{
	if ((flags & NSAlphaShiftKeyMask) ==
		(modifierFlags & NSAlphaShiftKeyMask))
		return;
	
	Document *document = [[self windowController] document];
	if (flags & NSAlphaShiftKeyMask)
		[document keyDown:HOST_HID_K_CAPSLOCK];
	else
		[document keyUp:HOST_HID_K_CAPSLOCK];
}

- (void) flagsChanged:(NSEvent *)theEvent
{
	int newModifierFlags = [theEvent modifierFlags];
	
	[self updateFlags:newModifierFlags 
			  forMask:COCOA_LCTRL usageId:HOST_HID_K_LEFTCONTROL];
	[self updateFlags:newModifierFlags 
			  forMask:COCOA_LSHIFT usageId:HOST_HID_K_LEFTSHIFT];
	[self updateFlags:newModifierFlags 
			  forMask:COCOA_LALT usageId:HOST_HID_K_LEFTALT];
	[self updateFlags:newModifierFlags 
			  forMask:COCOA_LGUI usageId:HOST_HID_K_LEFTGUI];
	[self updateFlags:newModifierFlags 
			  forMask:COCOA_RCTRL usageId:HOST_HID_K_RIGHTCONTROL];
	[self updateFlags:newModifierFlags 
			  forMask:COCOA_RSHIFT usageId:HOST_HID_K_RIGHTSHIFT];
	[self updateFlags:newModifierFlags 
			  forMask:COCOA_RALT usageId:HOST_HID_K_RIGHTALT];
	[self updateFlags:newModifierFlags 
			  forMask:COCOA_RGUI usageId:HOST_HID_K_RIGHTGUI];
	[self updateCapsLock:newModifierFlags];
	
	modifierFlags = newModifierFlags;
}

- (NSRect) constrainFrameRect:(NSRect) frameRect toScreen:(NSScreen *) screen
{
	return frameRect;
}

- (NSSize) windowWillResize:(NSWindow *) window toSize:(NSSize) proposedFrameSize
{
	if (!fullscreen)
		return proposedFrameSize;
	else
		return [window frame].size;
}

- (BOOL) windowShouldZoom:(NSWindow *) window toFrame:(NSRect) proposedFrame
{
	return !fullscreen;
}

- (void) setFrameOrigin:(NSPoint) point
{
	if (!fullscreen)
		[super setFrameOrigin:point];
}

- (void) setFrameTopLeftPoint:(NSPoint) point
{
	if (!fullscreen)
		[super setFrameTopLeftPoint:point];
}

- (BOOL) validateUserInterfaceItem:(id) item
{
	if ([item action] == @selector(toggleFullscreen:))
	{
		NSString *menuTitle;
		if (!fullscreen)
			menuTitle = NSLocalizedString(@"Enter Fullscreen",
										  @"Title for menu item to enter fullscreen "
										  "(should be the same as the initial menu "
										  "item in the nib).");
		else
			menuTitle = NSLocalizedString(@"Exit Fullscreen",
										  @"Title for menu item to exit fullscreen.");
		[item setTitleWithMnemonic:menuTitle];
		
		return YES;
	}
	else if ([item action] == @selector(setHalfSize:))
		return !fullscreen;
	else if ([item action] == @selector(setActualSize:))
		return !fullscreen;
	else if ([item action] == @selector(setDoubleSize:))
		return !fullscreen;
	else if ([item action] == @selector(fitToScreen:))
		return !fullscreen;
	else if ([item action] == @selector(toggleToolbarShown:))
		return !fullscreen;
	else if ([item action] == @selector(runToolbarCustomizationPalette:))
		return !fullscreen;
	else
		return YES;
}

- (void) setFrameSize:(double) proportion
{
	NSRect windowRect = [self frame];
	NSView *content = [self contentView];
	NSRect contentRect = [content frame];
	NSScreen *screen = [self screen];
	NSRect screenRect = [screen visibleFrame];
	float scale = [self userSpaceScaleFactor];
	
	contentRect.size.width *= scale;
	contentRect.size.height *= scale;
	
	double deltaWidth = NSWidth(windowRect) - NSWidth(contentRect);
	double deltaHeight = NSHeight(windowRect) - NSHeight(contentRect);
	
	windowRect.origin.x = NSMidX(windowRect);
	windowRect.origin.y = NSMaxY(windowRect);
	windowRect.size.width = scale * proportion * DEFAULT_FRAME_WIDTH + deltaWidth;
	windowRect.size.height = scale * proportion * DEFAULT_FRAME_HEIGHT + deltaHeight;
	windowRect.origin.x -= NSWidth(windowRect) / 2;
	windowRect.origin.y -= NSHeight(windowRect);
	
	if (NSMaxX(windowRect) > NSMaxX(screenRect))
		windowRect.origin.x = NSMaxX(screenRect) - NSWidth(windowRect);
	if (NSMaxY(windowRect) > NSMaxY(screenRect))
		windowRect.origin.y = NSMaxY(screenRect) - NSHeight(windowRect);
	if (NSMinX(windowRect) < NSMinX(screenRect))
		windowRect.origin.x = NSMinX(screenRect);
	if (NSMinY(windowRect) < NSMinY(screenRect))
		windowRect.origin.y = NSMinY(screenRect);
	
	if (NSWidth(windowRect) > NSWidth(screenRect))
		windowRect.size.width = NSWidth(screenRect);
	if (NSHeight(windowRect) > NSHeight(screenRect))
		windowRect.size.height = NSHeight(screenRect);
	
	[self setFrame:windowRect display:YES animate:YES];
}

- (void) toggleFullscreen:(id) sender
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	if (!fullscreen)
	{
		[documentController disableMenuBar];
		
		fullscreenExitRect = [self frame];
		NSRect contentRect = [[self contentView] frame];
		float titlebarHeight = (NSHeight(fullscreenExitRect) -
								([self userSpaceScaleFactor] * 
								 NSHeight(contentRect)));
		
		NSRect windowRect = [[self screen] frame];
		windowRect.size.height += titlebarHeight;
		
		[self setFrame:windowRect
			   display:YES
			   animate:YES];
		
		fullscreen = YES;
	}
	else
	{
		[self setFrame:fullscreenExitRect
			   display:YES
			   animate:YES];
		
		[documentController enableMenuBar];
		
		fullscreen = NO;
	}
}

- (void) setHalfSize:(id) sender
{
	if (!fullscreen)
		[self setFrameSize:0.5];
}

- (void) setActualSize:(id) sender
{
	if (!fullscreen)
		[self setFrameSize:1.0];
}

- (void) setDoubleSize:(id) sender
{
	if (!fullscreen)
		[self setFrameSize:2.0];
}

- (void) fitToScreen:(id) sender
{
	if (!fullscreen)
		[self setFrameSize:256.0];
}

@end
