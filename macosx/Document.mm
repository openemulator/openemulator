
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import "Document.h"
#import "DocumentController.h"
#import "EmulationWindowController.h"
#import "StringConversion.h"

#import "OEPortAudio.h"
#import "OEEmulation.h"

#import <sstream>

@implementation Document

- (id)init
{
	self = [super init];
	
	return self;
}

- (id)initWithTemplateURL:(NSURL *)absoluteURL
					error:(NSError **)outError
{
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
	[self deleteEmulation];
	
	[emulationWindowController release];
	
	[super dealloc];
}

- (void)showEmulation:(id)sender
{
	[emulationWindowController showWindow:sender];
}

- (void)newEmulation:(NSURL *)url
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	OEPortAudio *oePortAudio = (OEPortAudio *)[documentController oePortAudio];
	
	OEEmulation *theEmulation = new OEEmulation();
	theEmulation->setResourcePath(getCString([[NSBundle mainBundle] resourcePath]));
	theEmulation->setComponent("hostAudio", oePortAudio);
	theEmulation->open(getCString([url path]));
	
	OEDevicesInfo *theDevicesInfo = new OEDevicesInfo();
	*theDevicesInfo = theEmulation->getDevicesInfo();
	devicesInfo = theDevicesInfo;
	
	oePortAudio->addEmulation((OEEmulation *)emulation);
	
	emulation = theEmulation;
}

- (void)deleteEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	OEPortAudio *oePortAudio = (OEPortAudio *)[documentController oePortAudio];
	oePortAudio->removeEmulation((OEEmulation *)emulation);
	
	delete (OEDevicesInfo *)devicesInfo;
	delete (OEEmulation *)emulation;
	
	emulation = nil;
}

- (void)lockEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	OEPortAudio *oePortAudio = (OEPortAudio *)[documentController oePortAudio];
	oePortAudio->lockEmulations();
}

- (void)unlockEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	OEPortAudio *oePortAudio = (OEPortAudio *)[documentController oePortAudio];
	oePortAudio->unlockEmulations();
}

- (void *)emulation
{
	return emulation;
}

- (NSImage *)getResourceImage:(NSString *)imagePath
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *path = [[resourcePath
					   stringByAppendingString:@"/images/"]
					  stringByAppendingString:imagePath];
	NSImage *theImage = [[NSImage alloc] initWithContentsOfFile:path];
	if (theImage)
		[theImage autorelease];
	
	return theImage;
}

- (NSAttributedString *)formatDeviceLabel:(NSString *)deviceLabel
					  withInformativeText:(NSString *)informativeText
{
	NSMutableParagraphStyle *paragraphStyle;
	paragraphStyle = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy]
					  autorelease];
	[paragraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
	
	NSDictionary *deviceLabelAttrs = [NSDictionary dictionaryWithObjectsAndKeys:
									  [NSFont messageFontOfSize:12.0f],
									  NSFontAttributeName,
									  paragraphStyle,
									  NSParagraphStyleAttributeName,
									  [NSColor controlTextColor],
									  NSForegroundColorAttributeName,
									  nil];
	NSMutableAttributedString *aString;
	aString = [[[NSMutableAttributedString alloc] initWithString:deviceLabel
													  attributes:deviceLabelAttrs]
			   autorelease];
	
	NSDictionary *informativeTextAttrs = [NSDictionary dictionaryWithObjectsAndKeys:
										  [NSFont messageFontOfSize:9.0f],
										  NSFontAttributeName,
										  paragraphStyle,
										  NSParagraphStyleAttributeName,
										  [NSColor darkGrayColor],
										  NSForegroundColorAttributeName,
										  nil];
	NSAttributedString *aInformativeText;
	aInformativeText = [[[NSAttributedString alloc] initWithString:informativeText
														attributes:informativeTextAttrs]
						autorelease];
	[aString appendAttributedString:aInformativeText];
	
	return aString;
}

- (BOOL)readFromURL:(NSURL *)absoluteURL
			 ofType:(NSString *)typeName
			  error:(NSError **)outError
{
	[self deleteEmulation];
	[self newEmulation:absoluteURL];
	
	if (emulation)
	{
		if (((OEEmulation *)emulation)->isOpen())
			return YES;
		
		[self deleteEmulation];
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
	if (emulation)
	{
		string emulationPath = getCString([[absoluteURL path]
										   stringByAppendingString:@"/"]);
		
		[emulationWindowController updateOptions];
		
		[self lockEmulation];
		bool isSaved = ((OEEmulation *)emulation)->save(emulationPath);
		[self unlockEmulation];
		
		if (isSaved)
			return YES;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileWriteUnknownError
								userInfo:nil];
	return NO;
}

- (IBAction)saveDocumentAsTemplate:(id)sender
{
	NSString *path = [MY_TEMPLATES_FOLDER stringByExpandingTildeInPath];
	
	NSFileManager *fileManager = [NSFileManager defaultManager];
	if (![fileManager fileExistsAtPath:path])
		[fileManager createDirectoryAtPath:path
			   withIntermediateDirectories:YES
								attributes:nil
									 error:nil];
	
	NSSavePanel *panel = [NSSavePanel savePanel];
	[panel setRequiredFileType:@"emulation"];
	[panel beginSheetForDirectory:path
							 file:nil
				   modalForWindow:[self windowForSheet]
					modalDelegate:self
				   didEndSelector:@selector(saveDocumentAsTemplateDidEnd:
											returnCode:contextInfo:)
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
	emulationWindowController = [[EmulationWindowController alloc] init];
	[self addWindowController:emulationWindowController];
}

- (void *)devicesInfo
{
	return devicesInfo;
}

- (NSString *)getEDLOptions
{
	string value;
	
	if (emulation)
	{
		[self lockEmulation];
		value = ((OEEmulation *)emulation)->getOptions();
		[self unlockEmulation];
	}
	
	return getNSString(value);
}

- (void)setEDLOptions:(NSString *)value
{
	if (!emulation)
		return;
	
	[self lockEmulation];
	((OEEmulation *)emulation)->setOptions(getCString(value));
	[self unlockEmulation];
}

- (void)addEDL:(NSString *)path
   connections:(NSDictionary *)connections
{
	if (!emulation)
		return;
	
	string pathString = getCString(path);
	map<string, string> connectionsMap;
	
	NSEnumerator *i = [connections keyEnumerator];
	NSString *inletRef;
	
	while (inletRef = [i nextObject])
	{
		NSString *outletRef = [connections objectForKey:inletRef];
		
		string inletRefString = getCString(inletRef);
		string outletRefString = getCString(outletRef);
		
		connectionsMap[inletRefString] = outletRefString;
	}
	
	[self lockEmulation];
	bool isAdded = ((OEEmulation *)emulation)->addEDL(pathString, connectionsMap);
	[self unlockEmulation];
	
	if (!isAdded)
	{
		NSString *messageText = @"The device could not be added.";
		
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert setMessageText:NSLocalizedString(messageText, messageText)];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert runModal];
	}
	
	[self updateChangeCount:NSChangeDone];
}

- (void)removeDevice:(NSDictionary *)dict
{
	if (!emulation)
		return;
	
	NSString *deviceRef = [dict objectForKey:@"ref"];
//	NSString *deviceLabel = [dict objectForKey:@"label"];
	
	string refString = getCString(deviceRef);
	
/*	if (!((OEPAEmulation *)emulation)->isDeviceTerminal(refString))
	{
		NSString *messageText = @"Do you want to remove the device \u201C%@\u201D?";
		NSString *informativeText = @"There is one or more devices connected to it, "
		"which will be removed as well.";
		
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert setMessageText:[NSString localizedStringWithFormat:messageText,
							   deviceLabel, messageText]];
		[alert setInformativeText:NSLocalizedString(informativeText,
													informativeText)];
		[alert addButtonWithTitle:NSLocalizedString(@"OK", @"OK")];
		[alert addButtonWithTitle:NSLocalizedString(@"Cancel", @"Cancel")];
		if ([alert runModal] != NSAlertDefaultReturn)
			return;
	}*/
	
	[self lockEmulation];
	bool isRemoved = ((OEEmulation *)emulation)->removeDevice(refString);
	[self unlockEmulation];
	
	if (!isRemoved)
	{
		NSString *messageText = @"The device could not be removed.";
		
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert setMessageText:NSLocalizedString(messageText, messageText)];
		[alert runModal];
	}
	
	[self updateChangeCount:NSChangeDone];
}

- (BOOL)mount:(NSString *)path
{
	if (!emulation)
		return NO;
	
	[self lockEmulation];
	return ((OEEmulation *)emulation)->mount(getCString(path));
	[self unlockEmulation];
}

- (BOOL)mountable:(NSString *)path
{
	if (!emulation)
		return NO;
	
	[self lockEmulation];
	return ((OEEmulation *)emulation)->mountable(getCString(path));
	[self unlockEmulation];
}

@end
