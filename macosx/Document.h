
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation document.
 */

#import <Cocoa/Cocoa.h>

#define TEMPLATE_FOLDER @"~/Library/Application Support/Open Emulator/Templates"

@interface Document : NSDocument
{
	void *emulation;
	
	NSPasteboard *pasteboard;
	NSArray *pasteboardTypes;
	
	NSString *label;
	NSString *group;
	NSString *description;
	NSString *modificationDate;
	NSString *runTime;
	NSImage *image;
	
	NSMutableArray *expansions;
	NSMutableArray *diskDrives;
	NSMutableArray *peripherals;
	
	BOOL power;
	int videoPreset;
}

- (id) initFromTemplateURL:(NSURL *) templateURL error:(NSError **) outError;

- (IBAction) saveDocumentAsTemplate:(id)sender;

- (BOOL) isCopyValid;
- (BOOL) isPasteValid;

- (void) powerButtonPressedAndReleased:(id)sender;
- (void) powerButtonPressed:(id)sender;
- (void) powerButtonReleased:(id)sender;
- (void) resetButtonPressedAndReleased:(id)sender;
- (void) resetButtonPressed:(id)sender;
- (void) resetButtonReleased:(id)sender;
- (void) pauseButtonPressedAndReleased:(id)sender;
- (void) pauseButtonPressed:(id)sender;
- (void) pauseButtonReleased:(id)sender;

- (BOOL) power;
- (void) setPower:(BOOL)value;
- (NSString *) label;
- (void) setLabel:(NSString *) value;
- (NSString *) group;
- (void) setGroup:(NSString *) value;
- (NSString *) description;
- (void) setDescription:(NSString *) value;
- (NSString *) modificationDate;
- (void) setModificationDate:(NSString *) value;
- (NSString *) runTime;
- (void) setRunTime:(NSString *) value;
- (NSImage *) image;
- (void) setImage:(NSImage *) value;

- (NSMutableArray *) expansions;
- (void) insertObject:(id) value inExpansionsAtIndex:(NSUInteger) index;
- (void) removeObjectFromExpansionsAtIndex:(NSUInteger) index;

- (NSMutableArray *) diskDrives;
- (void) insertObject:(id) value inDiskDrivesAtIndex:(NSUInteger) index;
- (void) removeObjectFromDiskDrivesAtIndex:(NSUInteger) index;

- (NSMutableArray *) peripherals;
- (void) insertObject:(id) value inPeripheralsAtIndex:(NSUInteger) index;
- (void) removeObjectFromPeripheralsAtIndex:(NSUInteger) index;

@end
