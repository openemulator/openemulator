
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
	
	void *component;
	NSString *label;
	NSImage *image;
}

- (id)initWithDocument:(Document *)theDocument;
- (id)initWithGroupName:(NSString *)theGroupName;
- (id)initWithDevice:(void *)theDevice
			document:(Document *)theDocument;
- (id)initWithStorage:(void *)theStorage
			 location:(NSString *)theLocation
			 document:(Document *)theDocument;

- (EmulationItemType)type;
- (NSMutableArray *)children;
- (EmulationItem *)childWithUid:(NSString *)theUid;

- (void *)component;
- (NSString *)label;
- (NSImage *)image;

- (NSString *)location;
- (NSString *)state;
- (BOOL)isRemovable;

- (BOOL)isCanvas;
- (void)showCanvases;

- (BOOL)isStorage;
- (BOOL)isMounted;
- (BOOL)isLocked;
- (NSString *)diskImagePath;
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
