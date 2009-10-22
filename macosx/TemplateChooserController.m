
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
		chooserController = [[ChooserController alloc] initWithTemplates];
		[chooserController setDelegate:self];
	}
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
	
	[chooserController release];
}

- (void) windowDidLoad
{
	[self setWindowFrameAutosaveName:@"TemplateChooser"];
	
	NSView *view = [chooserController view];
	[fChooserView addSubview:view];
	[view setFrame:[fChooserView bounds]];
	
	[self updateTemplates];
}

- (void) updateTemplates
{
	[chooserController updateUserTemplates];
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	NSString *itemPath = [userDefaults stringForKey:@"OELastTemplate"];
	[chooserController selectItemWithItemPath:itemPath];
	
	[[self window] center];
}

- (void) showWindow:(id) sender
{
	[self updateTemplates];
	
	[super showWindow:sender];
}

- (void) chooserWasDoubleClicked:(id) sender
{
	[self chooseTemplate:sender];
}

- (IBAction) chooseTemplate:(id) sender
{
	NSString *itemPath = [chooserController selectedItemPath];
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
