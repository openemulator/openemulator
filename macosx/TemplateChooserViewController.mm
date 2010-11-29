
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser view.
 */

#import "TemplateChooserViewController.h"
#import "Document.h"
#import "ChooserItem.h"
#import "StringConversion.h"

#import "OEEDL.h"

@implementation TemplateChooserViewController

- (void)awakeFromNib
{
	[super awakeFromNib];
	
	NSString *templatesPath = [[[NSBundle mainBundle] resourcePath]
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
	NSArray *edlFilenames = [[NSFileManager defaultManager]
							 contentsOfDirectoryAtPath:path
							 error:nil];
	
	for (int i = 0; i < [edlFilenames count]; i++)
	{
		NSString *edlFilename = [edlFilenames objectAtIndex:i];
		NSString *edlPath = [path stringByAppendingPathComponent:edlFilename];
		
		OEEDL edl;
		edl.open(getCString(edlPath));
		if (edl.isOpen())
		{
			OEHeaderInfo headerInfo = edl.getHeaderInfo();
			NSString *groupName = @"";//getNSString(headerInfo.type);
			NSString *label = getNSString(headerInfo.label);
			NSString *imageName = getNSString(headerInfo.image);
			NSString *description = getNSString(headerInfo.description);
			
			if (theGroupName)
				groupName = theGroupName;
			
			if (![groups objectForKey:groupName])
			{
				NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
				[groups setObject:group forKey:groupName];
			}
			
			NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
			NSString *imagePath = [resourcePath stringByAppendingPathComponent:imageName];
			ChooserItem *item = [[ChooserItem alloc] initWithTitle:label
														  subtitle:description
														 imagePath:imagePath
														   edlPath:edlPath
															  data:nil];
			if (item)
			{
				[item autorelease];
				[[groups objectForKey:groupName] addObject:item];
			}
		}
	}
}

@end
