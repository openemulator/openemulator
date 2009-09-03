
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation document.
 */

#import <Cocoa/Cocoa.h>

@interface Document : NSDocument
{
	NSPasteboard *pasteboard;
	NSArray *pasteboardTypes;
	
	BOOL power;
	BOOL pause;
	NSString *label;
	NSString *description;
	NSDate *modificationDate;
	NSImage *image;

	NSMutableArray *expansions;
	NSMutableArray *diskDrives;
	NSMutableArray *peripherals;
	
	int videoPreset;
	float brightness;
	float contrast;
	float saturation;
	float sharpness;
	float temperature;
	float tint;
	
	float volume;
}

- (id)initWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;

- (IBAction)saveDocumentAsTemplate:(id)sender;

- (BOOL)isCopyValid;
- (BOOL)isPasteValid;

- (void)togglePower:(id)sender;
- (void)resetEmulation:(id)sender;
- (void)togglePause:(id)sender;

- (BOOL)power;
- (void)setPower:(BOOL)value;
- (BOOL)pause;
- (void)setPause:(BOOL)value;
- (NSString *)label;
- (void)setLabel:(NSString *)value;
- (NSString *)description;
- (void)setDescription:(NSString *)value;
- (NSDate *)modificationDate;
- (void)setModificationDate:(NSDate *)value;
- (NSImage *)image;
- (void)setImage:(NSImage *)value;

- (NSMutableArray *)expansions;
- (void)insertObject:(id)value inExpansionsAtIndex:(NSUInteger)index;
- (void)removeObjectFromExpansionsAtIndex:(NSUInteger)index;

- (NSMutableArray *)diskDrives;
- (void)insertObject:(id)value inDiskDrivesAtIndex:(NSUInteger)index;
- (void)removeObjectFromDiskDrivesAtIndex:(NSUInteger)index;

- (NSMutableArray *)peripherals;
- (void)insertObject:(id)value inPeripheralsAtIndex:(NSUInteger)index;
- (void)removeObjectFromPeripheralsAtIndex:(NSUInteger)index;

- (int)videoPreset;
- (void)setVideoPreset:(int)value;

- (float)brightness;
- (void)setBrightness:(float)value;
- (float)contrast;
- (void)setContrast:(float)value;
- (float)saturation;
- (void)setSaturation:(float)value;
- (float)sharpness;
- (void)setSharpness:(float)value;
- (float)temperature;
- (void)setTemperature:(float)value;
- (float)tint;
- (void)setTint:(float)value;
- (float)volume;
- (void)setVolume:(float)value;

@end
