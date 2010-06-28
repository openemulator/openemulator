
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
	
	NSImage *image;
	NSString *label;
	NSString *notes;
	NSString *modificationDate;
	NSString *powerState;
	
	NSMutableArray *freeInlets;
	
	NSMutableArray *expansions;
	NSMutableArray *storage;
	NSMutableArray *peripherals;
}

- (id) initWithTemplateURL:(NSURL *) templateURL error:(NSError **) outError;

- (IBAction) saveDocumentAsTemplate:(id) sender;

- (NSImage *) image;
- (void) setImage:(NSImage *) value;
- (NSString *) label;
- (void) setLabel:(NSString *) value;
- (NSString *) notes;
- (void) setNotes:(NSString *) value;
- (NSString *) modificationDate;
- (void) setModificationDate:(NSString *) value;
- (NSString *) powerState;
- (void) setPowerState:(NSString *) value;

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

- (void) addDevices:(NSString *) path connections:(NSDictionary *) connections;
- (void) removeDevice:(NSDictionary *) dict;

- (void) powerDownPressed:(id) sender;
- (void) sleepPressed:(id) sender;
- (void) wakeUpPressed:(id) sender;
- (void) restartPressed:(id) sender;
- (void) interruptPressed:(id) sender;

- (BOOL) isCopyValid;
- (BOOL) isPasteValid;

@end
