
/**
 * OpenEmulator
 * Mac OS X Canvas View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas view.
 */

#import "CanvasViewController.h"
#import "DocumentController.h"
#import "StringConversion.h"

#import "OEOpenGLCanvas.h"

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
	{0x00, HOST_CANVAS_K_A},
	{0x0b, HOST_CANVAS_K_B},
	{0x08, HOST_CANVAS_K_C},
	{0x02, HOST_CANVAS_K_D},
	{0x0e, HOST_CANVAS_K_E},
	{0x03, HOST_CANVAS_K_F},
	{0x05, HOST_CANVAS_K_G},
	{0x04, HOST_CANVAS_K_H},
	{0x22, HOST_CANVAS_K_I},
	{0x26, HOST_CANVAS_K_J},
	{0x28, HOST_CANVAS_K_K},
	{0x25, HOST_CANVAS_K_L},
	{0x2e, HOST_CANVAS_K_M},
	{0x2d, HOST_CANVAS_K_N},
	{0x1f, HOST_CANVAS_K_O},
	{0x23, HOST_CANVAS_K_P},
	{0x0c, HOST_CANVAS_K_Q},
	{0x0f, HOST_CANVAS_K_R},
	{0x01, HOST_CANVAS_K_S},
	{0x11, HOST_CANVAS_K_T},
	{0x20, HOST_CANVAS_K_U},
	{0x09, HOST_CANVAS_K_V},
	{0x0d, HOST_CANVAS_K_W},
	{0x07, HOST_CANVAS_K_X},
	{0x10, HOST_CANVAS_K_Y},
	{0x06, HOST_CANVAS_K_Z},
	{0x12, HOST_CANVAS_K_1},
	{0x13, HOST_CANVAS_K_2},
	{0x14, HOST_CANVAS_K_3},
	{0x15, HOST_CANVAS_K_4},
	{0x17, HOST_CANVAS_K_5},
	{0x16, HOST_CANVAS_K_6},
	{0x1a, HOST_CANVAS_K_7},
	{0x1c, HOST_CANVAS_K_8},
	{0x19, HOST_CANVAS_K_9},
	{0x1d, HOST_CANVAS_K_0},
	{0x24, HOST_CANVAS_K_ENTER},
	{0x35, HOST_CANVAS_K_ESCAPE},
	{0x33, HOST_CANVAS_K_BACKSPACE},
	{0x30, HOST_CANVAS_K_TAB},
	{0x31, HOST_CANVAS_K_SPACE},
	{0x1b, HOST_CANVAS_K_MINUS},
	{0x18, HOST_CANVAS_K_EQUAL},
	{0x21, HOST_CANVAS_K_LEFTBRACKET},
	{0x1e, HOST_CANVAS_K_RIGHTBRACKET},
	{0x2a, HOST_CANVAS_K_BACKSLASH},
	{0x0a, HOST_CANVAS_K_NON_US1},
	{0x29, HOST_CANVAS_K_SEMICOLON},
	{0x27, HOST_CANVAS_K_QUOTE},
	{0x32, HOST_CANVAS_K_GRAVEACCENT},
	{0x2b, HOST_CANVAS_K_COMMA},
	{0x2f, HOST_CANVAS_K_PERIOD},
	{0x2c, HOST_CANVAS_K_SLASH},
	{0x39, HOST_CANVAS_K_CAPSLOCK},
	{0x7a, HOST_CANVAS_K_F1},
	{0x78, HOST_CANVAS_K_F2},
	{0x63, HOST_CANVAS_K_F3},
	{0x76, HOST_CANVAS_K_F4},
	{0x60, HOST_CANVAS_K_F5},
	{0x61, HOST_CANVAS_K_F6},
	{0x62, HOST_CANVAS_K_F7},
	{0x64, HOST_CANVAS_K_F8},
	{0x65, HOST_CANVAS_K_F9},
	{0x6d, HOST_CANVAS_K_F10},
	{0x67, HOST_CANVAS_K_F11},
	{0x6f, HOST_CANVAS_K_F12},
	{0x69, HOST_CANVAS_K_PRINTSCREEN},
	{0x6b, HOST_CANVAS_K_SCROLLLOCK},
	{0x71, HOST_CANVAS_K_PAUSE},
	{0x72, HOST_CANVAS_K_INSERT},
	{0x73, HOST_CANVAS_K_HOME},
	{0x74, HOST_CANVAS_K_PAGEUP},
	{0x75, HOST_CANVAS_K_DELETE},
	{0x77, HOST_CANVAS_K_END},
	{0x79, HOST_CANVAS_K_PAGEDOWN},
	{0x7c, HOST_CANVAS_K_RIGHT},
	{0x7b, HOST_CANVAS_K_LEFT},
	{0x7d, HOST_CANVAS_K_DOWN},
	{0x7e, HOST_CANVAS_K_UP},
	{0x47, HOST_CANVAS_KP_NUMLOCK},
	{0x4b, HOST_CANVAS_KP_SLASH},
	{0x43, HOST_CANVAS_KP_STAR},
	{0x4e, HOST_CANVAS_KP_MINUS},
	{0x45, HOST_CANVAS_KP_PLUS},
	{0x4c, HOST_CANVAS_KP_ENTER},
	{0x53, HOST_CANVAS_KP_1},
	{0x54, HOST_CANVAS_KP_2},
	{0x55, HOST_CANVAS_KP_3},
	{0x56, HOST_CANVAS_KP_4},
	{0x57, HOST_CANVAS_KP_5},
	{0x58, HOST_CANVAS_KP_6},
	{0x59, HOST_CANVAS_KP_7},
	{0x5b, HOST_CANVAS_KP_8},
	{0x5c, HOST_CANVAS_KP_9},
	{0x52, HOST_CANVAS_KP_0},
	{0x41, HOST_CANVAS_KP_PERIOD},
	{0x51, HOST_CANVAS_KP_EQUAL},
	{0x6a, HOST_CANVAS_K_F16},
	{0x40, HOST_CANVAS_K_F17},
	{0x4f, HOST_CANVAS_K_F18},
	{0x50, HOST_CANVAS_K_F19},
	{0x7f, HOST_CANVAS_K_POWER},
	
	{0x3b, HOST_CANVAS_K_LEFTCONTROL},
	{0x38, HOST_CANVAS_K_LEFTSHIFT},
	{0x3a, HOST_CANVAS_K_LEFTALT},
	{0x37, HOST_CANVAS_K_LEFTGUI},
	{0x3e, HOST_CANVAS_K_RIGHTCONTROL},
	{0x3c, HOST_CANVAS_K_RIGHTSHIFT},
	{0x3d, HOST_CANVAS_K_RIGHTALT},
	{0x36, HOST_CANVAS_K_RIGHTGUI},
};

@implementation CanvasViewController

static void setCaptureMode(int captureMode)
{
	switch (captureMode)
	{
		case 0:
			CGDisplayShowCursor(kCGDirectMainDisplay);
			break;
			
		case 1:
			CGDisplayHideCursor(kCGDirectMainDisplay);
			break;
			
		case 2:
			CGDisplayHideCursor(kCGDirectMainDisplay);
			break;
	}
}

static void setKeyboardFlags(void *userData, int ledFlags)
{
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
									const CVTimeStamp *now,
									const CVTimeStamp *outputTime,
									CVOptionFlags flagsIn,
									CVOptionFlags *flagsOut,
									void *displayLinkContext)
{
	// There is no autorelease pool when this method is called because
	// it is called from a background thread
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
    [(CanvasViewController *)displayLinkContext drawFrame];
	
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
		[self registerForDraggedTypes:[NSArray arrayWithObjects:
									   NSStringPboardType,
									   NSFilenamesPboardType, 
									   nil]];
		
		/*		Document *document = [fDocumentWindowController document];
		OEPortAudioEmulation *emulation = (OEPortAudioEmulation *)[document emulation];
*/		
		oeOpenGLCanvas = new OEOpenGLCanvas();
		
		memset(keyMap, sizeof(keyMap), 0);
		for (int i = 0;
			 i < sizeof(canvasKeyMap) / sizeof(CanvasKeyMapEntry);
			 i++)
			keyMap[canvasKeyMap[i].keyCode] = canvasKeyMap[i].usageId;
		
		keyModifierFlags = 0;
	}
	
	return self;
}

- (void)dealloc
{
    CVDisplayLinkRelease(displayLink);
	
	delete (OEOpenGLCanvas *)oeOpenGLCanvas;
	
	[self unregisterDraggedTypes];
	
	[super dealloc];
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
		
		if (([[documentController diskImagePathExtensions] containsObject:pathExtension])
			|| ([[documentController audioPathExtensions] containsObject:pathExtension])
			|| ([pathExtension compare:@"txt"] == NSOrderedSame))
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
		DocumentController *documentController;
		documentController = [NSDocumentController sharedDocumentController];
		
		NSPasteboard *pasteboard = [sender draggingPasteboard];
		NSString *path = [[pasteboard propertyListForType:NSFilenamesPboardType]
						  objectAtIndex:0];
		
		[documentController application:NSApp openFile:path];
		
		return YES;
	}
	else if ([[pasteboard types] containsObject:NSStringPboardType])
	{
		// Paste here
		return YES;
	}
	
    return NO;
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (BOOL)resignFirstResponder
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->resetKeysAndButtons();
	
	NSLog(@"Lost focus");
	
	return YES;
}

- (void)prepareOpenGL
{
	GLint value = 1;
	[[self openGLContext] setValues:&value forParameter:NSOpenGLCPSwapInterval]; 
	
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	((OEOpenGLCanvas *)oeOpenGLCanvas)->initOpenGL();
	
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

- (void)reshape
{
	[[self openGLContext] makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	[[self openGLContext] update];
	
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)theRect
{
	[self drawFrame];
}

- (void)drawFrame
{
	[[self openGLContext] makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	NSRect frame = [self bounds];
	((OEOpenGLCanvas *)oeOpenGLCanvas)->draw(NSWidth(frame),
											 NSHeight(frame));
	
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
		((OEOpenGLCanvas *)oeOpenGLCanvas)->setUnicodeKey(unicode);
	}
	
	if (![theEvent isARepeat])
	{
		int usageId = [self getUsageId:[theEvent keyCode]];
		((OEOpenGLCanvas *)oeOpenGLCanvas)->setKey(usageId, true);
	}
}

- (void)keyUp:(NSEvent *)theEvent
{
	int usageId = [self getUsageId:[theEvent keyCode]];
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setKey(usageId, false);
}

- (void)updateFlags:(int)flags
			forMask:(int)mask
			usageId:(int)usageId
{
	if ((flags & mask) == (keyModifierFlags & mask))
		return;
	
	BOOL value = ((flags & mask) != 0);
	
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setKey(usageId, value);
}

- (void)flagsChanged:(NSEvent *)theEvent
{
	int flags = [theEvent modifierFlags];
	
	[self updateFlags:flags forMask:NSLeftControlKeyMask
			  usageId:HOST_CANVAS_K_LEFTCONTROL];
	[self updateFlags:flags forMask:NSLeftShiftKeyMask
			  usageId:HOST_CANVAS_K_LEFTSHIFT];
	[self updateFlags:flags forMask:NSLeftAlternateKeyMask
			  usageId:HOST_CANVAS_K_LEFTALT];
	[self updateFlags:flags forMask:NSLeftCommandKeyMask
			  usageId:HOST_CANVAS_K_LEFTGUI];
	[self updateFlags:flags forMask:NSRightControlKeyMask
			  usageId:HOST_CANVAS_K_RIGHTCONTROL];
	[self updateFlags:flags forMask:NSRightShiftKeyMask
			  usageId:HOST_CANVAS_K_RIGHTSHIFT];
	[self updateFlags:flags forMask:NSRightAlternateKeyMask
			  usageId:HOST_CANVAS_K_RIGHTALT];
	[self updateFlags:flags forMask:NSRightCommandKeyMask
			  usageId:HOST_CANVAS_K_RIGHTGUI];
	keyModifierFlags = flags;	
	
	// To-Do: NSAlphaShiftKeyMask
	// To-Do: NSNumericPadKeyMask
	
	// To-Do: See if NSShiftKeyMask, NSControlKeyMask,
	// NSAlternateKeyMask, NSCommandKeyMask is set but not the others
	// Act accordingly with a left key message
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSPoint position = [NSEvent mouseLocation];
	
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setMousePosition(position.x,
														 position.y);
	((OEOpenGLCanvas *)oeOpenGLCanvas)->moveMouse([theEvent deltaX],
												  [theEvent deltaY]);
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
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setMouseButton(0, true);
}

- (void)mouseUp:(NSEvent *)theEvent
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setMouseButton(0, false);
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setMouseButton(1, true);
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setMouseButton(1, false);
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setMouseButton([theEvent buttonNumber], true);
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setMouseButton([theEvent buttonNumber], false);
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->sendMouseWheelEvent(0, [theEvent deltaX]);
	((OEOpenGLCanvas *)oeOpenGLCanvas)->sendMouseWheelEvent(1, [theEvent deltaY]);
}

- (void)systemPowerDown:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_POWERDOWN);
}

- (void)systemSleep:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_SLEEP);
}

- (void)systemWakeUp:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_WAKEUP);
}

- (void)systemColdRestart:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_COLDRESTART);
}

- (void)systemWarmRestart:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_WARMRESTART);
}

- (void)systemBreak:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_BREAK);
}

- (void)systemDebuggerBreak:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_DEBUGGERBREAK);
}

- (void)applicationBreak:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_APPLICATIONBREAK);
}

- (void)applicationDebuggerBreak:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_APPLICATIONDEBUGGERBREAK);
}

- (void)systemHibernate:(id)sender
{
	((OEOpenGLCanvas *)oeOpenGLCanvas)->setSystemKey(HOST_CANVAS_S_HIBERNATE);
}

- (NSString *)documentText
{
	string characterString;
//	((OEOpenGLCanvas *)oeOpenGLCanvas)->notify(HOST_CLIPBOARD_WILL_COPY,
//											   &characterString];
	
	return getNSString(characterString);
}

- (void)copy:(id)sender
{
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	NSArray *pasteboardTypes = [NSArray arrayWithObjects:NSStringPboardType, nil];
	
	[pasteboard declareTypes:pasteboardTypes owner:self];
	[pasteboard setString:[self documentText] forType:NSStringPboardType];
	
	// To-Do: If copy fails, copy the canvas
}

- (void)paste:(id)sender
{
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	
	//	string characterString = getString([pasteboard stringForType:NSStringPboardType]);
	
	//	[self notifyHost:HOST_CLIPBOARD_WILL_PASTE data:&characterString];
}

- (void)startSpeaking:(id)sender
{
	NSTextView *dummy = [[[NSTextView alloc] init] autorelease];
	[dummy insertText:[self documentText]];
	[dummy startSpeaking:self];
}

@end
