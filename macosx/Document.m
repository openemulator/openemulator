
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
		
		[self setPower:YES];
		[self setPause:NO];
		[self setLabel:@"Apple II"];
		[self setDescription:@"The Apple II is a nice computer."];
		[self setModificationDate:[NSDate date]];
		[self setVolume:1.0f];
		expansions = [[NSMutableArray alloc] init];
		diskDrives = [[NSMutableArray alloc] init];
		portDevices = [[NSMutableArray alloc] init];
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
	//        or just copy files if it is a package
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
	//        or just copy files if destination is a package
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
	
	[panel beginSheetForDirectory:@"/Users/mressl/Library/Application Support/OpenEmulator/Templates/"
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
	[self setPower:![self power]];
	// To-Do: libemulation
}

- (void)resetEmulation:(id)sender
{
	// To-Do: libemulation
	[self updateChangeCount:NSChangeDone];
}

- (void)togglePause:(id)sender
{
	[self setPause:![self pause]];
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
	[dummy release];
}

- (BOOL)power
{
	return power;
}

- (void)setPower:(BOOL)value
{
	if (power != value)
		power = value;
}

- (BOOL)pause
{
	return pause;
}

- (void)setPause:(BOOL)value
{
	if (pause != value)
		pause = value;
}

- (NSString *)label
{
	return [[label retain] autorelease];
}

- (void)setLabel:(NSString *)value
{
    if (label != value)
	{
        [label release];
        label = [value copy];
    }
}

- (NSString *)description
{
	return [[description retain] autorelease];
}

- (void)setDescription:(NSString *)value
{
    if (description != value)
	{
        [description release];
        description = [value copy];
    }
}

- (NSDate *)modificationDate
{
	return modificationDate;
}

- (void)setModificationDate:(NSDate *)value
{
    if (modificationDate != value)
	{
        [modificationDate release];
        modificationDate = [value copy];
    }
}

- (NSImage *)image
{
	return [[image retain] autorelease];
}

- (void)setImage:(NSImage *)value
{
    if (image != value)
	{
        [image release];
        image = [value copy];
    }
}

- (NSMutableArray *)expansions
{
	return [[expansions retain] autorelease];
}

- (void)insertObject:(id)value inExpansionsAtIndex:(NSUInteger)index
{
    [expansions insertObject:value atIndex:index];
}

- (void)removeObjectFromExpansionsAtIndex:(NSUInteger)index
{
    [expansions removeObjectAtIndex:index];
}

- (NSMutableArray *)diskDrives
{
	return [[diskDrives retain] autorelease];
}

- (void)insertObject:(id)value inDiskDrivesAtIndex:(NSUInteger)index
{
    [diskDrives insertObject:value atIndex:index];
}

- (void)removeObjectFromDiskDrivesAtIndex:(NSUInteger)index
{
    [diskDrives removeObjectAtIndex:index];
}

- (NSMutableArray *)portDevices
{
	return [[portDevices retain] autorelease];
}

- (void)insertObject:(id)value inPortDevicesAtIndex:(NSUInteger)index
{
    [portDevices insertObject:value atIndex:index];
}

- (void)removeObjectFromPortDevicesAtIndex:(NSUInteger)index
{
    [portDevices removeObjectAtIndex:index];
}

- (int)videoPreset
{
	return videoPreset;
}

- (void)setVideoPreset:(int)value
{
    if (videoPreset != value)
		videoPreset = value;
}

- (float)brightness
{
	return brightness;
}

- (void)setBrightness:(float)value
{
    if (brightness != value)
		brightness = value;
}

- (float)contrast
{
	return contrast;
}

- (void)setContrast:(float)value
{
    if (contrast != value)
		contrast = value;
}

- (float)saturation
{
	return saturation;
}

- (void)setSaturation:(float)value
{
    if (saturation != value)
		saturation = value;
}

- (float)sharpness
{
	return sharpness;
}

- (void)setSharpness:(float)value
{
    if (sharpness != value)
		sharpness = value;
}

- (float)temperature
{
	return temperature;
}

- (void)setTemperature:(float)value
{
    if (temperature != value)
		temperature = value;
}

- (float)tint
{
	return tint;
}

- (void)setTint:(float)value
{
    if (tint != value)
		tint = value;
}

- (float)volume
{
	return volume;
}

- (void)setVolume:(float)value
{
    if (volume != value)
		volume = value;
}

@end
