
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

typedef enum
{
	EMULATION_ITEM_ROOT,
	EMULATION_ITEM_GROUP,
	EMULATION_ITEM_DEVICE,
	EMULATION_ITEM_DISKIMAGE,
} EmulationItemType;

@interface EmulationItem : NSObject
{
	EmulationItemType itemType;
	NSMutableArray *children;
	Document *document;
	
	NSString *uid;
	NSImage *image;
	NSString *label;
	
	NSString *location;
	NSString *state;
	
	NSMutableArray *canvasComponents;
	
	NSMutableArray *storageComponents;
	
	NSMutableArray *settingsRefs;
	NSMutableArray *settingsNames;
	NSMutableArray *settingsLabels;
	NSMutableArray *settingsTypes;
	NSMutableArray *settingsOptions;
}

- (id)initWithDocument:(Document *)theDocument;
- (id)initWithGroupName:(NSString *)theGroupName;
- (id)initWithDeviceInfo:(void *)theDeviceInfo
				document:(Document *)theDocument;
- (id)initWithStorage:(void *)theComponent
				  uid:(NSString *)theUID
			 location:(NSString *)theLocation
			 document:(Document *)theDocument;

- (EmulationItemType)type;
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
- (BOOL)isMounted;
- (BOOL)isLocked;
- (NSString *)diskImagePath;
- (NSString *)diskImageFormat;
- (NSString *)diskImageCapacity;
- (BOOL)mount:(NSString *)path;
- (void)unmount;
- (BOOL)canMount:(NSString *)path;

- (NSInteger)numberOfSettings;
- (NSString *)labelForSettingAtIndex:(NSInteger)index;
- (NSString *)typeForSettingAtIndex:(NSInteger)index;
- (NSArray *)optionsForSettingAtIndex:(NSInteger)index;
- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;
- (NSString *)valueForSettingAtIndex:(NSInteger)index;

@end
