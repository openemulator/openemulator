
/**
 * OpenEmulator
 * Mac OS X Emulation Item
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation item.
 */

#import "EmulationItem.h"
#import "StringConversion.h"

#import "Emulation.h"

@implementation EmulationItem

- (id)initWithDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_ROOT;
		children = [[NSMutableArray alloc] init];
		
		EmulationInfo *emulationInfo = (EmulationInfo *)[theDocument emulationInfo];
		for (NSInteger i = 0; i < emulationInfo->size(); i++)
		{
			EmulationDeviceInfo &deviceInfo = emulationInfo->at(i);
			
			// Create group item
			NSString *groupName = getNSString(deviceInfo.group);
			EmulationItem *groupItem = [self childWithUid:groupName];
			if (!groupItem)
			{
				groupItem = [[EmulationItem alloc] initWithGroupName:groupName];
				[children addObject:groupItem];
				[groupItem release];
			}
			
			// Create device item
			EmulationItem *deviceItem;
			deviceItem = [[EmulationItem alloc] initWithDeviceInfo:&deviceInfo
														  document:theDocument];
			[[groupItem children] addObject:deviceItem];
			[deviceItem release];
		}
	}
	
	return self;
}

- (id)initWithGroupName:(NSString *)theGroupName
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_GROUP;
		children = [[NSMutableArray alloc] init];
		
		uid = [theGroupName copy];
		label = [[NSLocalizedString(theGroupName, @"Emulation Item Label.")
				  uppercaseString] retain];
	}
	
	return self;
}

- (id)initWithDeviceInfo:(void *)theDeviceInfo
				document:(Document *)theDocument
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_DEVICE;
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		EmulationDeviceInfo *deviceInfo = (EmulationDeviceInfo *)theDeviceInfo;
		
		uid = [getNSString(deviceInfo->id) retain];
		label = [getNSString(deviceInfo->label) retain];
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *imagePath = [[resourcePath stringByAppendingPathComponent:
								getNSString(deviceInfo->image)] retain];
		image = [[NSImage alloc] initByReferencingFile:imagePath];
		
		settingsRefs = [[NSMutableArray alloc] init];
		settingsNames = [[NSMutableArray alloc] init];
		settingsLabels = [[NSMutableArray alloc] init];
		settingsTypes = [[NSMutableArray alloc] init];
		settingsOptions = [[NSMutableArray alloc] init];
		EmulationSettings &settings = deviceInfo->settings;
		for (NSInteger i = 0; i < settings.size(); i++)
		{
			EmulationSetting &setting = settings.at(i);
			[settingsRefs addObject:getNSString(setting.ref)];
			[settingsNames addObject:getNSString(setting.name)];
			[settingsLabels addObject:getNSString(setting.label)];
			[settingsTypes addObject:getNSString(setting.type)];
			[settingsOptions addObject:[getNSString(setting.options)
										componentsSeparatedByString:@","]];
		}

		location = [getNSString(deviceInfo->location) retain];
		
		state = [getNSString(deviceInfo->state) retain];
		canvases = [[NSMutableArray alloc] init];
		OEComponents &theCanvases = deviceInfo->canvases;
		for (NSInteger i = 0; i < theCanvases.size(); i++)
			[canvases addObject:[NSValue valueWithPointer:theCanvases.at(i)]];
		
		storage = deviceInfo->storage;
		if (storage && [document isStorageMounted:storage])
		{
			NSString *theUID;
			
			theUID = [NSString stringWithFormat:@"%@.storage", uid];
			
			EmulationItem *storageItem;
			storageItem = [[EmulationItem alloc] initWithStorage:storage
															 uid:theUID 
														location:location
														document:theDocument];
			[children addObject:storageItem];
			[storageItem release];
		}
	}
	
	return self;
}

- (id)initWithStorage:(void *)theComponent
				  uid:(NSString *)theUid
			 location:(NSString *)theLocation
			 document:(Document *)theDocument
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_DISKIMAGE;
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		uid = [theUid copy];
		label = [[[document imagePathForStorage:theComponent]
				  lastPathComponent] retain];
		image = [[NSImage imageNamed:@"DiskImage"] retain];
		
		location = [theLocation copy];
		storage = theComponent;
	}
	
	return self;
}

- (void)dealloc
{
	[children release];
	
	[uid release];
	[image release];
	[label release];
	
	[location release];
	[state release];
	
	[canvases release];
	
	[settingsRefs release];
	[settingsNames release];
	[settingsLabels release];
	[settingsTypes release];
	[settingsOptions release];
	
	[super dealloc];
}

- (EmulationItemType)type
{
	return itemType;
}

- (NSMutableArray *)children
{
	return children;
}

- (EmulationItem *)childWithUid:(NSString *)theUid
{
	for (EmulationItem *item in children)
	{
		if ([[item uid] compare:theUid] == NSOrderedSame)
			return item;
	}
	
	return nil;
}

- (NSString *)uid
{
	return uid;
}

- (NSImage *)image
{
	return image;
}

- (NSString *)label
{
	return label;
}

- (NSString *)location
{
	return location;
}

- (NSString *)state
{
	if (itemType == EMULATION_ITEM_DISKIMAGE)
	{
		NSString *theState;
		theState = ([document isStorageWritable:storage] ?
					NSLocalizedString(@"Read/Write", @"Emulation Item.") : 
					NSLocalizedString(@"Read-Only", @"Emulation Item."));
		if ([document isStorageLocked:storage])
			theState = [theState stringByAppendingString:
						NSLocalizedString(@", Locked", @"Emulation Item.")];
		
		return theState;
	}
	
	return state;
}

- (BOOL)isRemovable
{
	return (itemType == EMULATION_ITEM_DEVICE) && [location length];
}



- (BOOL)isCanvas
{
	return ([canvases count] != 0);
}

- (void)showCanvases
{
	for (NSInteger i = 0; i < [canvases count]; i++)
		[document showCanvas:[[canvases objectAtIndex:i] pointerValue]];
}



- (BOOL)isStorage
{
	return (((itemType == EMULATION_ITEM_DEVICE) && storage) ||
			![location length]);
}

- (BOOL)isMounted
{
	return (itemType == EMULATION_ITEM_DISKIMAGE);
}

- (BOOL)isLocked
{
	return [document isStorageLocked:storage];
}

- (NSString *)diskImagePath
{
	return [document imagePathForStorage:storage];
}

- (NSString *)diskImageFormat
{
	return [document imageFormatForStorage:storage];
}

- (NSString *)diskImageCapacity
{
	return [document imageCapacityForStorage:storage];
}

- (BOOL)mount:(NSString *)path
{
	if (![location length])
		return [document mount:path];
	
	return [document mount:path inStorage:storage];
}

- (void)unmount
{
	[document unmountStorage:storage];
}

- (BOOL)canMount:(NSString *)path
{
	if (![location length])
		return [document canMount:path];
	
	return [document canMount:path inStorage:storage];
}



- (NSInteger)numberOfSettings
{
	return [settingsRefs count];
}

- (NSString *)labelForSettingAtIndex:(NSInteger)index
{
	return [settingsLabels objectAtIndex:index];
}

- (NSString *)typeForSettingAtIndex:(NSInteger)index
{
	return [settingsTypes objectAtIndex:index];
}

- (NSArray *)optionsForSettingAtIndex:(NSInteger)index
{
	return [settingsOptions objectAtIndex:index];
}

- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;
{
	NSString *ref = [settingsRefs objectAtIndex:index];
	NSString *name = [settingsNames objectAtIndex:index];
	
	NSString *type = [settingsTypes objectAtIndex:index];
	if ([type compare:@"select"] == NSOrderedSame)
	{
		NSArray *options = [settingsOptions objectAtIndex:index];
		value = [options objectAtIndex:[value integerValue]];
	}
	
	[document setValue:value ofProperty:name forComponent:ref];
}

- (NSString *)valueForSettingAtIndex:(NSInteger)index
{
	NSString *ref = [settingsRefs objectAtIndex:index];
	NSString *name = [settingsNames objectAtIndex:index];
	
	NSString *value = [document valueOfProperty:name forComponent:ref];
	
	NSString *type = [settingsTypes objectAtIndex:index];
	if ([type compare:@"select"] == NSOrderedSame)
	{
		NSArray *options = [settingsOptions objectAtIndex:index];
		value = [NSString stringWithFormat:@"%d", [options indexOfObject:value]];
	}
	
	return value;
}

@end
