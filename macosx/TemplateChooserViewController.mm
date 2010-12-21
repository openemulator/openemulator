
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser view.
 */

#import "TemplateChooserViewController.h"
#import "Document.h"
#import "ChooserItem.h"
#import "StringConversion.h"

#import "OEEDL.h"

#define MY_TEMPLATES @"My Templates"

@implementation TemplateChooserViewController

- (void)loadGroups
{
	NSString *path = [[[NSBundle mainBundle] resourcePath]
					  stringByAppendingPathComponent:@"templates"];
	
	[groups removeAllObjects];
	
	// Find application templates
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSArray *subpaths = [fileManager contentsOfDirectoryAtPath:path
														 error:nil];
	for (int i = 0; i < [subpaths count]; i++)
	{
		NSString *groupName = [subpaths objectAtIndex:i];
		NSString *groupPath = [path stringByAppendingPathComponent:groupName];
		if ([self validateGroupAtPath:groupPath])
			[groups addObject:groupName];
	}
	
	// Sort alphabetically
	[groups setArray:[groups sortedArrayUsingSelector:@selector(compare:)]];
	
	// Find my templates
	[items removeObjectForKey:NSLocalizedString(MY_TEMPLATES, MY_TEMPLATES)];
	if ([self validateGroupAtPath:MY_TEMPLATES_FOLDER])
		[groups addObject:NSLocalizedString(MY_TEMPLATES, MY_TEMPLATES)];
}

- (void)loadItems
{
	if ([items objectForKey:selectedGroup])
		return;
	
	NSString *group = NSLocalizedString(MY_TEMPLATES, MY_TEMPLATES);
	if ([selectedGroup compare:group] == NSOrderedSame)
		[self addGroupAtPath:MY_TEMPLATES_FOLDER
					 toGroup:group];
	else
	{
		NSString *templatesPath = [[[NSBundle mainBundle] resourcePath]
								   stringByAppendingPathComponent:@"templates"];
		NSString *groupPath = [templatesPath stringByAppendingPathComponent:selectedGroup];
		[self addGroupAtPath:groupPath
					 toGroup:selectedGroup];
	}
}

- (BOOL)validateGroupAtPath:(NSString *)groupPath
{
	groupPath = [groupPath stringByExpandingTildeInPath];
	NSFileManager *fileManager = [NSFileManager defaultManager];
	BOOL isDirectory;
	if (![fileManager fileExistsAtPath:groupPath isDirectory:&isDirectory] ||
		!isDirectory)
		return NO;
	
	NSArray *subpaths = [fileManager contentsOfDirectoryAtPath:groupPath
														 error:nil];
	for (int i = 0; i < [subpaths count]; i++)
	{
		NSString *path = [subpaths objectAtIndex:i];
		NSString *pathExtension = [[path pathExtension] lowercaseString];
		
		if (([pathExtension compare:@OE_PACKAGE_PATH_EXTENSION] == NSOrderedSame) ||
			([pathExtension compare:@OE_FILE_PATH_EXTENSION] == NSOrderedSame))
			return YES;
	}
	
	return NO;
}

- (void)addGroupAtPath:(NSString *)path
			   toGroup:(NSString *)group
{
	if (![items objectForKey:group])
		[items setObject:[NSMutableArray array] forKey:group];
	
	path = [path stringByExpandingTildeInPath];
	
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSArray *pathContents = [[NSFileManager defaultManager]
							 contentsOfDirectoryAtPath:path
							 error:nil];
	for (int i = 0; i < [pathContents count]; i++)
	{
		NSString *edlFilename = [pathContents objectAtIndex:i];
		NSString *edlPath = [path stringByAppendingPathComponent:edlFilename];
		
		NSString *pathExtension = [[edlPath pathExtension] lowercaseString];
		if (([pathExtension compare:@OE_PACKAGE_PATH_EXTENSION] != NSOrderedSame) &&
			([pathExtension compare:@OE_FILE_PATH_EXTENSION] != NSOrderedSame))
			continue;
		
		OEEDL edl;
		edl.open(getCPPString(edlPath));
		if (edl.isOpen())
		{
			OEHeaderInfo headerInfo = edl.getHeaderInfo();
			NSString *edlLabel = [edlFilename stringByDeletingPathExtension];
			NSString *edlImage = [resourcePath stringByAppendingPathComponent:
								  getNSString(headerInfo.image)];
			NSString *edlDescription = getNSString(headerInfo.description);
			
			ChooserItem *chooserItem = [[ChooserItem alloc] initWithLabel:edlLabel
																imagePath:edlImage
															  description:edlDescription
																  edlPath:edlPath
																	 data:nil];
			if (chooserItem)
			{
				[[items objectForKey:group] addObject:chooserItem];
				[chooserItem release];
			}
		}
	}
}

@end
