
/**
 * OpenEmulator
 * Mac OS X Emulation Item
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation item.
 */

#import <Cocoa/Cocoa.h>

#import "Document.h"

@interface EmulationStorage : NSObject
{
	void *storage;
}

@end

@interface EmulationItem : NSObject
{
	Document *document;
	
	NSString *uid;
	NSImage *image;
	NSString *label;
	NSString *location;
	NSString *state;
	
	BOOL canvas;
	
	BOOL storage;
	BOOL mounted;
	NSString *diskImagePath;
	void *storageComponent;
	
	NSMutableArray *settingsRefs;
	NSMutableArray *settingsNames;
	NSMutableArray *settingsLabels;
	NSMutableArray *settingsTypes;
	NSMutableArray *settingsOptions;
	
	NSMutableArray *children;
}

- (id)initWithDocument:(Document *)theDocument;
- (id)initWithLabel:(NSString *)theLabel;
- (id)initWithDeviceInfo:(void *)theDeviceInfo
			  inDocument:(Document *)theDocument;
- (id)initWithDiskImageAtPath:(NSString *)thePath
					  storage:(void *)theComponent
					 location:(NSString *)theLocation
				   inDocument:(Document *)theDocument;

- (NSImage *)getImage:(NSString *)path;

- (NSString *)uid;
- (NSImage *)image;
- (NSString *)label;
- (NSString *)location;
- (NSString *)state;

- (BOOL)canvas;

- (BOOL)storage;
- (BOOL)mounted;
- (BOOL)locked;
- (NSString *)storagePath;
- (BOOL)mount:(NSString *)path;
- (void)unmount;

- (NSInteger)numberOfSettings;
- (NSString *)labelForSettingAtIndex:(NSInteger)index;
- (NSString *)typeForSettingAtIndex:(NSInteger)index;
- (NSArray *)optionsForSettingAtIndex:(NSInteger)index;
- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;
- (NSString *)valueForSettingAtIndex:(NSInteger)index;

- (NSMutableArray *)children;
- (EmulationItem *)childWithUid:(NSString *)theUid;

@end
