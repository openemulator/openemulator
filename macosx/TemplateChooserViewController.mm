
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

- (void)awakeFromNib
{
	[super awakeFromNib];
	
	NSString *templatesPath = [[[NSBundle mainBundle] resourcePath]
							   stringByAppendingPathComponent:@"templates"];
	
	NSArray *templatesSubpaths = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:templatesPath
																					   error:nil];
	for (int i = 0; i < [templatesSubpaths count]; i++)
	{
		NSString *pathComponent = [templatesSubpaths objectAtIndex:i];
		NSString *groupPath = [templatesPath stringByAppendingPathComponent:pathComponent];
		NSArray *groupSubpaths = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:groupPath
																					 error:nil];
		if ([groupSubpaths count])
			[groupNames addObject:pathComponent];
	}
	
	[groupNames addObjectsFromArray:[groupNames sortedArrayUsingSelector:@selector(compare:)]];
}

- (void)updateUserTemplates
{
	NSString *selectedItemPath = [self selectedItemPath];
	
	NSString *group = NSLocalizedString(MY_TEMPLATES, MY_TEMPLATES);
	[self removeGroup:group];
	if (![groupNames containsObject:group])
		[groupNames addObject:group];
	
	[self selectItemWithPath:selectedItemPath];
}

- (void)populateGroup:(NSString *)selectedGroup
{
	if ([groups objectForKey:selectedGroup] == nil)
		return;
	
	NSString *group = NSLocalizedString(MY_TEMPLATES, MY_TEMPLATES);
//	if ([selectedGroup compare:group] == NSOrderedSame)
//		[self addTemplatesFromPath:a
//						   toGroup:b];
}

- (void)addTemplatesFromPath:(NSString *)path
					 toGroup:(NSString *)group
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	
	NSArray *pathContents = [[NSFileManager defaultManager]
							 contentsOfDirectoryAtPath:path
							 error:nil];
	for (int i = 0; i < [pathContents count]; i++)
	{
		NSString *edlFilename = [pathContents objectAtIndex:i];
		NSString *edlPath = [path stringByAppendingPathComponent:edlFilename];
		
		OEEDL edl;
		edl.open(getCString(edlPath));
		if (edl.isOpen())
		{
			OEHeaderInfo headerInfo = edl.getHeaderInfo();
			NSString *edlLabel = getNSString(headerInfo.label);
			NSString *edlImage = [resourcePath stringByAppendingPathComponent:
								  getNSString(headerInfo.image)];
			NSString *edlDescription = getNSString(headerInfo.description);
			
			ChooserItem *item = [[ChooserItem alloc] initWithTitle:edlLabel
														  subtitle:edlDescription
														 imagePath:edlImage
														   edlPath:edlPath
															  data:nil];
			if (item)
			{
				NSMutableArray *groupsItem;
				if (![groups objectForKey:group])
				{
					groupsItem = [[[NSMutableArray alloc] init] autorelease];
					[groups setObject:groupsItem forKey:group];
				}
				else
					groupsItem = [groups objectForKey:group];
				
				[groupsItem addObject:item];
				[item release];
			}
		}
	}
}

- (void)removeGroup:(NSString *)group
{
	if ([groups objectForKey:group])
	{
		[groups removeObjectForKey:group];
		[groupNames removeLastObject];
	}
}

@end
