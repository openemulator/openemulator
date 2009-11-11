
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a device chooser view controller.
 */

#import "DeviceChooserViewController.h"

#import "OEParser.h"

#import "ChooserItem.h"
#import "Document.h"

@interface DeviceInfo : NSObject
{
	OEDMLInfo dmlInfo;
	NSString *path;
}

- (id) initWithDMLInfo:(OEDMLInfo *) info
				atPath:(NSString *) path;
- (OEDMLInfo *) dmlInfo;
- (NSString *) path;

@end

@implementation DeviceInfo

- initWithDMLInfo:(OEDMLInfo *) info
		   atPath:(NSString *) thePath;
{
	if (self = [super init])
	{
		dmlInfo = *info;
		path = [thePath copy];
	}
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
	
	[path release];
}

- (OEDMLInfo *) dmlInfo
{
	return &dmlInfo;
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
		OEParser parser(string([devicePath UTF8String]));
		if (!parser.isOpen())
			continue;
		
		OEDMLInfo *dmlInfo = parser.getDMLInfo();
		DeviceInfo *deviceInfo = [[DeviceInfo alloc] initWithDMLInfo:dmlInfo
															  atPath:devicePath];
		[deviceInfos addObject:deviceInfo];
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
		
		OEDMLInfo *dmlInfo = [deviceInfo dmlInfo];
		NSString *label = [NSString stringWithUTF8String:dmlInfo->label.c_str()];
		NSString *imageName = [NSString stringWithUTF8String:dmlInfo->image.c_str()];
		NSString *description = [NSString stringWithUTF8String:dmlInfo->description.c_str()];
		NSString *groupName = [NSString stringWithUTF8String:dmlInfo->group.c_str()];
		
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
