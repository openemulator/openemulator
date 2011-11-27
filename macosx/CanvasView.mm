
/**
 * OpenEmulator
 * Mac OS X Canvas View
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas view
 */

#import "Carbon/Carbon.h"

#import "CanvasView.h"

#import "NSStringAdditions.h"

#import "CanvasWindowController.h"
#import "Application.h"
#import "DocumentController.h"

#import "DeviceInterface.h"
#import "OpenGLCanvas.h"

#define NSLeftControlKeyMask	0x00000001
#define NSLeftShiftKeyMask		0x00000002
#define NSLeftAlternateKeyMask	0x00000020
#define NSLeftCommandKeyMask	0x00000008
#define NSRightControlKeyMask	0x00002000
#define NSRightShiftKeyMask		0x00000004
#define NSRightAlternateKeyMask	0x00000040
#define NSRightCommandKeyMask	0x00000010

@implementation CanvasView

// Callback methods

static void setCapture(void *userData, OpenGLCanvasCapture value)
{
    BOOL isCapture = (value != OPENGLCANVAS_CAPTURE_NONE);
    BOOL enableMouseCursor = (value !=
                              OPENGLCANVAS_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR);
    
    [(Application *)NSApp setCapture:isCapture];
    
    if (isCapture)
        CGDisplayHideCursor(kCGDirectMainDisplay);
    else
        CGDisplayShowCursor(kCGDirectMainDisplay);
    
    PushSymbolicHotKeyMode(isCapture ?
                           kHIHotKeyModeAllDisabled : 
                           kHIHotKeyModeAllEnabled);
    
    CGAssociateMouseAndMouseCursorPosition(enableMouseCursor);
}

static void setKeyboardLEDs(void *userData, CanvasKeyboardLEDs value)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    [(CanvasView *)userData setKeyboardLEDs:value];
    
    [pool drain];
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp *now,
                                    const CVTimeStamp *outputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags *flagsOut,
                                    void *displayLinkContext)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    [(CanvasView *)displayLinkContext vsync];
    
    [pool drain];
    
    return kCVReturnSuccess;
}

// Class

- (id)initWithFrame:(NSRect)rect
{
    NSOpenGLPixelFormatAttribute pixelFormatAtrributes[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize,
        32,
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
    
    self = [super initWithFrame:rect pixelFormat:pixelFormat];
    
    if (self)
    {
        // From:
        //   http://stuff.mit.edu/afs/sipb/project/darwin/src/
        //   modules/AppleADBKeyboard/AppleADBKeyboard.cpp
        keyMap[0x00] = CANVAS_K_A;
        keyMap[0x0b] = CANVAS_K_B;
        keyMap[0x08] = CANVAS_K_C;
        keyMap[0x02] = CANVAS_K_D;
        keyMap[0x0e] = CANVAS_K_E;
        keyMap[0x03] = CANVAS_K_F;
        keyMap[0x05] = CANVAS_K_G;
        keyMap[0x04] = CANVAS_K_H;
        keyMap[0x22] = CANVAS_K_I;
        keyMap[0x26] = CANVAS_K_J;
        keyMap[0x28] = CANVAS_K_K;
        keyMap[0x25] = CANVAS_K_L;
        keyMap[0x2e] = CANVAS_K_M;
        keyMap[0x2d] = CANVAS_K_N;
        keyMap[0x1f] = CANVAS_K_O;
        keyMap[0x23] = CANVAS_K_P;
        keyMap[0x0c] = CANVAS_K_Q;
        keyMap[0x0f] = CANVAS_K_R;
        keyMap[0x01] = CANVAS_K_S;
        keyMap[0x11] = CANVAS_K_T;
        keyMap[0x20] = CANVAS_K_U;
        keyMap[0x09] = CANVAS_K_V;
        keyMap[0x0d] = CANVAS_K_W;
        keyMap[0x07] = CANVAS_K_X;
        keyMap[0x10] = CANVAS_K_Y;
        keyMap[0x06] = CANVAS_K_Z;
        keyMap[0x12] = CANVAS_K_1;
        keyMap[0x13] = CANVAS_K_2;
        keyMap[0x14] = CANVAS_K_3;
        keyMap[0x15] = CANVAS_K_4;
        keyMap[0x17] = CANVAS_K_5;
        keyMap[0x16] = CANVAS_K_6;
        keyMap[0x1a] = CANVAS_K_7;
        keyMap[0x1c] = CANVAS_K_8;
        keyMap[0x19] = CANVAS_K_9;
        keyMap[0x1d] = CANVAS_K_0;
        keyMap[0x24] = CANVAS_K_ENTER;
        keyMap[0x35] = CANVAS_K_ESCAPE;
        keyMap[0x33] = CANVAS_K_BACKSPACE;
        keyMap[0x30] = CANVAS_K_TAB;
        keyMap[0x31] = CANVAS_K_SPACE;
        keyMap[0x1b] = CANVAS_K_MINUS;
        keyMap[0x18] = CANVAS_K_EQUAL;
        keyMap[0x21] = CANVAS_K_LEFTBRACKET;
        keyMap[0x1e] = CANVAS_K_RIGHTBRACKET;
        keyMap[0x2a] = CANVAS_K_BACKSLASH;
        keyMap[0x0a] = CANVAS_K_NON_US1;
        keyMap[0x29] = CANVAS_K_SEMICOLON;
        keyMap[0x27] = CANVAS_K_QUOTE;
        keyMap[0x32] = CANVAS_K_GRAVEACCENT;
        keyMap[0x2b] = CANVAS_K_COMMA;
        keyMap[0x2f] = CANVAS_K_PERIOD;
        keyMap[0x2c] = CANVAS_K_SLASH;
        keyMap[0x39] = CANVAS_K_CAPSLOCK;
        keyMap[0x7a] = CANVAS_K_F1;
        keyMap[0x78] = CANVAS_K_F2;
        keyMap[0x63] = CANVAS_K_F3;
        keyMap[0x76] = CANVAS_K_F4;
        keyMap[0x60] = CANVAS_K_F5;
        keyMap[0x61] = CANVAS_K_F6;
        keyMap[0x62] = CANVAS_K_F7;
        keyMap[0x64] = CANVAS_K_F8;
        keyMap[0x65] = CANVAS_K_F9;
        keyMap[0x6d] = CANVAS_K_F10;
        keyMap[0x67] = CANVAS_K_F11;
        keyMap[0x6f] = CANVAS_K_F12;
        keyMap[0x69] = CANVAS_K_PRINTSCREEN;
        keyMap[0x6b] = CANVAS_K_SCROLLLOCK;
        keyMap[0x71] = CANVAS_K_PAUSE;
        keyMap[0x72] = CANVAS_K_INSERT;
        keyMap[0x73] = CANVAS_K_HOME;
        keyMap[0x74] = CANVAS_K_PAGEUP;
        keyMap[0x75] = CANVAS_K_DELETE;
        keyMap[0x77] = CANVAS_K_END;
        keyMap[0x79] = CANVAS_K_PAGEDOWN;
        keyMap[0x7c] = CANVAS_K_RIGHT;
        keyMap[0x7b] = CANVAS_K_LEFT;
        keyMap[0x7d] = CANVAS_K_DOWN;
        keyMap[0x7e] = CANVAS_K_UP;
        keyMap[0x47] = CANVAS_KP_NUMLOCK;
        keyMap[0x4b] = CANVAS_KP_SLASH;
        keyMap[0x43] = CANVAS_KP_STAR;
        keyMap[0x4e] = CANVAS_KP_MINUS;
        keyMap[0x45] = CANVAS_KP_PLUS;
        keyMap[0x4c] = CANVAS_KP_ENTER;
        keyMap[0x53] = CANVAS_KP_1;
        keyMap[0x54] = CANVAS_KP_2;
        keyMap[0x55] = CANVAS_KP_3;
        keyMap[0x56] = CANVAS_KP_4;
        keyMap[0x57] = CANVAS_KP_5;
        keyMap[0x58] = CANVAS_KP_6;
        keyMap[0x59] = CANVAS_KP_7;
        keyMap[0x5b] = CANVAS_KP_8;
        keyMap[0x5c] = CANVAS_KP_9;
        keyMap[0x52] = CANVAS_KP_0;
        keyMap[0x41] = CANVAS_KP_PERIOD;
        keyMap[0x51] = CANVAS_KP_EQUAL;
        keyMap[0x6a] = CANVAS_K_F16;
        keyMap[0x40] = CANVAS_K_F17;
        keyMap[0x4f] = CANVAS_K_F18;
        keyMap[0x50] = CANVAS_K_F19;
        keyMap[0x7f] = CANVAS_K_POWER;
        
        keyMap[0x3b] = CANVAS_K_LEFTCONTROL;
        keyMap[0x38] = CANVAS_K_LEFTSHIFT;
        keyMap[0x3a] = CANVAS_K_LEFTALT;
        keyMap[0x37] = CANVAS_K_LEFTGUI;
        keyMap[0x3e] = CANVAS_K_RIGHTCONTROL;
        keyMap[0x3c] = CANVAS_K_RIGHTSHIFT;
        keyMap[0x3d] = CANVAS_K_RIGHTALT;
        keyMap[0x36] = CANVAS_K_RIGHTGUI;
    }
    
    return self;
}

- (void)dealloc
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults removeObserver:self
                      forKeyPath:@"OEVideoEnableShader"];
    
    if (displayLink)
        CVDisplayLinkRelease(displayLink);
    
    [super dealloc];
}

// Window

- (void)awakeFromNib
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    canvas->setEnableShader([userDefaults boolForKey:@"OEVideoEnableShader"]);
    [userDefaults addObserver:self
                   forKeyPath:@"OEVideoEnableShader"
                      options:NSKeyValueObservingOptionNew
                      context:nil];
    
    [self registerForDraggedTypes:[NSArray arrayWithObjects:
                                   NSStringPboardType,
                                   NSFilenamesPboardType, 
                                   nil]];
    
    [self initOpenGL];
}

- (BOOL)validateUserInterfaceItem:(id)anItem
{
    SEL action = [anItem action];
    
    if (action == @selector(copy:))
        return ![self isPaperCanvas];
    else if (action == @selector(paste:))
    {
        NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
        return [[pasteboard types] containsObject:NSStringPboardType];
    }
    else if (action == @selector(delete:))
        return [self isPaperCanvas];
    else if ((action == @selector(sendPowerDown:)) ||
             (action == @selector(sendSleep:)) ||
             (action == @selector(sendWakeUp:)) ||
             (action == @selector(sendColdRestart:)) ||
             (action == @selector(sendWarmRestart:)) ||
             (action == @selector(sendDebuggerBreak:)))
    {
        CanvasWindowController *canvasWindowController = [[self window] windowController];
        OEComponent *device = (OEComponent *)[canvasWindowController device];
        
        if (device)
        {
            bool isEventObservers = false;
            
            device->postMessage(NULL, DEVICE_IS_OBSERVED, &isEventObservers);
            
            return isEventObservers;
        }
    }
    
    return YES;
}

- (BOOL)isFlipped
{
    return YES;
}

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)isMouseInView
{
    NSPoint mouseLocation = [self convertPoint:[[self window] convertScreenToBase:
                                                [NSEvent mouseLocation]]
                                      fromView:nil];
    
    return NSMouseInRect(mouseLocation, [self bounds], [self isFlipped]);
}

- (void)windowDidResize
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    NSSize contentSize = [[self enclosingScrollView] contentSize];
    
    CGLLockContext(cglContextObj);
    
    [[self openGLContext] makeCurrentContext];
    
    canvas->setViewportSize(OEMakeSize(contentSize.width,
                                       contentSize.height));
    
    [NSOpenGLContext clearCurrentContext];
    
    CGLUnlockContext(cglContextObj);
    
    OESize newCanvasSize = canvas->getSize();
    OERect clipRect = canvas->getClipRect();
    
    canvasSize = NSMakeSize(newCanvasSize.width, newCanvasSize.height);
    
    NSRect frame = NSMakeRect(0,
                              0,
                              contentSize.width,
                              canvasSize.height * contentSize.height /
                              clipRect.size.height);
    if (frame.size.height < contentSize.height)
        frame.size.height = contentSize.height;
    [self setFrame:frame];
    
    [self scrollPoint:NSMakePoint(0,
                                  clipRect.origin.y * frame.size.height /
                                  canvasSize.height)];
}

- (void)windowDidBecomeKey
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    ((OpenGLCanvas *)canvas)->becomeKeyWindow();
    
    [self synchronizeKeyboardLEDs];
    
    [document unlockEmulation];
    
    if ([self isMouseInView])
        [self mouseEntered:nil];
    
/*    NSTrackingAreaOptions options = (NSTrackingMouseEnteredAndExited |
                                     NSTrackingMouseMoved |
                                     NSTrackingActiveInKeyWindow);
    NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                        options:options
                                                          owner:self
                                                       userInfo:nil];
    
    [self addTrackingArea:area];
    
    [area release];*/
}

- (void)windowDidResignKey
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    if ([self isMouseInView])
        [self mouseExited:nil];
    
    [document lockEmulation];
    
    canvas->resignKeyWindow();
    
    [document unlockEmulation];
    
/*    for (NSTrackingArea *area in [self trackingAreas])
        if ([area owner] == self)
            [self removeTrackingArea:area];*/
}

// Drag and drop

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
            CanvasWindowController *canvasWindowController = [[self window] windowController];
            Document *document = [canvasWindowController document];
            
            if ([document canMountNow:path])
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
        return [documentController application:NSApp openFile:path];
    }
    else if ([[pasteboard types] containsObject:NSStringPboardType])
    {
        CanvasWindowController *canvasWindowController = [[self window] windowController];
        Document *document = [canvasWindowController document];
        OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
        
        if (!canvas)
            return NO;
        
        wstring clipboard = [[pasteboard stringForType:NSStringPboardType] cppWString];
        
        [document lockEmulation];
        
        canvas->doPaste(clipboard);
        
        [document unlockEmulation];
        
        return YES;
    }
    
    return NO;
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    id theObject = [change objectForKey:NSKeyValueChangeNewKey];
    
    if ([keyPath isEqualToString:@"OEVideoEnableShader"])
        canvas->setEnableShader([theObject boolValue]);
}

// Drawing

- (void)initOpenGL
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    GLint value = 1;
    [[self openGLContext] setValues:&value
                       forParameter:NSOpenGLCPSwapInterval]; 
    
    [[self openGLContext] makeCurrentContext];
    
    canvas->open(setCapture, setKeyboardLEDs, self);
    
    [NSOpenGLContext clearCurrentContext];
    
    if (CVDisplayLinkCreateWithActiveCGDisplays(&displayLink) == kCVReturnSuccess)
    {
        CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, self);
        
        cglContextObj = (CGLContextObj)[[self openGLContext] CGLContextObj];
        cglPixelFormatObj = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
        
        CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,
                                                          cglContextObj,
                                                          cglPixelFormatObj);
    }
}

- (void)freeOpenGL
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [[self openGLContext] makeCurrentContext];
    
    canvas->close();
    
    [NSOpenGLContext clearCurrentContext];
}

- (void)enterContext
{
    CGLLockContext(cglContextObj);
    
    [[self openGLContext] makeCurrentContext];
}

- (void)leaveContext
{
    CGLUnlockContext(cglContextObj);
}

- (void)startDisplayLink
{
    CVDisplayLinkStart(displayLink);
    
    /*	[[NSNotificationCenter defaultCenter] addObserver:self
     selector:@selector(globalFrameDidChange:)
     name:NSViewGlobalFrameDidChangeNotification
     object:self];*/
}

- (void)stopDisplayLink
{
    CVDisplayLinkStop(displayLink);
    
    //	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (BOOL)displayLinkRunning
{
    return CVDisplayLinkIsRunning(displayLink);
}

- (void)globalFrameDidChange:(NSNotification*)notification
{
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,
                                                      cglContextObj,
                                                      cglPixelFormatObj);
}

- (BOOL)isDisplayCanvas
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return NO;
    
    [document lockEmulation];
    
    CanvasMode value = canvas->getMode();
    
    [document unlockEmulation];
    
    return (value == CANVAS_MODE_DISPLAY);
}

- (BOOL)isPaperCanvas
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return NO;
    
    [document lockEmulation];
    
    CanvasMode value = canvas->getMode();
    
    [document unlockEmulation];
    
    return (value == CANVAS_MODE_PAPER);
}

- (NSSize)defaultViewSize
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return NSMakeSize(0, 0);
    
    OESize size = OEMakeSize(0, 0);
    
    [document lockEmulation];
    
    size = canvas->getDefaultViewportSize();
    
    [document unlockEmulation];
    
    NSSize defaultViewSize;
    defaultViewSize.width = (size.width < 128) ? 128 : size.width;
    defaultViewSize.height = (size.height < 128) ? 128 : size.height;
    
    return defaultViewSize;
}

- (void)update
{
    [self enterContext];
    
    [super update];
    
    [self leaveContext];
}

- (void)drawRect:(NSRect)theRect
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [self enterContext];
    
    float canvasHeight = canvas->getSize().height;
    canvas->scrollPoint(OEMakePoint(0,
                                    canvasHeight *
                                    NSMinY([self visibleRect]) /
                                    NSHeight([self frame])));
    
    if (![self displayLinkRunning])
    {
        if (canvas->vsync())
            canvas->draw();
    }
    else
        canvas->draw();
    
    [[self openGLContext] flushBuffer];
    
    [self leaveContext];
}

- (void)vsync
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [self enterContext];
    
    OESize newCanvasSize = canvas->getSize();
    if (canvasSize.height != newCanvasSize.height)
        [self performSelectorOnMainThread:@selector(windowDidResize)
                               withObject:nil
                            waitUntilDone:NO];
    canvasSize = NSMakeSize(newCanvasSize.width, newCanvasSize.height);
    
    if (canvas->vsync())
        [[self openGLContext] flushBuffer];
    
    [self leaveContext];
}

// Printing

- (NSSize)canvasSize
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return NSMakeSize(0, 0);
        
    OESize size = canvas->getSize();
    
    return NSMakeSize(size.width, size.height);
}

- (NSSize)canvasPixelDensity
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return NSMakeSize(0, 0);
    
    OESize pixelDensity = canvas->getPixelDensity();
    
    return NSMakeSize(pixelDensity.width, pixelDensity.height);
}

- (NSSize)pageSize
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return NSMakeSize(0, 0);
    
    OESize size = canvas->getPageSize();
    
    return NSMakeSize(size.width, size.height);
}

- (NSImage *)canvasImage:(NSRect)rect
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return nil;
    
    [self enterContext];
    
    OEImage image = canvas->getImage(OEMakeRect(rect.origin.x,
                                                rect.origin.y,
                                                rect.size.width,
                                                rect.size.height));
    
    [self leaveContext];
    
    OESize size = image.getSize();
    NSInteger bytesPerPixel = image.getBytesPerPixel();
    NSInteger bytesPerRow = image.getBytesPerRow();
    unsigned char *data = image.getPixels();
    
    NSBitmapImageRep *rep;
    rep = [[[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&data
                                                   pixelsWide:size.width
                                                   pixelsHigh:size.height
                                                bitsPerSample:8
                                              samplesPerPixel:bytesPerPixel
                                                     hasAlpha:(bytesPerPixel == 4)
                                                     isPlanar:NO
                                               colorSpaceName:NSCalibratedRGBColorSpace
                                                 bitmapFormat:0
                                                  bytesPerRow:bytesPerRow
                                                 bitsPerPixel:8 * bytesPerPixel]
           autorelease];
    
    NSImage *theImage = [[[NSImage alloc] init] autorelease];
    [theImage addRepresentation:rep];
    
    [theImage setFlipped:YES];
    // This flips the rep
    [theImage lockFocusOnRepresentation:rep];
    [theImage unlockFocus];
    
    return theImage;
}

// Keyboard

- (int)getUsageId:(int)keyCode
{
    int usageId = (keyCode < DEVICE_KEYMAP_SIZE) ? keyMap[keyCode] : 0;
    
    if (!usageId)
        NSLog(@"Unknown key code %d", keyCode);
    
    return usageId;
}

- (void)sendUnicodeKeyEvent:(int)unicode
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->sendUnicodeChar((CanvasUnicodeChar)unicode);
    
    [document unlockEmulation];
}

- (void)updateFlags:(int)flags
            forMask:(int)mask
            usageId:(int)usageId
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    if ((flags & mask) == (keyModifierFlags & mask))
        return;
    
    BOOL value = ((flags & mask) != 0);
    
    [document lockEmulation];
    
    canvas->setKey(usageId, value);
    
    [document unlockEmulation];
}

- (void)synchronizeKeyboardLEDs
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    CGEventRef event = CGEventCreate(NULL);
    CGEventFlags modifierFlags = CGEventGetFlags(event);
    CFRelease(event);
    
    bool hostCapsLock = modifierFlags & NSAlphaShiftKeyMask;
    bool emulationCapsLock = OEGetBit(keyboardLEDs, CANVAS_KL_CAPSLOCK);
    if (hostCapsLock != emulationCapsLock)
    {
        if (!capsLockNotSynchronized)
        {
            capsLockNotSynchronized = true;
            
            canvas->setKey(CANVAS_K_CAPSLOCK, true);
            canvas->setKey(CANVAS_K_CAPSLOCK, false);
        }
    }
    else
        capsLockNotSynchronized = false;
}

- (void)setKeyboardLEDs:(int)theKeyboardLEDs
{
    keyboardLEDs = theKeyboardLEDs;
    
    [self synchronizeKeyboardLEDs];
}

- (void)keyDown:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    NSString *characters = [theEvent characters];
    for (NSInteger i = 0; i < [characters length]; i++)
    {
        int c = [characters characterAtIndex:i];
        if (c == 0x0d)
            [self sendUnicodeKeyEvent:0x0a];
        else if (c == 0x0a)
            [self sendUnicodeKeyEvent:0x0d];
        else if (c == 0x7f)
            [self sendUnicodeKeyEvent:0x08];
        else if (c == 0xf728)
            [self sendUnicodeKeyEvent:0x7f];
        else
            [self sendUnicodeKeyEvent:c];
    }
    
    if (![theEvent isARepeat])
    {
        int usageId = [self getUsageId:[theEvent keyCode]];
        
        [document lockEmulation];
        
        canvas->setKey(usageId, true);
        
        [document unlockEmulation];
    }
}

- (void)keyUp:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    int usageId = [self getUsageId:[theEvent keyCode]];
    
    [document lockEmulation];
    
    canvas->setKey(usageId, false);
    
    [document unlockEmulation];
}

- (void)flagsChanged:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    
    int flags = (int) [theEvent modifierFlags];
    
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
    
    [document lockEmulation];
    
    [self synchronizeKeyboardLEDs];
    
    [document unlockEmulation];
}

// Mouse

- (void)mouseEntered:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->enterMouse();
    
    [document unlockEmulation];
}

- (void)mouseExited:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->exitMouse();
    
    [document unlockEmulation];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    NSPoint position = [NSEvent mouseLocation];
    
    [document lockEmulation];
    
    canvas->setMousePosition(position.x, position.y);
    canvas->moveMouse([theEvent deltaX], [theEvent deltaY]);
    
    [document unlockEmulation];
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
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->setMouseButton(0, true);
    
    [document unlockEmulation];
}

- (void)mouseUp:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->setMouseButton(0, false);
    
    [document unlockEmulation];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->setMouseButton(1, true);
    
    [document unlockEmulation];
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->setMouseButton(1, false);
    
    [document unlockEmulation];
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->setMouseButton((int) [theEvent buttonNumber], true);
    
    [document unlockEmulation];
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->setMouseButton((int) [theEvent buttonNumber], false);
    
    [document unlockEmulation];
}

- (void)scrollWheel:(NSEvent *)theEvent
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    if ([self isPaperCanvas])
        return [super scrollWheel:theEvent];
    
    [document lockEmulation];
    
    if ([theEvent deltaX])
        canvas->sendMouseWheelEvent(0, [theEvent deltaX]);
    if ([theEvent deltaY])
        canvas->sendMouseWheelEvent(1, [theEvent deltaY]);
    
    [document unlockEmulation];
}

// Copy/paste

- (NSString *)copyString
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return @"";
    
    wstring clipboard;
    
    [document lockEmulation];
    
    canvas->doCopy(clipboard);
    
    [document unlockEmulation];
    
    return [NSString stringWithCPPWString:clipboard];
}

- (void)copy:(id)sender
{
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSString *theString = [self copyString];
    
    if ([theString length])
    {
        NSArray *pasteboardTypes = [NSArray arrayWithObjects:NSStringPboardType, nil];
        [pasteboard declareTypes:pasteboardTypes owner:self];
        
        [pasteboard setString:theString
                      forType:NSStringPboardType];
    }
    else
    {
        NSArray *pasteboardTypes = [NSArray arrayWithObjects:NSTIFFPboardType, nil];
        [pasteboard declareTypes:pasteboardTypes owner:self];
        
        NSRect rect;
        rect.origin = NSMakePoint(0, 0);
        rect.size = [self canvasSize];
        
        NSImage *image = [self canvasImage:rect];
        
        [pasteboard setData:[image TIFFRepresentation]
                    forType:NSTIFFPboardType];
    }
}

- (void)pasteString:(NSString *)text
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    if (!text)
        return;
    
    [document lockEmulation];
    
    canvas->doPaste([text cppWString]);
    
    [document unlockEmulation];
}

- (void)paste:(id)sender
{
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    [self pasteString:[pasteboard stringForType:NSStringPboardType]];
}

- (void)startSpeaking:(id)sender
{
    NSString *theString = [self copyString];
    if (theString)
    {
        NSTextView *dummy = [[[NSTextView alloc] init] autorelease];
        [dummy insertText:theString];
        [dummy startSpeaking:self];
    }
}

- (void)delete:(id)sender
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    Document *document = [canvasWindowController document];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasWindowController canvas];
    
    if (!canvas)
        return;
    
    [document lockEmulation];
    
    canvas->doDelete();
    
    [document unlockEmulation];
}

// System events

- (void)sendEvent:(int)value
{
    CanvasWindowController *canvasWindowController = [[self window] windowController];
    OEComponent *device = (OpenGLCanvas *)[canvasWindowController device];
    
    if (!device)
        return;
    
    DeviceEvent event = (DeviceEvent)value;
    device->postNotification(device, DEVICE_DID_CHANGE, &event);
}

- (void)sendPowerDown:(id)sender
{
    [self sendEvent:DEVICE_POWERDOWN];
}

- (void)sendSleep:(id)sender
{
    [self sendEvent:DEVICE_SLEEP];
}

- (void)sendWakeUp:(id)sender
{
    [self sendEvent:DEVICE_WAKEUP];
}

- (void)sendColdRestart:(id)sender
{
    [self sendEvent:DEVICE_COLDRESTART];
}

- (void)sendWarmRestart:(id)sender
{
    [self sendEvent:DEVICE_WARMRESTART];
}

- (void)sendDebuggerBreak:(id)sender
{
    [self sendEvent:DEVICE_DEBUGGERBREAK];
}

// Support for the text input system

- (BOOL)hasMarkedText
{
    return NO;
}

- (NSRange)markedRange
{
    return NSMakeRange(NSNotFound, 0);
}

- (NSRange)selectedRange
{
    return NSMakeRange(NSNotFound, 0);
}

- (void)setMarkedText:(id)aString
        selectedRange:(NSRange)selectedRange
     replacementRange:(NSRange)replacementRange
{
}

- (void)unmarkText
{
}

- (NSArray *)validAttributesForMarkedText
{
    return [NSArray array];
}

- (NSAttributedString *)attributedSubstringForProposedRange:(NSRange)aRange
                                                actualRange:(NSRangePointer)actualRange
{
    return nil;
}

- (void)insertText:(id)aString
  replacementRange:(NSRange)replacementRange
{
    for (NSInteger i = 0; i < [aString length]; i++)
        [self sendUnicodeKeyEvent:[aString characterAtIndex:i]];
}

- (NSUInteger)characterIndexForPoint:(NSPoint)aPoint
{
    return NSNotFound;
}

- (NSRect)firstRectForCharacterRange:(NSRange)aRange
                         actualRange:(NSRangePointer)actualRange
{
    return [self bounds];
}

- (void)doCommandBySelector:(SEL)aSelector
{
}

- (BOOL)drawsVerticallyForCharacterAtIndex:(NSUInteger)charIndex
{
    return NO;
}

@end
