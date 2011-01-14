
/**
 * OpenEmulator
 * Mac OS X Canvas View
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas view.
 */

#import "CanvasView.h"
#import "CanvasWindowController.h"
#import "DocumentController.h"
#import "Document.h"
#import "StringConversion.h"

#import "OpenGLHAL.h"

#define NSLeftControlKeyMask	(1 << 0)
#define NSLeftShiftKeyMask		(1 << 1)
#define NSRightShiftKeyMask		(1 << 2)
#define NSLeftAlternateKeyMask	(1 << 3)
#define NSRightCommandKeyMask	(1 << 4)
#define NSLeftCommandKeyMask	(1 << 5)
#define NSRightAlternateKeyMask	(1 << 6)
#define NSRightControlKeyMask	(1 << 13)

typedef struct
{
	int keyCode;
	int usageId;
} CanvasKeyMapEntry;

// From:
// http://stuff.mit.edu/afs/sipb/project/darwin/src/
// modules/AppleADBKeyboard/AppleADBKeyboard.cpp
CanvasKeyMapEntry canvasKeyMap[] = 
{
	{0x00, CANVAS_K_A},
	{0x0b, CANVAS_K_B},
	{0x08, CANVAS_K_C},
	{0x02, CANVAS_K_D},
	{0x0e, CANVAS_K_E},
	{0x03, CANVAS_K_F},
	{0x05, CANVAS_K_G},
	{0x04, CANVAS_K_H},
	{0x22, CANVAS_K_I},
	{0x26, CANVAS_K_J},
	{0x28, CANVAS_K_K},
	{0x25, CANVAS_K_L},
	{0x2e, CANVAS_K_M},
	{0x2d, CANVAS_K_N},
	{0x1f, CANVAS_K_O},
	{0x23, CANVAS_K_P},
	{0x0c, CANVAS_K_Q},
	{0x0f, CANVAS_K_R},
	{0x01, CANVAS_K_S},
	{0x11, CANVAS_K_T},
	{0x20, CANVAS_K_U},
	{0x09, CANVAS_K_V},
	{0x0d, CANVAS_K_W},
	{0x07, CANVAS_K_X},
	{0x10, CANVAS_K_Y},
	{0x06, CANVAS_K_Z},
	{0x12, CANVAS_K_1},
	{0x13, CANVAS_K_2},
	{0x14, CANVAS_K_3},
	{0x15, CANVAS_K_4},
	{0x17, CANVAS_K_5},
	{0x16, CANVAS_K_6},
	{0x1a, CANVAS_K_7},
	{0x1c, CANVAS_K_8},
	{0x19, CANVAS_K_9},
	{0x1d, CANVAS_K_0},
	{0x24, CANVAS_K_ENTER},
	{0x35, CANVAS_K_ESCAPE},
	{0x33, CANVAS_K_BACKSPACE},
	{0x30, CANVAS_K_TAB},
	{0x31, CANVAS_K_SPACE},
	{0x1b, CANVAS_K_MINUS},
	{0x18, CANVAS_K_EQUAL},
	{0x21, CANVAS_K_LEFTBRACKET},
	{0x1e, CANVAS_K_RIGHTBRACKET},
	{0x2a, CANVAS_K_BACKSLASH},
	{0x0a, CANVAS_K_NON_US1},
	{0x29, CANVAS_K_SEMICOLON},
	{0x27, CANVAS_K_QUOTE},
	{0x32, CANVAS_K_GRAVEACCENT},
	{0x2b, CANVAS_K_COMMA},
	{0x2f, CANVAS_K_PERIOD},
	{0x2c, CANVAS_K_SLASH},
	{0x39, CANVAS_K_CAPSLOCK},
	{0x7a, CANVAS_K_F1},
	{0x78, CANVAS_K_F2},
	{0x63, CANVAS_K_F3},
	{0x76, CANVAS_K_F4},
	{0x60, CANVAS_K_F5},
	{0x61, CANVAS_K_F6},
	{0x62, CANVAS_K_F7},
	{0x64, CANVAS_K_F8},
	{0x65, CANVAS_K_F9},
	{0x6d, CANVAS_K_F10},
	{0x67, CANVAS_K_F11},
	{0x6f, CANVAS_K_F12},
	{0x69, CANVAS_K_PRINTSCREEN},
	{0x6b, CANVAS_K_SCROLLLOCK},
	{0x71, CANVAS_K_PAUSE},
	{0x72, CANVAS_K_INSERT},
	{0x73, CANVAS_K_HOME},
	{0x74, CANVAS_K_PAGEUP},
	{0x75, CANVAS_K_DELETE},
	{0x77, CANVAS_K_END},
	{0x79, CANVAS_K_PAGEDOWN},
	{0x7c, CANVAS_K_RIGHT},
	{0x7b, CANVAS_K_LEFT},
	{0x7d, CANVAS_K_DOWN},
	{0x7e, CANVAS_K_UP},
	{0x47, CANVAS_KP_NUMLOCK},
	{0x4b, CANVAS_KP_SLASH},
	{0x43, CANVAS_KP_STAR},
	{0x4e, CANVAS_KP_MINUS},
	{0x45, CANVAS_KP_PLUS},
	{0x4c, CANVAS_KP_ENTER},
	{0x53, CANVAS_KP_1},
	{0x54, CANVAS_KP_2},
	{0x55, CANVAS_KP_3},
	{0x56, CANVAS_KP_4},
	{0x57, CANVAS_KP_5},
	{0x58, CANVAS_KP_6},
	{0x59, CANVAS_KP_7},
	{0x5b, CANVAS_KP_8},
	{0x5c, CANVAS_KP_9},
	{0x52, CANVAS_KP_0},
	{0x41, CANVAS_KP_PERIOD},
	{0x51, CANVAS_KP_EQUAL},
	{0x6a, CANVAS_K_F16},
	{0x40, CANVAS_K_F17},
	{0x4f, CANVAS_K_F18},
	{0x50, CANVAS_K_F19},
	{0x7f, CANVAS_K_POWER},
	
	{0x3b, CANVAS_K_LEFTCONTROL},
	{0x38, CANVAS_K_LEFTSHIFT},
	{0x3a, CANVAS_K_LEFTALT},
	{0x37, CANVAS_K_LEFTGUI},
	{0x3e, CANVAS_K_RIGHTCONTROL},
	{0x3c, CANVAS_K_RIGHTSHIFT},
	{0x3d, CANVAS_K_RIGHTALT},
	{0x36, CANVAS_K_RIGHTGUI},
};

@implementation CanvasView

static void setCapture(void *userData, CanvasCapture capture)
{
	switch (capture)
	{
		case CANVAS_CAPTURE_NONE:
			CGDisplayShowCursor(kCGDirectMainDisplay);
			CGAssociateMouseAndMouseCursorPosition(YES);
			break;
			
		case CANVAS_CAPTURE_KEYBOARD_AND_MOUSE:
			CGDisplayHideCursor(kCGDirectMainDisplay);
			CGAssociateMouseAndMouseCursorPosition(NO);
			break;
			
		case CANVAS_CAPTURE_MOUSE:
			CGDisplayHideCursor(kCGDirectMainDisplay);
			CGAssociateMouseAndMouseCursorPosition(YES);
			break;
	}
}

static void setKeyboardFlags(void *userData, int flags)
{
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
									const CVTimeStamp *now,
									const CVTimeStamp *outputTime,
									CVOptionFlags flagsIn,
									CVOptionFlags *flagsOut,
									void *displayLinkContext)
{
	// There is no autorelease pool when this method is called
	// (it is called from a background thread)
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	[(CanvasView *)displayLinkContext drawView];
	
	[pool release];
	
	return kCVReturnSuccess;
}

- (id)initWithFrame:(NSRect)rect
{
	NSOpenGLPixelFormatAttribute pixelFormatAtrributes[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 0,
		NSOpenGLPFAStencilSize, 0,
		NSOpenGLPFAAccumSize, 0,
		0
	};
	
	NSOpenGLPixelFormat *pixelFormat;
	pixelFormat = [[NSOpenGLPixelFormat alloc]
				   initWithAttributes:pixelFormatAtrributes];
	if (!pixelFormat)
	{
		NSLog(@"Cannot create NSOpenGLPixelFormat");
		return nil;
	}
	[pixelFormat autorelease];
	
	if (self = [super initWithFrame:rect pixelFormat:pixelFormat])
	{
		memset(keyMap, sizeof(keyMap), 0);
		for (int i = 0;
			 i < sizeof(canvasKeyMap) / sizeof(CanvasKeyMapEntry);
			 i++)
			keyMap[canvasKeyMap[i].keyCode] = canvasKeyMap[i].usageId;
		
		keyModifierFlags = 0;

		[self registerForDraggedTypes:[NSArray arrayWithObjects:
									   NSStringPboardType,
									   NSFilenamesPboardType, 
									   nil]];
	}
	
	return self;
}

- (void)dealloc
{
	CVDisplayLinkRelease(displayLink);
	
	if (canvas)
		((OpenGLHAL *)canvas)->close();
	
	[super dealloc];
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
	NSLog(@"windowDidBecomeKey");
	
	NSPoint mouseLocation = [self convertPoint:[[self window] convertScreenToBase:
												[NSEvent mouseLocation]]
									  fromView:nil];
	if (NSMouseInRect(mouseLocation, [self bounds], [self isFlipped]))
		[self mouseEntered:nil];
	
	NSTrackingAreaOptions options = (NSTrackingMouseEnteredAndExited |
									 NSTrackingMouseMoved |
									 NSTrackingActiveInKeyWindow);
	
	NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:[self bounds]
														options:options
														  owner:self
													   userInfo:nil];
	[self addTrackingArea:area];
	[area release];
}

- (void)windowDidResignKey:(NSNotification *)notification
{
	NSLog(@"windowDidResignKey");
	
	NSPoint mouseLocation = [self convertPoint:[[self window] convertScreenToBase:
												[NSEvent mouseLocation]]
									  fromView:nil];
	if (NSMouseInRect(mouseLocation, [self bounds], [self isFlipped]))
		[self mouseExited:nil];
	
	for (NSTrackingArea *area in [self trackingAreas])
		if ([area owner] == self)
			[self removeTrackingArea:area];
}

- (void)awakeFromNib
{
	CanvasWindowController *canvasWindowController = [[self window] windowController];
	canvas = [canvasWindowController canvas];
}

- (NSSize)canvasSize
{
	// This should be locked
	OESize canvasSize = ((OpenGLHAL *)canvas)->getSize();
	
	return NSMakeSize(canvasSize.width, canvasSize.height);
}



- (void)prepareOpenGL
{
	GLint value = 1;
	[[self openGLContext] setValues:&value
					   forParameter:NSOpenGLCPSwapInterval]; 
	
	CanvasWindowController *canvasWindowController;
	canvasWindowController = [[self window] windowController];
	
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	((OpenGLHAL *)canvas)->open(setCapture,
								setKeyboardFlags,
								NULL);
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, self);
	CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] 
														   CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,
													  cglContext,
													  cglPixelFormat);
	
	CVDisplayLinkStart(displayLink);
}



- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
	NSPasteboard *pasteboard = [sender draggingPasteboard];
	if ([[pasteboard types] containsObject:NSFilenamesPboardType])
	{
		DocumentController *documentController;
		documentController = [NSDocumentController sharedDocumentController];
		
		NSString *path = [[pasteboard propertyListForType:NSFilenamesPboardType]
						  objectAtIndex:0];
		NSString *pathExtension = [[path pathExtension] lowercaseString];
		
		if ([[documentController diskImagePathExtensions] containsObject:pathExtension])
		{
			Document *document = [[[self window] windowController] document];
			if ([document canMount:path])
				return NSDragOperationCopy;
		}
		else if ([[documentController audioPathExtensions] containsObject:pathExtension] ||
				 [[documentController textPathExtensions] containsObject:pathExtension])
			return NSDragOperationCopy;
	}
	else if ([[pasteboard types] containsObject:NSStringPboardType])
		return NSDragOperationCopy;
	
	return NSDragOperationNone;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
	NSPasteboard *pasteboard = [sender draggingPasteboard];
	
	if ([[pasteboard types] containsObject:NSFilenamesPboardType])
	{
		NSPasteboard *pasteboard = [sender draggingPasteboard];
		NSString *path = [[pasteboard propertyListForType:NSFilenamesPboardType]
						  objectAtIndex:0];
		
		DocumentController *documentController;
		documentController = [NSDocumentController sharedDocumentController];
		[documentController application:NSApp openFile:path];
		
		return YES;
	}
	else if ([[pasteboard types] containsObject:NSStringPboardType])
	{
		string clipboard = getCPPString([pasteboard stringForType:NSStringPboardType]);
		
		((OpenGLHAL *)canvas)->paste(clipboard);
		
		return YES;
	}
	
	return NO;
}

- (void)reshape
{
	[[self openGLContext] makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	[[self openGLContext] update];
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)theRect
{
	if (!CVDisplayLinkIsRunning(displayLink))
		[self drawView];
}

- (void)drawView
{
	[[self openGLContext] makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	NSRect frame = [self bounds];
	((OpenGLHAL *)canvas)->draw(NSWidth(frame), NSHeight(frame));
	[[self openGLContext] flushBuffer];
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (int)getUsageId:(int)keyCode
{
	int usageId = (keyCode < DEVICE_KEYMAP_SIZE) ? keyMap[keyCode] : 0;
	
	if (!usageId)
		NSLog(@"Unknown key code %d", keyCode);
	
	return usageId;
}

- (void)keyDown:(NSEvent *)theEvent
{
	NSString *characters = [theEvent characters];
	for (int i = 0; i < [characters length]; i++)
	{
		int unicode = [characters characterAtIndex:i];
		((OpenGLHAL *)canvas)->sendUnicodeKeyEvent(unicode);
	}
	
	if (![theEvent isARepeat])
	{
		int usageId = [self getUsageId:[theEvent keyCode]];
		((OpenGLHAL *)canvas)->setKey(usageId, true);
	}
}

- (void)keyUp:(NSEvent *)theEvent
{
	int usageId = [self getUsageId:[theEvent keyCode]];
	((OpenGLHAL *)canvas)->setKey(usageId, false);
}

- (void)updateFlags:(int)flags
			forMask:(int)mask
			usageId:(int)usageId
{
	if ((flags & mask) == (keyModifierFlags & mask))
		return;
	
	BOOL value = ((flags & mask) != 0);
	
	((OpenGLHAL *)canvas)->setKey(usageId, value);
}

- (void)flagsChanged:(NSEvent *)theEvent
{
	int flags = [theEvent modifierFlags];
	
	[self updateFlags:flags forMask:NSLeftControlKeyMask
			  usageId:CANVAS_K_LEFTCONTROL];
	[self updateFlags:flags forMask:NSLeftShiftKeyMask
			  usageId:CANVAS_K_LEFTSHIFT];
	[self updateFlags:flags forMask:NSLeftAlternateKeyMask
			  usageId:CANVAS_K_LEFTALT];
	[self updateFlags:flags forMask:NSLeftCommandKeyMask
			  usageId:CANVAS_K_LEFTGUI];
	[self updateFlags:flags forMask:NSRightControlKeyMask
			  usageId:CANVAS_K_RIGHTCONTROL];
	[self updateFlags:flags forMask:NSRightShiftKeyMask
			  usageId:CANVAS_K_RIGHTSHIFT];
	[self updateFlags:flags forMask:NSRightAlternateKeyMask
			  usageId:CANVAS_K_RIGHTALT];
	[self updateFlags:flags forMask:NSRightCommandKeyMask
			  usageId:CANVAS_K_RIGHTGUI];
	keyModifierFlags = flags;	
	
	// To-Do: NSAlphaShiftKeyMask
	// To-Do: NSNumericPadKeyMask
	
	// To-Do: See if NSShiftKeyMask, NSControlKeyMask,
	// NSAlternateKeyMask, NSCommandKeyMask is set but not the others
	// Act accordingly with a left key message
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	NSLog(@"mouseEntered");
	// This should be locked
	((OpenGLHAL *)canvas)->enterMouse();
}

- (void)mouseExited:(NSEvent *)theEvent
{
	NSLog(@"mouseExited");
	// This should be locked
	((OpenGLHAL *)canvas)->enterMouse();
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSLog(@"mouseMoved");
	NSPoint position = [NSEvent mouseLocation];
	
	((OpenGLHAL *)canvas)->setMousePosition(position.x, position.y);
	((OpenGLHAL *)canvas)->moveMouse([theEvent deltaX], [theEvent deltaY]);
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	[self mouseMoved:theEvent];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
	[self mouseMoved:theEvent];
}

- (void)otherMouseDragged:(NSEvent *)theEvent
{
	[self mouseMoved:theEvent];
}

- (void)mouseDown:(NSEvent *)theEvent
{
	((OpenGLHAL *)canvas)->setMouseButton(0, true);
}

- (void)mouseUp:(NSEvent *)theEvent
{
	((OpenGLHAL *)canvas)->setMouseButton(0, false);
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	((OpenGLHAL *)canvas)->setMouseButton(1, true);
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
	((OpenGLHAL *)canvas)->setMouseButton(1, false);
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
	((OpenGLHAL *)canvas)->setMouseButton([theEvent buttonNumber], true);
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
	((OpenGLHAL *)canvas)->setMouseButton([theEvent buttonNumber], false);
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	((OpenGLHAL *)canvas)->sendMouseWheelEvent(0, [theEvent deltaX]);
	((OpenGLHAL *)canvas)->sendMouseWheelEvent(1, [theEvent deltaY]);
}

- (void)systemPowerDown:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_POWERDOWN);
}

- (void)systemSleep:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_SLEEP);
}

- (void)systemWakeUp:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_WAKEUP);
}

- (void)systemColdRestart:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_COLDRESTART);
}

- (void)systemWarmRestart:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_WARMRESTART);
}

- (void)systemBreak:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_BREAK);
}

- (void)systemDebuggerBreak:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_DEBUGGERBREAK);
}

- (void)applicationBreak:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_APPLICATIONBREAK);
}

- (void)applicationDebuggerBreak:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_APPLICATIONDEBUGGERBREAK);
}

- (void)systemHibernate:(id)sender
{
	((OpenGLHAL *)canvas)->sendSystemEvent(CANVAS_S_HIBERNATE);
}

- (void)copy:(id)sender
{
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	string clipboard;
	
	if (((OpenGLHAL *)canvas)->copy(clipboard))
	{
		NSArray *pasteboardTypes = [NSArray arrayWithObjects:NSStringPboardType, nil];
		[pasteboard declareTypes:pasteboardTypes owner:self];
		
		[pasteboard setString:getNSString(clipboard)
					  forType:NSStringPboardType];
	}
	else
	{
		NSArray *pasteboardTypes = [NSArray arrayWithObjects:NSTIFFPboardType, nil];
		[pasteboard declareTypes:pasteboardTypes owner:self];
		
		[self lockFocus];
		NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
								 initWithFocusedViewRect:[self bounds]];
		[self unlockFocus];
		
		[pasteboard setData:[rep TIFFRepresentation]
					forType:NSTIFFPboardType];
	}
}

- (void)pasteText:(NSString *)text
{
	string clipboard = getCPPString(text);
	
	// This should be locked
	((OpenGLHAL *)canvas)->paste(clipboard);
}

- (void)paste:(id)sender
{
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	[self pasteText:[pasteboard stringForType:NSStringPboardType]];
}

- (void)startSpeaking:(id)sender
{
	string clipboard;
	
	// This should be locked
	if (((OpenGLHAL *)canvas)->copy(clipboard))
	{
		NSTextView *dummy = [[[NSTextView alloc] init] autorelease];
		[dummy insertText:getNSString(clipboard)];
		[dummy startSpeaking:self];
	}
}

@end
