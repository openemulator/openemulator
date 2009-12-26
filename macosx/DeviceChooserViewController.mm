
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a device chooser view controller.
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
	[super dealloc];
	
	delete info;
	[path release];
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
	[super dealloc];
	
	[deviceInfos release];
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

- (void) updateForInlets:(NSArray *)inlets
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *imagesPath = [resourcePath
							stringByAppendingPathComponent:@"images"];
	
	[groups removeAllObjects];
	[groupNames removeAllObjects];
	
	int count = [deviceInfos count];
	for (int i = 0; i < count; i++)
	{
		DeviceInfo *deviceInfo = [deviceInfos objectAtIndex:i];
		OEInfo *info = [deviceInfo info];
		
		// 1: Copy available inlets
		// 2: For every unconnected outlet in the device, remove the inlet
		// 3: If the list ends up empty, we are ok
/*		NSArray *testInlets = [NSArray arrayWithArray:inlets];
		if (!testInlets)
			continue;
		
		OEPorts *outlets = info->getOutlets();
		for (OEPorts::iterator o = outlets.begin();
			 o != outlets.end();
			 o++)
		{
			if (
		}
*/		
		NSString *label = [NSString stringWithUTF8String:info->getLabel().c_str()];
		NSString *imageName = [NSString stringWithUTF8String:info->getImage().c_str()];
		NSString *description = [NSString stringWithUTF8String:info->getDescription().c_str()];
		NSString *groupName = [NSString stringWithUTF8String:info->getGroup().c_str()];
		
		if (![groups objectForKey:groupName])
		{
			NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
			[groups setObject:group forKey:groupName];
		}
		NSString *imagePath = [imagesPath stringByAppendingPathComponent:imageName];
		ChooserItem *item = [[ChooserItem alloc] initWithItem:[deviceInfo path]
														label:label
													imagePath:imagePath
												  description:description];
		if (!item)
			continue;
		
		[item autorelease];
		[[groups objectForKey:groupName] addObject:item];
	}
	
	[groupNames addObjectsFromArray:[[groups allKeys]
									 sortedArrayUsingSelector:@selector(compare:)]];
	
	[self selectItemWithPath:nil];
}

@end
