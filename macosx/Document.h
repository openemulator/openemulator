
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import <Cocoa/Cocoa.h>

#define TEMPLATE_FOLDER @"~/Library/Application Support/Open Emulator/Templates"

typedef struct OEEmulation OEEmulation;

@interface Document : NSDocument
{
	OEEmulation *emulation;
	
	NSPasteboard *pasteboard;
	NSArray *pasteboardTypes;
	
	NSString *label;
	NSString *group;
	NSString *description;
	NSString *modificationDate;
	NSString *runTime;
	NSImage *image;
	
	NSMutableArray *freeInlets;
	
	NSMutableArray *expansions;
	NSMutableArray *storage;
	NSMutableArray *peripherals;
	
	BOOL power;
}

- (id) initWithTemplateURL:(NSURL *) templateURL error:(NSError **) outError;

- (IBAction) saveDocumentAsTemplate:(id) sender;

- (void) tick:(float) ms;

- (void) addDevices:(NSString *) path connections:(NSDictionary *) connections;
- (void) removeDevice:(NSDictionary *) dict;

- (void) powerButtonPressedAndReleased:(id) sender;
- (void) powerButtonPressed:(id) sender;
- (void) powerButtonReleased:(id) sender;
- (void) resetButtonPressedAndReleased:(id) sender;
- (void) resetButtonPressed:(id) sender;
- (void) resetButtonReleased:(id) sender;
- (void) pauseButtonPressedAndReleased:(id) sender;
- (void) pauseButtonPressed:(id) sender;
- (void) pauseButtonReleased:(id) sender;
- (void) interruptButtonPressedAndReleased:(id) sender;
- (void) interruptButtonPressed:(id) sender;
- (void) interruptButtonReleased:(id) sender;

- (BOOL) isCopyValid;
- (BOOL) isPasteValid;

- (BOOL) power;
- (void) setPower:(BOOL) value;
- (NSString *) label;
- (void) setLabel:(NSString *) value;
- (NSString *) description;
- (void) setDescription:(NSString *) value;
- (NSString *) modificationDate;
- (void) setModificationDate:(NSString *) value;
- (NSString *) runTime;
- (void) setRunTime:(NSString *) value;
- (NSImage *) image;
- (void) setImage:(NSImage *) value;

- (NSMutableArray *) freeInlets;

- (NSMutableArray *) expansions;
- (void) insertObject:(id) value inExpansionsAtIndex:(NSUInteger) index;
- (void) removeObjectFromExpansionsAtIndex:(NSUInteger) index;

- (NSMutableArray *) storage;
- (void) insertObject:(id) value inStorageAtIndex:(NSUInteger) index;
- (void) removeObjectFromStorageAtIndex:(NSUInteger) index;

- (NSMutableArray *) peripherals;
- (void) insertObject:(id) value inPeripheralsAtIndex:(NSUInteger) index;
- (void) removeObjectFromPeripheralsAtIndex:(NSUInteger) index;

@end
