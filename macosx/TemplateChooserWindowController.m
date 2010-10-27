
/**
 * OpenEmulator
 * Mac OS X Template Chooser Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import "TemplateChooserWindowController.h"

@implementation TemplateChooserWindowController

- (id)init
{
	self = [super initWithWindowNibName:@"TemplateChooser"];
	
	if (self)
	{
		templateChooserViewController = [[TemplateChooserViewController alloc] init];
		[templateChooserViewController setDelegate:self];
	}
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
	
	[templateChooserViewController release];
}

- (void)run
{
	[self showWindow:self];
	
	[templateChooserViewController updateUserTemplates];
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSString *itemPath = [defaults stringForKey:@"OELastTemplate"];
	[templateChooserViewController selectItemWithPath:itemPath];
	
	[[self window] center];
	
	[fChooseButton setEnabled:([templateChooserViewController selectedItemPath]
							   != nil)];
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"TemplateChooser"];
	
	NSView *view = [templateChooserViewController view];
	[fTemplateChooserView addSubview:view];
	[view setFrameSize:[fTemplateChooserView frame].size];
}

- (void)chooserWasDoubleClicked:(id)sender
{
	[self chooseTemplate:sender];
}

- (IBAction)chooseTemplate:(id)sender
{
	NSString *templatePath = [templateChooserViewController selectedItemPath];
	NSURL *url = nil;
	if (templatePath)
	{
		NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
		[defaults setObject:templatePath
					 forKey:@"OELastTemplate"];
		url = [NSURL fileURLWithPath:templatePath];
	}
	
	[[self window] orderOut:self];
	
	if (url)
	{
		NSError *error;
		if (![fDocumentController openUntitledDocumentWithTemplateURL:url
															  display:YES
																error:&error])
			[[NSAlert alertWithError:error] runModal];
	}
}

@end
