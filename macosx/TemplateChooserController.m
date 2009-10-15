
/**
 * OpenEmulator
 * Mac OS X Template Chooser Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the new document from template window.
 */

#import "TemplateChooserController.h"

@implementation TemplateChooserController

- (id) init
{
	self = [super initWithWindowNibName:@"TemplateChooser"];
	
	if (self)
	{
		templateChooser = [[TemplateChooser alloc] init];
		[templateChooser setDelegate:self];
	}
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
	
	[templateChooser release];
}

- (void) windowDidLoad
{
	[super windowDidLoad];
	
	[self setWindowFrameAutosaveName:@"TemplateChooser"];
	
	[templateChooser populateOutlineView:fOutlineView
						  andChooserView:fChooserView];
	
	NSString *itemPath = [[NSUserDefaults standardUserDefaults]
						  stringForKey:@"OELastTemplate"];
	[templateChooser selectItemWithItemPath:itemPath];
}

- (void) templateChooserSelectionDidChange:(id) sender
{
	NSString *selectedItemPath = [templateChooser selectedItemPath];
	if (!selectedItemPath)
		return;
	
	NSLog(@"selectedItemPath: %@", selectedItemPath);
	[[NSUserDefaults standardUserDefaults] setObject:selectedItemPath
											  forKey:@"OELastTemplate"]; 	 
}

- (void) templateChooserWasDoubleClicked:(id) sender
{
	NSURL *url = [NSURL fileURLWithPath:[templateChooser selectedItemPath]];
	
	[[self window] orderOut:self];
	
	if (url)
	{
		NSError *error;
		if (![fDocumentController openUntitledDocumentFromTemplateURL:url
															  display:YES
																error:&error])
			[[NSAlert alertWithError:error] runModal];
	}
}

@end
