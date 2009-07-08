
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

- (id)initFromTemplate:(NSURL *)templateURL
{
	if (self = [super init])
	{
		pasteboard = [NSPasteboard generalPasteboard];
		[pasteboard retain];
		
		pasteboardTypes = [NSArray arrayWithObjects:NSStringPboardType, nil];
		[pasteboardTypes retain];
	}
	
	return self;
}

- (id)init
{
	return [self initFromTemplate:nil];
}

- (void)dealloc
{
	[pasteboardTypes release];
	[pasteboard release];
	
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
		return [self validateCopy];
	else if ([item action] == @selector(paste:))
		return [self validatePaste];
	else if ([item action] == @selector(startSpeaking:))
		return [self validateCopy];
	
	return YES;
}

- (BOOL)validateCopy
{
	return YES; // To-Do: libemulation
}

- (BOOL)validatePaste
{
	return [pasteboard availableTypeFromArray:pasteboardTypes] != nil;
}

- (BOOL)readFromFileWrapper:(NSFileWrapper *)fileWrapper
					 ofType:(NSString *)typeName
					  error:(NSError **)outError
{
	// Read files to memory, distribute to objects
	// Pass emulation path to libemulator
	// Note: Files should be opened once the emulation starts
	// In case someone moves the emulation, they can be rewritten without problem
	return YES;
}

- (NSFileWrapper *)fileWrapperOfType:(NSString *)typeName error:(NSError **)outError {
	NSFileWrapper *fileWrapper = [[[NSFileWrapper alloc] initDirectoryWithFileWrappers:nil]
								  autorelease];
	// Generate file wrapper
	// Add files
	// And write to files
	return fileWrapper;
}

- (void)togglePower:(id)sender
{
	[self updateChangeCount:NSChangeDone];
}

- (void)resetEmulation:(id)sender
{
}

- (void)togglePause:(id)sender
{
}

- (NSString *)getDocumentText
{
	// To-Do: libemulation
	return @"This is a meticulously designed test of the speech synthesizing system.";  
}

- (void)copy:(id)sender
{
	if ([self validateCopy])
	{
		[pasteboard declareTypes:pasteboardTypes owner:self];
		[pasteboard setString:[self getDocumentText] forType:NSStringPboardType];
	}
}

- (void)paste:(id)sender
{
	if ([self validatePaste])
	{
		NSString *text = [pasteboard stringForType:NSStringPboardType];
		NSMutableString *mutableText = [NSMutableString 
										stringWithString:text];
		
		// Convert newlines
		[mutableText replaceOccurrencesOfString:@"\n"
									 withString:@"\r"
										options:NSLiteralSearch
										  range:NSMakeRange(0, [text length])];
		
		// To-do: Send to libemulation with [mutableText cStringUsingEncoding:NSASCIIStringEncoding]
	}
}

- (void)startSpeaking:(id)sender
{
	NSTextView *dummy = [[NSTextView alloc] init];
	[dummy insertText:[self getDocumentText]];
	[dummy startSpeaking:nil];
}

@end
