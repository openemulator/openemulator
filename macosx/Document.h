
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
	
	BOOL power;
	NSString *type;
	NSString *description;
	NSDate *modificationDate;
	NSImage *image;

	NSMutableArray *expansions;
	NSMutableArray *diskDrives;
	NSMutableArray *portDevices;
	
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
- (NSString *)type;
- (void)setType:(NSString *)value;
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

- (NSMutableArray *)portDevices;
- (void)insertObject:(id)value inPortDevicesAtIndex:(NSUInteger)index;
- (void)removeObjectFromPortDevicesAtIndex:(NSUInteger)index;

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
