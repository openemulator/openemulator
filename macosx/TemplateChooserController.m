
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
	
	[templateChooser setupOutlineView:fOutlineView
					   andChooserView:fChooserView];
	
	[self updateTemplate];
}

- (void) updateTemplate
{
	[templateChooser updateUserTemplates];
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	NSString *itemPath = [userDefaults stringForKey:@"OELastTemplate"];
	[templateChooser selectItemWithItemPath:itemPath];
	
	[[self window] center];
}

- (void) showWindow:(id) sender
{
	[self updateTemplate];
	[super showWindow:sender];
}

- (void) templateChooserWasDoubleClicked:(id) sender
{
	[self chooseTemplate:sender];
}

- (IBAction) chooseTemplate:(id) sender
{
	NSString *itemPath = [templateChooser selectedItemPath];
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	[userDefaults setObject:itemPath
					 forKey:@"OELastTemplate"];
	
	NSURL *url = [NSURL fileURLWithPath:itemPath];
	
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
