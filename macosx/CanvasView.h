
/**
 * OpenEmulator
 * Mac OS X Canvas View
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas view
 */

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

#import "Document.h"

#define DEVICE_KEYMAP_SIZE		256
#define DEVICE_MOUSE_BUTTONNUM	8

@interface CanvasView : NSOpenGLView
<NSTextInputClient>
{
    CVDisplayLinkRef displayLink;
    CGLContextObj cglContextObj;
    CGLPixelFormatObj cglPixelFormatObj;
    
    NSSize canvasSize;
    BOOL needsReshape;
    
    int keyMap[DEVICE_KEYMAP_SIZE];
    int keyModifierFlags;
    int keyboardFlags;
    BOOL capsLockNotSynchronized;
}

- (void)windowDidResize;
- (void)windowDidBecomeKey;
- (void)windowDidResignKey;

- (void)initOpenGL;
- (void)freeOpenGL;

- (void)startDisplayLink;
- (void)stopDisplayLink;
- (BOOL)displayLinkRunning;

- (BOOL)isDisplayCanvas;
- (BOOL)isPaperCanvas;
- (NSSize)defaultViewSize;
- (void)vsync;

- (NSSize)canvasSize;
- (NSSize)canvasPixelDensity;
- (NSSize)pageSize;
- (NSImage *)canvasImage:(NSRect)rect;

- (void)setKeyboardFlags:(int)theKeyboardFlags;
- (void)synchronizeKeyboardFlags;

- (void)pasteString:(NSString *)text;

- (IBAction)sendPowerDown:(id)sender;
- (IBAction)sendSleep:(id)sender;
- (IBAction)sendWakeUp:(id)sender;
- (IBAction)sendColdRestart:(id)sender;
- (IBAction)sendWarmRestart:(id)sender;
- (IBAction)sendDebuggerBreak:(id)sender;

@end
