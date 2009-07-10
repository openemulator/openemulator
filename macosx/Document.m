
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation document.
 */

#import "Document.h"
#import "DocumentWindowController.h"

@implementation Document

- (id)init
{
	if (self = [super init])
	{
		pasteboard = [NSPasteboard generalPasteboard];
		[pasteboard retain];
		
		pasteboardTypes = [NSArray arrayWithObjects:NSStringPboardType, nil];
		[pasteboardTypes retain];
		
		// To-Do: Create a temporary work folder
		
		computerType = @"Apple II";
	}
	
	return self;
}

- (id)initWithTemplateURL:(NSURL *)templateURL
					error:(NSError **)outError
{
	// To-Do: If there is a template, copy the template's files to the work folder
	// To-Do: Read info.xml to update inspector
	return [self init];
	
	// To-Do: Start emulation
}

- (void)dealloc
{
	[pasteboardTypes release];
	[pasteboard release];
	
	// To-Do: Remove temporary work folder
	
	[super dealloc];
}

- (void)makeWindowControllers
{
	NSWindowController *windowController;
	windowController = [[DocumentWindowController alloc] initWithWindowNibName:@"Document"];
	
	[self addWindowController:windowController];
	[windowController release];
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(copy:))
		return [self isCopyValid];
	else if ([item action] == @selector(paste:))
		return [self isPasteValid];
	else if ([item action] == @selector(startSpeaking:))
		return [self isCopyValid];
	
	return YES;
}

- (BOOL)isCopyValid
{
	return YES; // To-Do: libemulation
}

- (BOOL)isPasteValid
{
	return [pasteboard availableTypeFromArray:pasteboardTypes] != nil;
}

- (BOOL)readFromURL:(NSURL *)absoluteURL
			 ofType:(NSString *)typeName
			  error:(NSError **)outError
{
	// To-Do: Erase the files in the temporary folder
	// To-Do: Unzip the files in the .emulation file to the temporary work folder
	// To-Do: Read info.xml to update inspector
	// To-Do: Force a restart of libemulator
	
	*outError = [NSError errorWithDomain:@"test" code:0 userInfo:nil];
	printf("readFromURL: %s\n", [[absoluteURL path] UTF8String]);
	return YES;
}

- (BOOL)writeToURL:(NSURL *)absoluteURL
			ofType:(NSString *)typeName
			 error:(NSError **)outError
{
	// To-Do: Force a state write in libemulator to our temporary work folder
	// To-Do: Compress in zip format
	NSString *path = [absoluteURL path];
	FILE *fp;
	fp = fopen([path UTF8String], "wb");
	if (fp)
		fclose(fp);
	printf("writeToURL: %s\n", [path UTF8String]);
	return YES;
}

- (IBAction)saveDocumentAsTemplate:(id)sender
{
	NSSavePanel *panel = [[NSSavePanel alloc] init];
	
	[panel setRequiredFileType:@"emulation"];
	
	[panel beginSheetForDirectory:@"/Users/mressl/Library/"
							 file:nil
				   modalForWindow:[self windowForSheet]
					modalDelegate:self
				   didEndSelector:@selector(saveDocumentAsTemplateDidEnd:returnCode:contextInfo:)
					  contextInfo:nil];
}

- (void)saveDocumentAsTemplateDidEnd:(NSSavePanel *)panel
						  returnCode:(int)returnCode
						 contextInfo:(void *)contextInfo
{
	// To-Do: Call [self writeToURL:URL ]
}

- (void)togglePower:(id)sender
{
	// To-Do: libemulation
}

- (void)resetEmulation:(id)sender
{
	// To-Do: libemulation
	[self updateChangeCount:NSChangeDone];
}

- (void)togglePause:(id)sender
{
	// To-Do: libemulation
}

- (NSString *)getDocumentText
{
	// To-Do: libemulation
	return @"This is a meticulously designed test of the speech synthesizing system.";  
}

- (void)copy:(id)sender
{
	if ([self isCopyValid])
	{
		[pasteboard declareTypes:pasteboardTypes owner:self];
		[pasteboard setString:[self getDocumentText] forType:NSStringPboardType];
	}
}

- (void)paste:(id)sender
{
	if ([self isPasteValid])
	{
		NSString *text = [pasteboard stringForType:NSStringPboardType];
		NSMutableString *mutableText = [NSMutableString 
										stringWithString:text];
		
		// To-Do: Convert newlines in libemulator
		[mutableText replaceOccurrencesOfString:@"\n"
									 withString:@"\r"
										options:NSLiteralSearch
										  range:NSMakeRange(0, [text length])];
		
		// To-do: Send to libemulator
		// (using [mutableText cStringUsingEncoding:NSASCIIStringEncoding])
	}
}

- (void)startSpeaking:(id)sender
{
	NSTextView *dummy = [[NSTextView alloc] init];
	[dummy insertText:[self getDocumentText]];
	[dummy startSpeaking:self];
}

@end
