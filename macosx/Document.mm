
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

#import "OEEmulation.h"
#import "OEInfo.h"

#define TEMPLATE_FOLDER @"/Users/mressl/Library/Application Support"\
	"/OpenEmulator/Templates/"

@implementation Document

- (id)init
{
	printf("[Document init]\n");
	if (self = [super init])
	{
		emulation = nil;
		
		pasteboard = [NSPasteboard generalPasteboard];
		pasteboardTypes = [[NSArray alloc] initWithObjects:NSStringPboardType, nil];
		
		[self setPower:false];
		[self setLabel:@""];
		[self setDescription:@""];
		[self setModificationDate:[NSDate date]];
		[self setImage:nil];
		
		expansions = [[NSMutableArray alloc] init];
		diskDrives = [[NSMutableArray alloc] init];
		peripherals = [[NSMutableArray alloc] init];
		
		// To-Do: [self setVideoPreset:x];
		[self setVolume:1.0f];
	}
	
	return self;
}

- (id)initFromTemplateURL:(NSURL *)absoluteURL
					error:(NSError **)outError
{
	printf("[Document initFromTemplateURL]\n");
	if ([self init])
	{
		if ([self readFromURL:absoluteURL
					   ofType:nil
						error:outError])
			return self;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileReadUnknownError
								userInfo:nil];
	return nil;
}

- (void)dealloc
{
	printf("dealloc\n");
	
	[pasteboardTypes release];
	
	if (emulation)
		delete (OEEmulation *) emulation;
	
	[super dealloc];
}

- (BOOL)readFromURL:(NSURL *)absoluteURL
			 ofType:(NSString *)typeName
			  error:(NSError **)outError
{
	printf("[Document readFromURL]\n");
	const char *emulationPath = [[absoluteURL path] UTF8String];
	const char *resourcePath = [[[NSBundle mainBundle] resourcePath] UTF8String];
	
	if (emulation)
		delete (OEEmulation *) emulation;
	
	emulation = (void *) new OEEmulation(emulationPath, resourcePath);
	
	if (emulation)
	{
		if (((OEEmulation *) emulation)->isOpen())
		{
//			xmlDocPtr dml = ((OEEmulation *) emulation)->getDML();
//			OEInfo oeInfo(dml);
//			OEProperties properties = oeInfo.getProperties();
			
//			string desc = properties["description"];
//			const char *d = desc.c_str();
//			[self setDescription:[NSString stringWithUTF8String:d]];
//			string description = properties["description"];
//			[self setDescription:[NSString stringWithUTF8String:description.c_str()]];
			
			return YES;
		}
		
		delete (OEEmulation *) emulation;
		emulation = NULL;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileReadUnknownError
								userInfo:nil];
	return NO;
}

- (BOOL)writeToURL:(NSURL *)absoluteURL
			ofType:(NSString *)typeName
			 error:(NSError **)outError
{
	printf("[Document writeToURL]\n");
	const char *emulationPath = [[[absoluteURL path] stringByAppendingString:@"/"]
								 UTF8String];
	if (emulation)
	{
		if (((OEEmulation *) emulation)->save(string(emulationPath)))
			return YES;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileWriteUnknownError
								userInfo:nil];
	return NO;
}

- (IBAction)saveDocumentAsTemplate:(id)sender
{
	NSString *path = @"~/Library/Application Support/Open Emulator/Templates";
	path = [path stringByExpandingTildeInPath];
	
	NSFileManager *fileManager = [NSFileManager defaultManager];
	if (![fileManager fileExistsAtPath:path])
		[fileManager createDirectoryAtPath:path
			   withIntermediateDirectories:YES
								attributes:nil
									 error:nil];
	
	NSSavePanel *panel = [[NSSavePanel alloc] init];
	[panel setRequiredFileType:@"emulation"];
	[panel beginSheetForDirectory:path
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
	if (returnCode != NSOKButton)
		return;
	
	NSError *error;
	if (![self writeToURL:[panel URL]
				   ofType:nil
					error:&error])
		[[NSAlert alertWithError:error] runModal];
}

- (void)makeWindowControllers
{
	NSWindowController *windowController;
	windowController = [[DocumentWindowController alloc]
						initWithWindowNibName:@"Document"];
	
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

- (void)powerButtonPressedAndReleased:(id)sender
{
	[self powerButtonPressed:sender];
	[self powerButtonReleased:sender];
}

- (void)powerButtonPressed:(id)sender
{
	[self setPower:![self power]];
	// To-Do: libemulation
}

- (void)powerButtonReleased:(id)sender
{
	// To-Do: libemulation
}

- (void)resetButtonPressedAndReleased:(id)sender
{
	[self resetButtonPressed:sender];
	[self resetButtonReleased:sender];
}

- (void)resetButtonPressed:(id)sender
{
	// To-Do: libemulation
	[self updateChangeCount:NSChangeDone];
}

- (void)resetButtonReleased:(id)sender
{
	// To-Do: libemulation
}

- (void)pauseButtonPressed:(id)sender
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

- (NSMutableArray *)peripherals
{
	return [[peripherals retain] autorelease];
}

- (void)insertObject:(id)value inPeripheralsAtIndex:(NSUInteger)index
{
    [peripherals insertObject:value atIndex:index];
}

- (void)removeObjectFromPeripheralsAtIndex:(NSUInteger)index
{
    [peripherals removeObjectAtIndex:index];
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
