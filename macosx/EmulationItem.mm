
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
		
		location = [getNSString(deviceInfo->location) retain];
		state = [getNSString(deviceInfo->state) retain];
		
		canvasComponents = [[NSMutableArray alloc] init];
		OEComponents &canvases = deviceInfo->canvases;
		for (int i = 0; i < canvases.size(); i++)
			[canvasComponents addObject:[NSValue valueWithPointer:canvases.at(i)]];
		
		storageComponents = [[NSMutableArray alloc] init];
		OEComponents &storages = deviceInfo->storages;
		for (int i = 0; i < storages.size(); i++)
		{
			OEComponent *storage = storages.at(i);
			[storageComponents addObject:[NSValue valueWithPointer:storage]];
			
			if ([document isStorageMounted:storage])
			{
				NSString *theUID;
				
				theUID = [NSString stringWithFormat:@"%@.%d", uid, i];
				
				EmulationItem *storageItem;
				storageItem = [[EmulationItem alloc] initWithStorage:storage
																 uid:theUID 
															location:location
															document:theDocument];
				[children addObject:storageItem];
				[storageItem release];
			}
		}
		
		settingsRefs = [[NSMutableArray alloc] init];
		settingsNames = [[NSMutableArray alloc] init];
		settingsLabels = [[NSMutableArray alloc] init];
		settingsTypes = [[NSMutableArray alloc] init];
		settingsOptions = [[NSMutableArray alloc] init];
		EmulationSettings &settings = deviceInfo->settings;
		for (int i = 0; i < settings.size(); i++)
		{
			EmulationSetting &setting = settings.at(i);
			[settingsRefs addObject:getNSString(setting.ref)];
			[settingsNames addObject:getNSString(setting.name)];
			[settingsLabels addObject:getNSString(setting.label)];
			[settingsTypes addObject:getNSString(setting.type)];
			[settingsOptions addObject:[getNSString(setting.options)
										componentsSeparatedByString:@","]];
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
		storageComponents = [[NSMutableArray alloc] initWithObjects:
							 [NSValue valueWithPointer:theComponent], 
							 nil];
		
		itemType = EMULATION_ITEM_STORAGE;
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		uid = [theUid copy];
		label = [[[self diskImagePath] lastPathComponent] retain];
		image = [[NSImage imageNamed:@"DiskImage"] retain];
		
		location = [theLocation copy];
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
	
	[canvasComponents release];
	
	[storageComponents release];
	
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
	if (itemType == EMULATION_ITEM_STORAGE)
	{
		NSString *theState;
		void *theComponent = [[storageComponents objectAtIndex:0] pointerValue];
		
		theState = ([document isStorageWritable:theComponent] ?
					NSLocalizedString(@"Read/Write", @"Emulation Item.") : 
					NSLocalizedString(@"Read-Only", @"Emulation Item."));
		if ([document isStorageLocked:theComponent])
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
	return ([canvasComponents count] != 0);
}



- (BOOL)isStorage
{
	return ([storageComponents count] != 0) || ![location length];
}

- (BOOL)isMounted
{
	return (itemType == EMULATION_ITEM_STORAGE);
}

- (BOOL)isLocked
{
	if (![storageComponents count])
		return NO;
	
	for (int i = 0; i < [storageComponents count]; i++)
	{
		void *component = [[storageComponents objectAtIndex:i] pointerValue];
		if (![document isStorageLocked:component])
			return NO;
	}
	
	return YES;
}

- (NSString *)diskImagePath
{
	if (![storageComponents count])
		return @"";
	
	void *component = [[storageComponents objectAtIndex:0] pointerValue];
	return [document imagePathForStorage:component];
}

- (NSString *)diskImageFormat
{
	if (![storageComponents count])
		return @"";
	
	void *component = [[storageComponents objectAtIndex:0] pointerValue];
	return [document imageFormatForStorage:component];
}

- (NSString *)diskImageCapacity
{
	if (![storageComponents count])
		return @"";
	
	void *component = [[storageComponents objectAtIndex:0] pointerValue];
	return [document imageCapacityForStorage:component];
}

- (BOOL)mount:(NSString *)path
{
	if (![location length])
		return [document mount:path];
	
	for (int i = 0; i < [storageComponents count]; i++)
	{
		void *component = [[storageComponents objectAtIndex:i] pointerValue];
		
		if ([document mount:path inStorage:component])
			return YES;
	}
	
	return NO;
}

- (void)unmount
{
	void *component = [[storageComponents objectAtIndex:0] pointerValue];
	[document unmountStorage:component];
}

- (BOOL)canMount:(NSString *)path
{
	if (![location length])
		return [document canMount:path];
	
	for (int i = 0; i < [storageComponents count]; i++)
	{
		void *component = [[storageComponents objectAtIndex:i] pointerValue];
		
		if ([document canMount:path inStorage:component])
			return YES;
	}
	
	return NO;
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
