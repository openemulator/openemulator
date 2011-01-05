
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

@interface EmulationItem : NSObject
{
	NSMutableArray *children;
	Document *document;
	
	NSString *uid;
	NSString *imagePath;
	NSString *label;
	
	NSString *location;
	NSString *state;
	BOOL isRemovable;
	
	BOOL isCanvas;
	
	BOOL isStorage;
	void *storageComponent;
	
	NSMutableArray *settingsRefs;
	NSMutableArray *settingsNames;
	NSMutableArray *settingsLabels;
	NSMutableArray *settingsTypes;
	NSMutableArray *settingsOptions;
	
	NSImage *image;
}

- (id)initWithDocument:(Document *)theDocument;
- (id)initWithLabel:(NSString *)theLabel;
- (id)initWithDeviceInfo:(void *)theDeviceInfo
			  inDocument:(Document *)theDocument;
- (id)initWithStorage:(void *)theComponent
		   deviceInfo:(void *)theDeviceInfo
		   inDocument:(Document *)theDocument;

- (NSMutableArray *)children;
- (EmulationItem *)childWithUid:(NSString *)theUid;

- (NSString *)uid;
- (NSImage *)image;
- (NSString *)label;

- (NSString *)location;
- (NSString *)state;
- (BOOL)isRemovable;

- (BOOL)isCanvas;

- (BOOL)isStorage;
- (BOOL)isStorageMounted;
- (NSString *)storagePath;
- (NSString *)storageFormat;
- (NSString *)storageCapacity;
- (BOOL)mount:(NSString *)path;
- (void)unmount;

- (NSInteger)numberOfSettings;
- (NSString *)labelForSettingAtIndex:(NSInteger)index;
- (NSString *)typeForSettingAtIndex:(NSInteger)index;
- (NSArray *)optionsForSettingAtIndex:(NSInteger)index;
- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;
- (NSString *)valueForSettingAtIndex:(NSInteger)index;

@end
