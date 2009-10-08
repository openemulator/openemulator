
/**
 * OpenEmulator
 * Mac OS X Template Chooser Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import "TemplateChooserWindowController.h"

@implementation TemplateChooserWindowController

- (id)init:(DocumentController *)theDocumentController
{
	self = [super initWithWindowNibName:@"TemplateChooser"];
	if (self)
		documentController = theDocumentController;
	
	groups = [NSArray arrayWithObjects:
			  @"Amiga",
			  @"Apple I, II, III",
			  @"Apple Lisa",
			  @"Atari",
			  @"Commodore",
			  @"Mac (680x0)",
			  @"Mac (PowerPC)",
			  @"PC",
			  @"ZX Spectrum",
			  @"My Templates", nil];
	if (groups)
		[groups retain]; 
	
	return self;
}

- (void)dealloc
{
	[groups release];
	[super dealloc];
}

- (void)awakeFromNib
{
	// To-Do: Read all templates
	// Add them to the group list
	// Populate the template list when the group list changes
	
	[outlineView setDataSource:self];
	[outlineView reloadData];
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	if (!item)
		return [groups objectAtIndex:index];
	
	return nil;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	return NO;
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	if (!item)
		return [groups count];
	
	return 0;
}

- (id)outlineView:(NSOutlineView *)outlineView
objectValueForTableColumn:(NSTableColumn *)tableColumn
		   byItem:(id)item
{
	return item;
}

- (void)windowDidLoad
{
	// Reset window size, leave window centered on screen
}

- (void)windowWillClose:(NSNotification *)notification
{
	if (documentController)
		[documentController noteTemplateChooserWindowClosed]; 
}

- (void)performChoose:(id)sender
{
	[[self window] performClose:self];
	
	NSURL *url = [NSURL fileURLWithPath:@"/Users/mressl/Apple II.emulation"];
	
	NSError *error;
	if (![documentController openUntitledDocumentFromTemplateURL:url
														 display:YES
														   error:&error])
		[[NSAlert alertWithError:error] runModal];
}

@end
