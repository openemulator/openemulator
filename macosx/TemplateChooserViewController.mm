
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser view.
 */

#import <string>

#import "TemplateChooserViewController.h"

#import "OEInfo.h"

#import "ChooserItem.h"
#import "Document.h"

#import "StringConversion.h"

@implementation TemplateChooserViewController

- (void)awakeFromNib
{
	[super awakeFromNib];
	
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *templatesPath = [resourcePath
							   stringByAppendingPathComponent:@"templates"];
	
	[self addTemplatesFromPath:templatesPath
				  setGroupName:nil];
	[groupNames addObjectsFromArray:[[groups allKeys]
									 sortedArrayUsingSelector:@selector(compare:)]];
}

- (void)updateUserTemplates
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

- (void)addTemplatesFromPath:(NSString *)path
				setGroupName:(NSString *)theGroupName
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	
	NSArray *templateFilenames = [[NSFileManager defaultManager]
								  contentsOfDirectoryAtPath:path
								  error:nil];
	
	int templateFilenamesCount = [templateFilenames count];
	for (int i = 0; i < templateFilenamesCount; i++)
	{
		NSString *templateFilename = [templateFilenames objectAtIndex:i];
		NSString *templatePath = [path stringByAppendingPathComponent:templateFilename];
		OEInfo info;
		info.open(getString(templatePath));
		if (info.isOpen())
		{
			NSString *groupName = getNSString(info.getType());
			NSString *label = getNSString(info.getLabel());
			NSString *imageName = getNSString(info.getImage());
			NSString *description = getNSString(info.getDescription());
			
			if (theGroupName)
				groupName = theGroupName;
			
			if (![groups objectForKey:groupName])
			{
				NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
				[groups setObject:group forKey:groupName];
			}
			
			NSString *imagePath = [resourcePath stringByAppendingPathComponent:imageName];
			ChooserItem *item = [[ChooserItem alloc] initWithTitle:label
														  subtitle:description
														 imagePath:imagePath
															  data:templatePath];
			if (item)
			{
				[item autorelease];
				[[groups objectForKey:groupName] addObject:item];
			}
		}
	}
}

@end
