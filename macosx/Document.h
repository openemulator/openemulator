
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import <Cocoa/Cocoa.h>

@interface Document : NSDocument {
	IBOutlet id fWindow;
	
	NSPasteboard *pasteboard;
	NSArray *pasteboardTypes;
	
	BOOL isFullscreen;
	NSWindow *fullscreenWindow;
}

- (BOOL)validateCopy;
- (BOOL)validatePaste;

- (void)togglePower:(id)sender;
- (void)resetEmulation:(id)sender;
- (void)togglePause:(id)sender;

@end
