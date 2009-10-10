
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

@implementation TemplateChooserWindowController

- (id) init:(DocumentController *) theDocumentController
{
	self = [super initWithWindowNibName:@"TemplateChooser"];
	if (self)
	{
		documentController = theDocumentController;
		
		templates = [[NSMutableDictionary alloc] init];
		
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *imageFolderPath = [resourcePath
									 stringByAppendingPathComponent:@"images"];
		NSString *templateFolderPath = [resourcePath
										stringByAppendingPathComponent:@"templates"];
		NSError *error;
		NSArray *templateLocalPaths = [[NSFileManager defaultManager]
									   contentsOfDirectoryAtPath:templateFolderPath
									   error:&error];
		
		int templatesCount = [templateLocalPaths count];
		for (int i = 0; i < templatesCount; i++)
		{
			NSString *templateLocalPath = [templateLocalPaths objectAtIndex:i];
			NSString *templatePath = [templateFolderPath stringByAppendingPathComponent:
									  templateLocalPath];
			string templatePathString = string([templatePath UTF8String]);
			OEParser parser(templatePathString);
			if (!parser.isOpen())
				continue;
			
			OEDMLInfo *dmlInfo = parser.getDMLInfo();
			NSString *label = [NSString stringWithUTF8String:dmlInfo->label.c_str()];
			NSString *imageName = [NSString stringWithUTF8String:dmlInfo->image.c_str()];
			NSString *description = [NSString stringWithUTF8String:dmlInfo->image.c_str()];
			NSString *groupName = [NSString stringWithUTF8String:dmlInfo->group.c_str()];
			
			if (![templates objectForKey:groupName])
			{
				if (!selectedGroup)
					selectedGroup = [groupName retain];
				
				NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
				[templates setObject:group forKey:groupName];
			}
			NSString *imagePath = [imageFolderPath stringByAppendingPathComponent:
								   imageName];
			ChooserItem *item = [[[ChooserItem alloc] initWithItem:templatePath
															 label:label
														 imagePath:imagePath
													   description:description]
								 autorelease];
			[[templates objectForKey:groupName] addObject:item];
		}
		
		groups = [NSMutableArray arrayWithArray:[[templates allKeys]
												 sortedArrayUsingSelector:
												 @selector(compare:)]];
		[groups retain];
		
		// Add only if there are personal templates
//		[groups addObject:@"My Templates"];
	}
	
	return self;
}

- (void) dealloc
{
	[groups release];
	[templates release];
	[selectedGroup release];
	
	[super dealloc];
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
	[fChooserView setCellsStyleMask:IKCellsStyleTitled];
	[fChooserView setAllowsMultipleSelection:NO];
	[fChooserView setAllowsEmptySelection:NO];
	[fChooserView setDelegate:self];
	[fChooserView setDataSource:self];
	[fChooserView reloadData];
	
	// Init outline and chooser to last initialized values
	[fOutlineView selectRowIndexes:[NSIndexSet
									indexSetWithIndex:[groups indexOfObject:@"Apple II"]]
			  byExtendingSelection:NO];
	[fChooserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:0]
				 byExtendingSelection:NO];
}

- (id) outlineView:(NSOutlineView *) outlineView child:(NSInteger) index ofItem:(id) item
{
	if (!item)
		return [groups objectAtIndex:index];
	
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
	
	selectedGroup = [[groups objectAtIndex:[fOutlineView selectedRow]] retain];
	[fChooserView reloadData];
	[fChooserView setSelectionIndexes:[NSIndexSet indexSetWithIndex:0]
				 byExtendingSelection:NO];
}

- (NSUInteger) numberOfItemsInImageBrowser:(IKImageBrowserView *) aBrowser
{
	if (!selectedGroup)
		return 0;
	
	return [[templates objectForKey:selectedGroup] count];
}

- (id) imageBrowser:(IKImageBrowserView *) aBrowser itemAtIndex:(NSUInteger) index
{
	if (!selectedGroup)
		return 0;
	
	return [[templates objectForKey:selectedGroup] objectAtIndex:index];
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
		if (![documentController openUntitledDocumentFromTemplateURL:url
															 display:YES
															   error:&error])
			[[NSAlert alertWithError:error] runModal];
	}
}

@end
