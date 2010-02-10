
/**
 * OpenEmulator
 * Mac OS X Device Chooser View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device chooser view.
 */

#import "DeviceChooserViewController.h"

#import "OEInfo.h"

#import "ChooserItem.h"
#import "Document.h"

@interface DeviceInfo : NSObject
{
	OEInfo *info;
	NSString *path;
}

- (id) initWithPath:(NSString *) path;
- (OEInfo *) info;
- (NSString *) path;

@end

@implementation DeviceInfo

- initWithPath:(NSString *) thePath;
{
	if (self = [super init])
	{
		info = new OEInfo(string([thePath UTF8String]));
		path = [thePath copy];
	}
	
	return self;
}

- (void) dealloc
{
	delete info;
	[path release];

	[super dealloc];
}

- (OEInfo *) info
{
	return info;
}

- (NSString *) path
{
	return path;
}

@end

@implementation DeviceChooserViewController

- (id) init
{
	self = [super init];
	
	if (self)
		deviceInfos = [[NSMutableArray alloc] init];
	
	return self;
}

- (void) dealloc
{
	[deviceInfos release];

	[super dealloc];
}

- (void) awakeFromNib
{
	[super awakeFromNib];
	
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *templatesPath = [resourcePath
							   stringByAppendingPathComponent:@"devices"];
	NSArray *deviceFilenames = [[NSFileManager defaultManager]
								contentsOfDirectoryAtPath:templatesPath
								error:nil];
	
	int deviceFilenamesCount = [deviceFilenames count];
	for (int i = 0; i < deviceFilenamesCount; i++)
	{
		NSString *deviceFilename = [deviceFilenames objectAtIndex:i];
		NSString *devicePath = [templatesPath
								stringByAppendingPathComponent:deviceFilename];
		DeviceInfo *deviceInfo = [[DeviceInfo alloc] initWithPath:devicePath];
		if (deviceInfo)
		{
			[deviceInfo autorelease];
			[deviceInfos addObject:deviceInfo];
		}
	}
}

- (void) updateWithInlets:(NSArray *) freeInlets
			  andCategory:(NSString *) category
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *imagesPath = [resourcePath
							stringByAppendingPathComponent:@"images"];
	
	[groups removeAllObjects];
	[groupNames removeAllObjects];
	
	for (int i = 0; i < [deviceInfos count]; i++)
	{
		DeviceInfo *deviceInfo = [deviceInfos objectAtIndex:i];
		OEInfo *info = [deviceInfo info];
		
		// Check if device's outlets match emulation inlets
		NSMutableArray *inlets = [NSMutableArray arrayWithArray:freeInlets];
		OEPorts *outlets = info->getOutlets();
		BOOL isInletsFound = YES;
		BOOL isCategoryFound = NO;
		for (OEPorts::iterator o = outlets->begin();
			 o != outlets->end();
			 o++)
		{
			if (o->connectionPort)
				continue;
			
			NSString *outletType = [NSString stringWithUTF8String:o->type.c_str()];
			BOOL isInletFound = NO;
			for (int j = 0; j < [inlets count]; j++)
			{
				NSMutableDictionary *dict = [inlets objectAtIndex:j];
				NSString *inletType = [dict objectForKey:@"type"];
				
				if ([inletType compare:outletType] == NSOrderedSame)
				{
					[inlets removeObjectAtIndex:j];
					isInletFound = YES;
					
					NSString *inletCategory = [dict objectForKey:@"category"];
					if ([inletCategory compare:category] == NSOrderedSame)
						isCategoryFound = YES;
				}
			}
			
			if (!isInletFound)
			{
				isInletsFound = NO;
				break;
			}
		}
		
		if (!isInletsFound)
			continue;
		if (!isCategoryFound)
			continue;
		
		// Add device
		NSString *label = [NSString stringWithUTF8String:info->getLabel().c_str()];
		NSString *imageName = [NSString stringWithUTF8String:info->getImage().c_str()];
		NSString *description = [NSString stringWithUTF8String:info->getDescription().
								 c_str()];
		NSString *groupName = [NSString stringWithUTF8String:info->getGroup().c_str()];
		
		if (![groups objectForKey:groupName])
		{
			NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
			[groups setObject:group forKey:groupName];
		}
		NSString *imagePath = [imagesPath stringByAppendingPathComponent:imageName];
		ChooserItem *item = [[ChooserItem alloc] initWithTitle:label
													  subtitle:description
													 imagePath:imagePath
														  data:[deviceInfo path]];
		if (!item)
			continue;
		
		[item autorelease];
		[[groups objectForKey:groupName] addObject:item];
	}
	
	[groupNames addObjectsFromArray:[[groups allKeys]
									 sortedArrayUsingSelector:@selector(compare:)]];
	
	[self selectItemWithPath:nil];
}

- (NSMutableArray *) selectedItemOutlets
{
	// Find selected device
	NSString *itemPath = [self selectedItemPath];
	if (!itemPath)
		return NULL;
	
	OEInfo *info = NULL;
	for (int i = 0; i < [deviceInfos count]; i++)
	{
		DeviceInfo *deviceInfo = [deviceInfos objectAtIndex:i];
		if ([[deviceInfo path] compare:itemPath] == NSOrderedSame)
		{
			info = [deviceInfo info];
			break;
		}
	}
	
	if (!info)
		return NULL;
	
	// List unconnected outlets
	NSMutableArray *freeOutlets = [[NSMutableArray alloc] init];
	if (!freeOutlets)
		return NULL;
	[freeOutlets autorelease];
	
	OEPorts *outlets = info->getOutlets();
	for (OEPorts::iterator o = outlets->begin();
		 o != outlets->end();
		 o++)
	{
		if (o->connectionPort)
			continue;
		
		string stringRef = o->ref.getStringRef();
		
		NSString *portType = [NSString stringWithUTF8String:o->type.c_str()];
		NSString *portLabel = [NSString stringWithUTF8String:o->label.c_str()];
		NSString *portImage = [NSString stringWithUTF8String:o->image.c_str()];
		NSString *portRef = [NSString stringWithUTF8String:stringRef.c_str()];
		
		NSMutableDictionary *dict = [[[NSMutableDictionary alloc] init] autorelease];
		[dict setObject:portType forKey:@"type"];
		[dict setObject:portLabel forKey:@"label"];
		[dict setObject:portImage forKey:@"image"];
		[dict setObject:portRef forKey:@"ref"];
		
		[freeOutlets addObject:dict];
	}
	
	return freeOutlets;
}

@end
