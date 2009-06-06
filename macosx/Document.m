
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
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
	}
	
	return self;
}

- (void)dealloc
{
	[pasteboardTypes release];
	[pasteboard release];

	[super dealloc];
}

- (void)makeWindowControllers {
    NSArray *myControllers = [self windowControllers];
	
    // If this document displaced a transient document,
	// it will already have been assigned a window controller.
	// If that is not the case, create one.
    if ([myControllers count] == 0) {
        [self addWindowController:[[[DocumentWindowController allocWithZone:[self zone]] init]
								   autorelease]];
    }
}

- (BOOL)readFromFileWrapper:(NSFileWrapper *)fileWrapper
					 ofType:(NSString *)typeName
					  error:(NSError **)outError
{
    if ( outError != NULL ) {
	}
    return YES;
}

- (NSFileWrapper *)fileWrapperOfType:(NSString *)typeName error:(NSError **)outError {
	*outError = nil;
	return nil;
	//[[NSFileWrapper alloc] initWithPath:@"lolo"];
}

- (void)togglePower:(id)sender
{
}

- (void)resetEmulation:(id)sender
{
}

- (void)togglePause:(id)sender
{
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

- (NSString *)getDocumentText
{
	// To-Do: libemulation
	return @"This is a test of the speech synthesizing system.";  
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
	NSTextView * dummy = [[NSTextView alloc] init];
	[dummy insertText:[self getDocumentText]];
	[dummy startSpeaking:nil];
}

@end
