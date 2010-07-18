
/**
 * OpenEmulator
 * Mac OS X Document View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation view.
 */

#import "DocumentView.h"
#import "Document.h"

#import "OEGL.h"
#import "OEHID.h"

#import "Host.h"

#define COCOA_LCTRL				(1 << 0)
#define COCOA_LSHIFT			(1 << 1)
#define COCOA_LALT				(1 << 3)
#define COCOA_LGUI				(1 << 5)
#define COCOA_RCTRL				(1 << 13)
#define COCOA_RSHIFT			(1 << 2)
#define COCOA_RALT				(1 << 6)
#define COCOA_RGUI				(1 << 4)

typedef struct
{
	int keyCode;
	int usageId;
} DocumentKeyMapInverseEntry;

// From:
// http://stuff.mit.edu/afs/sipb/project/darwin/src/
// modules/AppleADBKeyboard/AppleADBKeyboard.cpp
DocumentKeyMapInverseEntry documentKeyMapInverse[] = 
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
	{0x0a, HOST_HID_K_NON_US1},
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
	{0x51, HOST_HID_KP_EQUAL},
	{0x6a, HOST_HID_K_F16},
	{0x40, HOST_HID_K_F17},
	{0x4f, HOST_HID_K_F18},
	{0x50, HOST_HID_K_F19},
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

@implementation DocumentView

static void setCapture(void *userData, bool value)
{
	if (value)
		CGDisplayHideCursor(kCGDirectMainDisplay);
	else
		CGDisplayShowCursor(kCGDirectMainDisplay);
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
		NSOpenGLPFAWindow,
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
		Document *document = [[[self window] windowController] document];
		
		oegl = new OEGL();
		oehid = new OEHID((OEPAEmulation *)[document emulation], setCapture);
		
		memset(keyMap, sizeof(keyMap), 0);
		for (int i = 0;
			 i < sizeof(documentKeyMapInverse) / sizeof(DocumentKeyMapInverseEntry);
			 i++)
		{
			int keyCode = documentKeyMapInverse[i].keyCode;
			int usageId = documentKeyMapInverse[i].usageId;
			keyMap[keyCode] = usageId;
		}
		
		keyModifierFlags = 0;
	}
	
	return self;
}

- (void)dealloc
{
    CVDisplayLinkRelease(displayLink);
	
	if (oehid)
		delete (OEHID *)oehid;
	if (oegl)
		delete (OEGL *)oegl;
	
	[super dealloc];
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
									const CVTimeStamp *now,
									const CVTimeStamp *outputTime,
									CVOptionFlags flagsIn,
									CVOptionFlags *flagsOut,
									void *displayLinkContext)
{
    [(DocumentView *)displayLinkContext drawFrame];
	
    return kCVReturnSuccess;
}

- (void)prepareOpenGL
{
	GLint value = 1;
	[[self openGLContext] setValues:&value forParameter:NSOpenGLCPSwapInterval]; 
	
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
	NSOpenGLContext	*currentContext = [self openGLContext];
	[currentContext makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[currentContext CGLContextObj]);
	
	[[self openGLContext] update];
	
	CGLUnlockContext((CGLContextObj)[currentContext CGLContextObj]);
}

- (void)drawRect:(NSRect)theRect
{
	[self drawFrame];
}

- (void)drawFrame
{
	NSOpenGLContext	*currentContext = [self openGLContext];
	[currentContext makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[currentContext CGLContextObj]);
	
	NSRect frame = [self bounds];
	((OEGL *)oegl)->draw(NSWidth(frame), NSHeight(frame));
	
	[currentContext flushBuffer];
	
	CGLUnlockContext((CGLContextObj)[currentContext CGLContextObj]);
}

- (int)getUsageId:(int)keyCode
{
	int usageId = (keyCode < DOCUMENT_KEYMAP_SIZE) ? keyMap[keyCode] : 0;
	
	if (!usageId)
		NSLog(@"Unknown key code %d", keyCode);
	
	return usageId;
}

- (void)updateFlags:(int)flags
			forMask:(int)mask
			usageId:(int)usageId
{
	if ((flags & mask) == (keyModifierFlags & mask))
		return;
	
	BOOL value = ((flags & mask) != 0);
	
	((OEHID *)oehid)->setKey(usageId, value);
}

- (void)keyDown:(NSEvent *)theEvent
{
	NSString *characters = [theEvent characters];
	for (int i = 0; i < [characters length]; i++)
		((OEHID *)oehid)->sendUnicode([characters characterAtIndex:i]);
	
	if (![theEvent isARepeat])
		((OEHID *)oehid)->setKey([self getUsageId:[theEvent keyCode]], true);
}

- (void)keyUp:(NSEvent *)theEvent
{
	((OEHID *)oehid)->setKey([self getUsageId:[theEvent keyCode]], false);
}

- (void)flagsChanged:(NSEvent *)theEvent
{
	int flags = [theEvent modifierFlags];
	
	[self updateFlags:flags forMask:COCOA_LCTRL usageId:HOST_HID_K_LEFTCONTROL];
	[self updateFlags:flags forMask:COCOA_LSHIFT usageId:HOST_HID_K_LEFTSHIFT];
	[self updateFlags:flags forMask:COCOA_LALT usageId:HOST_HID_K_LEFTALT];
	[self updateFlags:flags forMask:COCOA_LGUI usageId:HOST_HID_K_LEFTGUI];
	[self updateFlags:flags forMask:COCOA_RCTRL usageId:HOST_HID_K_RIGHTCONTROL];
	[self updateFlags:flags forMask:COCOA_RSHIFT usageId:HOST_HID_K_RIGHTSHIFT];
	[self updateFlags:flags forMask:COCOA_RALT usageId:HOST_HID_K_RIGHTALT];
	[self updateFlags:flags forMask:COCOA_RGUI usageId:HOST_HID_K_RIGHTGUI];
	
	// To-Do: NSAlphaShiftKeyMask
	
	keyModifierFlags = flags;	
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSPoint position = [NSEvent mouseLocation];
	
	((OEHID *)oehid)->setMousePosition(position.x, position.y);
	((OEHID *)oehid)->moveMouse([theEvent deltaX], [theEvent deltaY]);
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
	((OEHID *)oehid)->setMouseButton(0, true);
}

- (void)mouseUp:(NSEvent *)theEvent
{
	((OEHID *)oehid)->setMouseButton(0, false);
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	((OEHID *)oehid)->setMouseButton(1, true);
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
	((OEHID *)oehid)->setMouseButton(1, false);
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
	((OEHID *)oehid)->setMouseButton([theEvent buttonNumber], true);
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
	((OEHID *)oehid)->setMouseButton([theEvent buttonNumber], false);
}

- (void)scrollWheel:(NSEvent *)theEvent
{
	((OEHID *)oehid)->sendMouseWheelEvent(0, [theEvent deltaX]);
	((OEHID *)oehid)->sendMouseWheelEvent(1, [theEvent deltaY]);
}

- (void)powerDown:(id)sender
{
	((OEHID *)oehid)->sendSystemEvent(HOST_HID_S_POWERDOWN);
}

- (void)sleep:(id)sender
{
	((OEHID *)oehid)->sendSystemEvent(HOST_HID_S_SLEEP);
}

- (void)wakeUp:(id)sender
{
	((OEHID *)oehid)->sendSystemEvent(HOST_HID_S_WAKEUP);
}

- (void)restart:(id)sender
{
	((OEHID *)oehid)->sendSystemEvent(HOST_HID_S_COLDRESTART);
}

- (void)debuggerBreak:(id)sender
{
	((OEHID *)oehid)->sendSystemEvent(HOST_HID_S_DEBUGGERBREAK);
}

@end
