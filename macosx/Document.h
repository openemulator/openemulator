
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation document.
 */

#import <Cocoa/Cocoa.h>

@interface Document : NSDocument {
	NSPasteboard *pasteboard;
	NSArray *pasteboardTypes;
	
	NSString *computerType;
	BOOL power;
}

- (id)initWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;

- (IBAction)saveDocumentAsTemplate:(id)sender;

- (BOOL)isCopyValid;
- (BOOL)isPasteValid;

- (void)togglePower:(id)sender;
- (void)resetEmulation:(id)sender;
- (void)togglePause:(id)sender;

- (BOOL)power;
- (void)setPower:(BOOL)thePower;

@end
