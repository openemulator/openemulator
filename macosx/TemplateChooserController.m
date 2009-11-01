
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
		templateChooserViewController = [[TemplateChooserViewController alloc] init];
		[templateChooserViewController setDelegate:self];
	}
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
	
	[templateChooserViewController release];
}

- (void) updateTemplateChooser
{
	[templateChooserViewController updateUserTemplates];
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	NSString *itemPath = [userDefaults stringForKey:@"OELastTemplate"];
	[templateChooserViewController selectItemWithPath:itemPath];
	
	[[self window] center];
	
	[fChooseButton setEnabled:([templateChooserViewController selectedItemPath]
							   != nil)];
}

- (void) awakeFromNib
{
	[self setWindowFrameAutosaveName:@"TemplateChooser"];
	
	NSView *view = [templateChooserViewController view];
	[fTemplateChooserView addSubview:view];
	
	[self updateTemplateChooser];
}

- (void) showWindow:(id) sender
{
	[self updateTemplateChooser];
	
	[super showWindow:sender];
}

- (void) chooserWasDoubleClicked:(id) sender
{
	[self chooseTemplate:sender];
}

- (IBAction) chooseTemplate:(id) sender
{
	NSString *itemPath = [templateChooserViewController selectedItemPath];
	NSURL *url = nil;
	if (itemPath)
	{
		NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
		[userDefaults setObject:itemPath
						 forKey:@"OELastTemplate"];
		url = [NSURL fileURLWithPath:itemPath];
	}
	
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
