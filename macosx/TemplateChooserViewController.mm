
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an chooser view controller for template and device choosing.
 */

#import "TemplateChooserViewController.h"

#import "OEParser.h"

#import "ChooserItem.h"
#import "Document.h"

@implementation TemplateChooserViewController

- (id) init
{
	self = [super init];
	
	if (self)
	{
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *templatesPath = [resourcePath
								   stringByAppendingPathComponent:@"templates"];
		[self addTemplatesFromPath:templatesPath
					  setGroupName:nil];
		
		[self updateUserTemplates];
	}
	
	return self;
}

- (void) addTemplatesFromPath:(NSString *) path
				 setGroupName:(NSString *) theGroupName
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *imagesPath = [resourcePath
							stringByAppendingPathComponent:@"images"];
	
	NSError *error;
	NSArray *templateFilenames = [[NSFileManager defaultManager]
								  contentsOfDirectoryAtPath:path
								  error:&error];
	
	int templateFilenamesCount = [templateFilenames count];
	for (int i = 0; i < templateFilenamesCount; i++)
	{
		NSString *templateFilename = [templateFilenames objectAtIndex:i];
		NSString *templatePath = [path stringByAppendingPathComponent:templateFilename];
		string templatePathString = string([templatePath UTF8String]);
		OEParser parser(templatePathString);
		if (!parser.isOpen())
			continue;
		
		NSString *label = [templateFilename stringByDeletingPathExtension];
		
		OEDMLInfo *dmlInfo = parser.getDMLInfo();
		NSString *imageName = [NSString stringWithUTF8String:dmlInfo->image.c_str()];
		NSString *description = [NSString stringWithUTF8String:dmlInfo->image.c_str()];
		NSString *groupName = [NSString stringWithUTF8String:dmlInfo->group.c_str()];
		
		if (theGroupName)
			groupName = theGroupName;
		
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
	}
	
	if (groupNames)
		[groupNames release];
	
	groupNames = [NSMutableArray arrayWithArray:[[groups allKeys]
												 sortedArrayUsingSelector:
												 @selector(compare:)]];
	[groupNames retain];
}

- (void) updateUserTemplates
{
	NSString *selectedItemPath = [self selectedItemPath];
	
	NSString *userTemplatesGroupName = NSLocalizedString(@"My Templates",
														 @"My Templates");
	NSMutableArray *userTemplatesArray = [groups objectForKey:userTemplatesGroupName];
	if (userTemplatesArray)
		[groups removeObjectForKey:userTemplatesGroupName];
	
	NSString *userTemplatesPath = [TEMPLATE_FOLDER stringByExpandingTildeInPath];
	[self addTemplatesFromPath:userTemplatesPath
				  setGroupName:userTemplatesGroupName];
	
	[self selectItemWithItemPath:selectedItemPath];
}

@end
