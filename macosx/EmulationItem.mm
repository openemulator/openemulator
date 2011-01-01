
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

- (id)initWithDocument:(Document *)document
{
	if (self = [super init])
	{
		children = [[NSMutableArray alloc] init];
		
		OEDevicesInfo *devicesInfo = (OEDevicesInfo *)[document devicesInfo];
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
														inDocument:document];
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
			  inDocument:(Document *)document
{
	if (self = [super init])
	{
		OEDeviceInfo *deviceInfo = (OEDeviceInfo *)theDeviceInfo;
		
		uid = [getNSString(deviceInfo->id) retain];
		label = [getNSString(deviceInfo->label) retain];
		image = [[self getImage:getNSString(deviceInfo->image)] retain];
		location = [getNSString(deviceInfo->location) retain];
		state = [getNSString(deviceInfo->state) retain];
		showable = (deviceInfo->canvases.size() != 0);
		mountable = (deviceInfo->storages.size() != 0);
		
		settingsLabels = [[NSMutableArray alloc] init];
		settingsValues = [[NSMutableArray alloc] init];
		settingsTypes = [[NSMutableArray alloc] init];
		settingsOptions = [[NSMutableArray alloc] init];
		
		for (OESettings::iterator i = deviceInfo->settings.begin();
			 i != deviceInfo->settings.end();
			 i++)
		{
			NSString *settingLabel = getNSString(i->label);
			NSString *settingValue = [document getValue:getNSString(i->name)
										   forComponent:getNSString(i->ref)];
			NSString *settingType = getNSString(i->type);
			NSArray *settingOptions = [getNSString(i->options)
									   componentsSeparatedByString:@","];
			
			[settingsLabels addObject:settingLabel];
			[settingsValues addObject:settingValue];
			[settingsTypes addObject:settingType];
			[settingsOptions addObject:settingOptions];
		}
		
		children = [[NSMutableArray alloc] init];
		
		for (OEComponents::iterator i = deviceInfo->storages.begin();
			 i != deviceInfo->storages.end();
			 i++)
		{
			
		}
		
		if ([uid compare:@"s6d1"] == NSOrderedSame)
		{
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
	
	[settingsLabels release];
	[settingsValues release];
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
	return [settingsValues count];
}

- (NSString *)labelForSettingAtIndex:(NSInteger)index
{
	return [settingsLabels objectAtIndex:index];
}

- (NSString *)valueForSettingAtIndex:(NSInteger)index
{
	return [settingsValues objectAtIndex:index];
}

- (NSString *)typeForSettingAtIndex:(NSInteger)index
{
	return [settingsTypes objectAtIndex:index];
}

- (NSArray *)optionsForSettingAtIndex:(NSInteger)index
{
	return [settingsOptions objectAtIndex:index];
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
