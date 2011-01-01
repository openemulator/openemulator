
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

#import "OEEmulation.h"

@implementation EmulationItem

- (id)initWithDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		children = [[NSMutableArray alloc] init];
		
		OEDevicesInfo *devicesInfo = (OEDevicesInfo *)[theDocument devicesInfo];
		for (NSInteger i = 0; i < devicesInfo->size(); i++)
		{
			OEDeviceInfo &deviceInfo = devicesInfo->at(i);
			
			NSString *groupName = getNSString(deviceInfo.group);
			EmulationItem *groupItem = [self childWithUid:groupName];
			if (!groupItem)
			{
				groupItem = [[EmulationItem alloc] initWithLabel:groupName];
				[children addObject:groupItem];
				[groupItem release];
			}
			
			EmulationItem *deviceItem;
			deviceItem = [[EmulationItem alloc] initWithDeviceInfo:&deviceInfo
														inDocument:theDocument];
			[[groupItem children] addObject:deviceItem];
			[deviceItem release];
		}
	}
	
	return self;
}

- (id)initWithLabel:(NSString *)theLabel
{
	if (self = [super init])
	{
		uid = [NSLocalizedString(theLabel, @"Group label") copy];
		label = [[theLabel uppercaseString] retain];
		
		children = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (id)initWithDeviceInfo:(void *)theDeviceInfo
			  inDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		OEDeviceInfo *deviceInfo = (OEDeviceInfo *)theDeviceInfo;
		
		document = theDocument;
		
		uid = [getNSString(deviceInfo->id) retain];
		label = [getNSString(deviceInfo->label) retain];
		image = [[self getImage:getNSString(deviceInfo->image)] retain];
		location = [getNSString(deviceInfo->location) retain];
		state = [getNSString(deviceInfo->state) retain];
		showable = (deviceInfo->canvases.size() != 0);
		mountable = (deviceInfo->storages.size() != 0);
		
		settingsRefs = [[NSMutableArray alloc] init];
		settingsNames = [[NSMutableArray alloc] init];
		settingsLabels = [[NSMutableArray alloc] init];
		settingsTypes = [[NSMutableArray alloc] init];
		settingsOptions = [[NSMutableArray alloc] init];
		
		for (OESettings::iterator i = deviceInfo->settings.begin();
			 i != deviceInfo->settings.end();
			 i++)
		{
			NSString *settingRef = getNSString(i->ref);
			NSString *settingName = getNSString(i->name);
			NSString *settingLabel = getNSString(i->label);
			NSString *settingType = getNSString(i->type);
			NSArray *settingOptions = [getNSString(i->options)
									   componentsSeparatedByString:@","];
			
			[settingsRefs addObject:settingRef];
			[settingsNames addObject:settingName];
			[settingsLabels addObject:settingLabel];
			[settingsTypes addObject:settingType];
			[settingsOptions addObject:settingOptions];
		}
		
		children = [[NSMutableArray alloc] init];
		
		for (OEComponents::iterator i = deviceInfo->storages.begin();
			 i != deviceInfo->storages.end();
			 i++)
		{
			// To-Do: Recover mounted disk images
			EmulationItem *diskImageItem;
			NSString *path = @"Apple DOS 3.3.dsk";
			diskImageItem = [[EmulationItem alloc] initWithDiskImageAtPath:path
														  storageComponent:nil
																  location:location
																  readOnly:NO
																	locked:NO];
			[children addObject:diskImageItem];
			[diskImageItem release];
		}
	}
	
	return self;
}

- (id)initWithDiskImageAtPath:(NSString *)thePath
			 storageComponent:(void *)theStorageComponent
					 location:(NSString *)theLocation
					 readOnly:(BOOL)isReadOnly
					   locked:(BOOL)isLocked
{
	if (self = [super init])
	{
		uid = @"";
		label = [thePath copy];
		image = [[NSImage imageNamed:@"DiskImage"] retain];
		location = [theLocation copy];
		state = @"Mounted";
		mounted = YES;
		
		children = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void)dealloc
{
	[uid release];
	[label release];
	[image release];
	[location release];
	
	[settingsRefs release];
	[settingsNames release];
	[settingsLabels release];
	[settingsTypes release];
	[settingsOptions release];
	
	[children release];
	
	[super dealloc];
}

- (NSImage *)getImage:(NSString *)path
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	path = [resourcePath stringByAppendingPathComponent:path];
	
	return [[[NSImage alloc] initByReferencingFile:path] autorelease];
}

- (NSString *)uid
{
	return uid;
}

- (NSString *)label
{
	return label;
}

- (NSImage *)image
{
	return image;
}

- (NSString *)location
{
	return location;
}

- (NSString *)state
{
	return state;
}

- (BOOL)showable
{
	return showable;
}

- (BOOL)mountable
{
	return mountable;
}

- (BOOL)mounted
{
	return mounted;
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

- (NSString *)valueForSettingAtIndex:(NSInteger)index
{
	NSString *ref = [settingsRefs objectAtIndex:index];
	NSString *name = [settingsNames objectAtIndex:index];
	
	NSString *value = [document getValueOfProperty:name component:ref];
	
	NSString *type = [settingsTypes objectAtIndex:index];
	if ([type compare:@"select"] == NSOrderedSame)
	{
		NSArray *options = [settingsOptions objectAtIndex:index];
		value = [NSString stringWithFormat:@"%d", [options indexOfObject:value]];
	}
	
	return value;
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
	
	[document setValue:value ofProperty:name component:ref];
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

@end
