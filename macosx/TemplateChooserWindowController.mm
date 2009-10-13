
/**
 * OpenEmulator
 * Mac OS X Template Chooser Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import <string>

#import "OEParser.h"

#import "TemplateChooserWindowController.h"
#import "ChooserItem.h"
#import "Document.h"

@implementation TemplateChooserWindowController

- (id) init:(DocumentController *) theDocumentController
{
	self = [super initWithWindowNibName:@"TemplateChooser"];
	if (self)
	{
		documentController = theDocumentController;
		
		groups = [[NSMutableDictionary alloc] init];
		
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *templatesPath = [resourcePath
								   stringByAppendingPathComponent:@"templates"];
		[self addTemplatesFromPath:templatesPath
					forceGroupName:nil];
		
		NSString *userTemplatesPath = [TEMPLATE_FOLDER stringByExpandingTildeInPath];
		[self addTemplatesFromPath:userTemplatesPath
					forceGroupName:NSLocalizedString(@"My Templates",
													 @"My Templates")];
		
		groupNames = [NSMutableArray arrayWithArray:[[groups allKeys]
													 sortedArrayUsingSelector:
													 @selector(compare:)]];
		[groupNames retain];
	}
	
	return self;
}

- (void) dealloc
{
	[groupNames release];
	[groups release];
	[selectedGroup release];
	
	[super dealloc];
}

- (void) addTemplatesFromPath:(NSString *) path
			   forceGroupName:(NSString *) forcedGroupName
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *imagesPath = [resourcePath
							stringByAppendingPathComponent:@"images"];
	
	NSError *error;
	NSArray *templateFilenames = [[NSFileManager defaultManager]
								  contentsOfDirectoryAtPath:path
								  error:&error];
	
	int templatesCount = [templateFilenames count];
	for (int i = 0; i < templatesCount; i++)
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
		
		if (forcedGroupName)
			groupName = forcedGroupName;
		
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

- (void) selectLastTemplate
{
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	NSString *lastTemplatePath = [userDefaults stringForKey:@"OELastTemplate"];
	
	int lastGroupIndex = -1;
	int lastChooserIndex = -1;
	
	int groupsCount = [groupNames count];
	for (int i = 0; i < groupsCount; i++)
	{
		NSString *groupName = [groupNames objectAtIndex:i];
		NSArray *templates = [groups objectForKey:groupName];
		
		int templatesCount = [templates count]; 
		for (int j = 0; j < templatesCount; j++)
		{
			ChooserItem *item = [templates objectAtIndex:j];
			
			NSString *templatePath = [item itemPath];
			if ([templatePath compare:lastTemplatePath] == NSOrderedSame)
			{
				lastGroupIndex = i;
				lastChooserIndex = j;
			}
		}
	}
	
	if (lastGroupIndex == -1)
		return;
	
	[fOutlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:lastGroupIndex]
			  byExtendingSelection:NO];
	[fChooserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:lastChooserIndex]
				 byExtendingSelection:NO];
}

- (void) awakeFromNib
{
	[fOutlineView setDelegate:self];
	[fOutlineView setDataSource:self];
	[fOutlineView reloadData];
	
	NSDictionary *attrs = [NSDictionary dictionaryWithObjectsAndKeys:
						   [NSFont messageFontOfSize:11.0f], NSFontAttributeName,
						   [NSColor blackColor], NSForegroundColorAttributeName,
						   nil];
	
	NSDictionary *hAttrs = [NSDictionary dictionaryWithObjectsAndKeys:
						   [NSFont messageFontOfSize:11.0f], NSFontAttributeName,
						   [NSColor whiteColor], NSForegroundColorAttributeName,
						   nil];
	
	[fChooserView setValue:attrs
					forKey:IKImageBrowserCellsTitleAttributesKey];
	[fChooserView setValue:hAttrs
					forKey:IKImageBrowserCellsHighlightedTitleAttributesKey];
	NSSize aSize;
	aSize.width = 96;
	aSize.height = 64;
	[fChooserView setCellSize:aSize];
	[fChooserView setDelegate:self];
	[fChooserView setDataSource:self];
	[fChooserView reloadData];

	[self selectLastTemplate];
}

- (id) outlineView:(NSOutlineView *) outlineView child:(NSInteger) index ofItem:(id) item
{
	if (!item)
		return [groupNames objectAtIndex:index];
			
	return nil;
}

- (BOOL) outlineView:(NSOutlineView *) outlineView isItemExpandable:(id) item
{
	return NO;
}

- (NSInteger) outlineView:(NSOutlineView *) outlineView numberOfChildrenOfItem:(id) item
{
	if (!item)
		return [groups count];
	
	return 0;
}

- (id)outlineView: (NSOutlineView *) outlineView
objectValueForTableColumn:(NSTableColumn *) tableColumn
		   byItem:(id) item
{
	return item;
}

- (void) outlineViewSelectionDidChange:(NSNotification *) notification
{
	if (selectedGroup)
		[selectedGroup release];
	
	selectedGroup = [[groupNames objectAtIndex:[fOutlineView selectedRow]] retain];
	[fChooserView reloadData];
	[fChooserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:0]
				 byExtendingSelection:NO];
}

- (NSUInteger) numberOfItemsInImageBrowser:(IKImageBrowserView *) aBrowser
{
	if (!selectedGroup)
		return 0;
	
	return [[groups objectForKey:selectedGroup] count];
}

- (id) imageBrowser:(IKImageBrowserView *) aBrowser itemAtIndex:(NSUInteger) index
{
	if (!selectedGroup)
		return 0;
	
	return [[groups objectForKey:selectedGroup] objectAtIndex:index];
}

- (void) imageBrowser:(IKImageBrowserView *) aBrowser
cellWasDoubleClickedAtIndex:(NSUInteger) index
{
	[self performChoose:aBrowser];
}

- (void) windowDidLoad
{
	// Reset window size, leave window centered on screen
}

- (void) windowWillClose:(NSNotification *) notification
{
	if (documentController)
		[documentController noteTemplateChooserWindowClosed]; 
}

- (void) performChoose:(id) sender
{
	[[self window] performClose:self];
	
	int index = [[fChooserView selectionIndexes] firstIndex];
	ChooserItem *item = [self imageBrowser:fChooserView itemAtIndex:index];
	NSURL *url = [NSURL fileURLWithPath:[item itemPath]];
	
	if(url)
	{
		NSError *error;
		if ([documentController openUntitledDocumentFromTemplateURL:url
															display:YES
															  error:&error])
		{
			[[NSUserDefaults standardUserDefaults] setObject:[item itemPath]
													  forKey:@"OELastTemplate"];
		}
		else
			[[NSAlert alertWithError:error] runModal];
	}
}

@end
