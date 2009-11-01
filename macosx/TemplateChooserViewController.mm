
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a template chooser view controller.
 */

#import "TemplateChooserViewController.h"

#import "OEParser.h"

#import "ChooserItem.h"
#import "Document.h"

@implementation TemplateChooserViewController

- (void) addTemplatesFromPath:(NSString *) path
				 setGroupName:(NSString *) theGroupName
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *imagesPath = [resourcePath
							stringByAppendingPathComponent:@"images"];
	
	NSArray *templateFilenames = [[NSFileManager defaultManager]
								  contentsOfDirectoryAtPath:path
								  error:nil];
	
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
		NSString *description = [NSString stringWithUTF8String:dmlInfo->description.c_str()];
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
}

- (void) updateUserTemplates
{
	NSString *userTemplatesGroupName = NSLocalizedString(@"My Templates",
														 @"My Templates");
	
	NSString *selectedItemPath = [self selectedItemPath];
	if ([groups objectForKey:userTemplatesGroupName])
	{
		[groups removeObjectForKey:userTemplatesGroupName];
		[groupNames removeLastObject];
	}
	
	NSString *userTemplatesPath = [TEMPLATE_FOLDER stringByExpandingTildeInPath];
	[self addTemplatesFromPath:userTemplatesPath
				  setGroupName:userTemplatesGroupName];
	
	if ([groups objectForKey:userTemplatesGroupName])
		[groupNames addObject:userTemplatesGroupName];
	[self selectItemWithPath:selectedItemPath];
}

- (void) awakeFromNib
{
	[super awakeFromNib];
	
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *templatesPath = [resourcePath
							   stringByAppendingPathComponent:@"templates"];
	
	[self addTemplatesFromPath:templatesPath
				  setGroupName:nil];
	[groupNames addObjectsFromArray:[[groups allKeys]
									 sortedArrayUsingSelector:@selector(compare:)]];
	
	[self updateUserTemplates];
}

@end
