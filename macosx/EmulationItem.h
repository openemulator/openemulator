
/**
 * OpenEmulator
 * Mac OS X Emulation Item
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation item
 */

#import <Cocoa/Cocoa.h>

#import "Document.h"

typedef enum
{
    EMULATIONITEM_ROOT,
    EMULATIONITEM_GROUP,
    EMULATIONITEM_DEVICE,
    EMULATIONITEM_MOUNT,
    EMULATIONITEM_AVAILABLEPORT,
} EmulationItemType;

typedef enum
{
    EMULATIONDEVICEEVENT_POWERDOWN,
    EMULATIONDEVICEEVENT_SLEEP,
    EMULATIONDEVICEEVENT_WAKEUP,
    EMULATIONDEVICEEVENT_COLDRESTART,
    EMULATIONDEVICEEVENT_WARMRESTART,
    EMULATIONDEVICEEVENT_DEBUGGERBREAK,
} EmulationDeviceEvent;

@interface EmulationItem : NSObject
{
    EmulationItemType type;
    NSString *uid;
    NSMutableArray *children;
    Document *document;
    
    NSString *label;
    NSImage *image;
    
    NSString *locationLabel;
    NSString *stateLabel;
    
    void *device;
    
    NSMutableArray *settingsComponent;
    NSMutableArray *settingsName;
    NSMutableArray *settingsLabel;
    NSMutableArray *settingsType;
    NSMutableArray *settingsOptions;
    NSMutableArray *settingsOptionKeys;
    
    NSMutableArray *canvases;
    NSMutableArray *storages;
    
    NSString *portType;
    NSString *portId;
}

- (id)initRootWithDocument:(Document *)theDocument;
- (id)initGroup:(NSString *)theUID;
- (id)initDevice:(NSString *)theUID
       component:(void *)theComponent
        portType:(NSString *)thePortType
          portId:(NSString *)thePortId
        document:(Document *)theDocument;
- (id)initMount:(NSString *)theUID
      component:(void *)theComponent
  locationLabel:(NSString *)theLocationLabel
       document:(Document *)theDocument;
- (id)initPort:(NSString *)theUID
         label:(NSString *)theLabel
     imagePath:(NSString *)theImagePath
      portType:(NSString *)thePortType
      document:(Document *)theDocument;

- (void)initSettings:(void *)theSettings;

- (BOOL)isGroup;
- (NSString *)uid;
- (NSMutableArray *)children;

- (NSString *)label;
- (NSImage *)image;

- (NSString *)locationLabel;
- (NSString *)stateLabel;

- (void *)device;

- (NSInteger)numberOfSettings;
- (NSString *)labelForSettingAtIndex:(NSInteger)index;
- (NSString *)typeForSettingAtIndex:(NSInteger)index;
- (NSArray *)optionsForSettingAtIndex:(NSInteger)index;
- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;
- (NSString *)valueForSettingAtIndex:(NSInteger)index;

- (BOOL)isRemovable;
- (void)remove;

- (BOOL)hasCanvases;
- (void)showCanvases;

- (BOOL)hasStorages;
- (BOOL)mount:(NSString *)path;
- (BOOL)canMount:(NSString *)path;
- (BOOL)isMount;
- (void)showInFinder;
- (BOOL)isLocked;
- (BOOL)unmount;

- (BOOL)isPort;
- (BOOL)addOEDocument:(NSString *)path;
- (NSString *)portType;

@end
