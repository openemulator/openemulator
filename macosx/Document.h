
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
	NSString *description;
	NSString *modificationDate;
	NSString *runTime;
	NSImage *image;
	
	NSMutableArray *expansions;
	NSMutableArray *diskDrives;
	NSMutableArray *peripherals;
	
	NSNumber *brightness;
	NSNumber *contrast;
	NSNumber *sharpness;
	NSNumber *saturation;
	NSNumber *temperature;
	NSNumber *tint;
	
	NSNumber *volume;

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
- (void) pauseButtonPressed:(id)sender;

- (BOOL) power;
- (void) setPower:(BOOL)value;
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

- (NSMutableArray *) expansions;
- (void) insertObject:(id) value inExpansionsAtIndex:(NSUInteger) index;
- (void) removeObjectFromExpansionsAtIndex:(NSUInteger) index;

- (NSMutableArray *) diskDrives;
- (void) insertObject:(id) value inDiskDrivesAtIndex:(NSUInteger) index;
- (void) removeObjectFromDiskDrivesAtIndex:(NSUInteger) index;

- (NSMutableArray *) peripherals;
- (void) insertObject:(id) value inPeripheralsAtIndex:(NSUInteger) index;
- (void) removeObjectFromPeripheralsAtIndex:(NSUInteger) index;

- (NSNumber *) brightness;
- (void) setBrightness:(NSNumber *) value;
- (NSNumber *) contrast;
- (void) setContrast:(NSNumber *) value;
- (NSNumber *) sharpness;
- (void) setSharpness:(NSNumber *) value;
- (NSNumber *) saturation;
- (void) setSaturation:(NSNumber *) value;
- (NSNumber *) temperature;
- (void) setTemperature:(NSNumber *) value;
- (NSNumber *) tint;
- (void) setTint:(NSNumber *) value;

- (NSNumber *) volume;
- (void) setVolume:(NSNumber *) value;

@end
