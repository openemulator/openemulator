
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an chooser view controller for template and device choosing.
 */

#import "DeviceChooserViewController.h"

#import "OEParser.h"

#import "ChooserItem.h"
#import "Document.h"


@implementation DeviceChooserViewController

- (id) init
{
	self = [super init];
	
	if (self)
	{
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *templatesPath = [resourcePath
								   stringByAppendingPathComponent:@"devices"];
		[self loadDevicesFromPath:templatesPath];
	}
	
	return self;
}

- (void) loadDevicesFromPath:(NSString *) path
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *imagesPath = [resourcePath
							stringByAppendingPathComponent:@"images"];
	
	NSError *error;
	NSArray *deviceFilenames = [[NSFileManager defaultManager]
								contentsOfDirectoryAtPath:path
								error:&error];
	
	int deviceFilenamesCount = [deviceFilenames count];
	for (int i = 0; i < deviceFilenamesCount; i++)
	{
		NSString *deviceFilename = [deviceFilenames objectAtIndex:i];
		NSString *devicePath = [path stringByAppendingPathComponent:deviceFilename];
		string devicePathString = string([devicePath UTF8String]);
		OEParser parser(devicePathString);
		if (!parser.isOpen())
			continue;
		
		NSString *label = [deviceFilename stringByDeletingPathExtension];
		
		OEDMLInfo *dmlInfo = parser.getDMLInfo();
		NSString *imageName = [NSString stringWithUTF8String:dmlInfo->image.c_str()];
		NSString *description = [NSString stringWithUTF8String:dmlInfo->image.c_str()];
		NSString *groupName = [NSString stringWithUTF8String:dmlInfo->group.c_str()];
		
		// To-Do: store in array of OEDMLInfo
	}
	
	if (groupNames)
		[groupNames release];
	
	groupNames = [NSMutableArray arrayWithArray:[[groups allKeys]
												 sortedArrayUsingSelector:
												 @selector(compare:)]];
	[groupNames retain];
}

/*
 - (void) updateDevicesWithOutlets:(NSArray *)availableInletNames
					andOutletType:(NSString *)type
{
	if (![groups objectForKey:groupName])
	{
		NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
		[groups setObject:group forKey:groupName];
	}
	NSString *imagePath = [imagesPath stringByAppendingPathComponent:imageName];
	ChooserItem *item = [[ChooserItem alloc] initWithItem:templatePath
													label:label
												imagePath:imagePath
											  description:description];
	if (item)
		[item autorelease];
	
	[[groups objectForKey:groupName] addObject:item];
}*/

@end
