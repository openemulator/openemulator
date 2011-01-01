
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
#import "StorageInterface.h"

#import <sstream>

void devicesDidUpdate()
{
}

void runAlert(string message)
{
}

OEComponent *addCanvas(void *userData)
{
	return new OEComponent();
}

void removeCanvas(OEComponent *canvas, void *userData)
{
	delete canvas;
}

@implementation Document

- (id)initWithTemplateURL:(NSURL *)absoluteURL
					error:(NSError **)outError
{
	if (self = [super init])
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
	
	theEmulation->setResourcePath(getCPPString([[NSBundle mainBundle] resourcePath]));
	
	theEmulation->setComponent("hostAudio", oePortAudio);
	
	theEmulation->setRunAlertCallback(runAlert);
	theEmulation->setAddCanvasCallback(addCanvas, self);
	theEmulation->setRemoveCanvasCallback(removeCanvas, self);
	theEmulation->setDevicesDidUpdateCallback(devicesDidUpdate);
	
	theEmulation->open(getCPPString([url path]));
	
	devicesInfo = new OEDevicesInfo();
	*((OEDevicesInfo *)devicesInfo) = theEmulation->getDevicesInfo();
	
	oePortAudio->addEmulation((OEEmulation *)emulation);
	
	emulation = theEmulation;
}

- (void)deleteEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	OEPortAudio *oePortAudio = (OEPortAudio *)[documentController oePortAudio];
	oePortAudio->removeEmulation((OEEmulation *)emulation);
	
	delete (OEEmulation *)emulation;
	emulation = nil;
	
	delete (OEDevicesInfo *)devicesInfo;
	devicesInfo = nil;
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

/*
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
	paragraphStyle = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
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
	aString = [[NSMutableAttributedString alloc] initWithString:deviceLabel
													 attributes:deviceLabelAttrs];
	
	NSDictionary *informativeTextAttrs = [NSDictionary dictionaryWithObjectsAndKeys:
										  [NSFont messageFontOfSize:9.0f],
										  NSFontAttributeName,
										  paragraphStyle,
										  NSParagraphStyleAttributeName,
										  [NSColor darkGrayColor],
										  NSForegroundColorAttributeName,
										  nil];
	NSAttributedString *aInformativeText;
	aInformativeText = [[NSAttributedString alloc] initWithString:informativeText
													   attributes:informativeTextAttrs];
	[aString appendAttributedString:aInformativeText];
	[aInformativeText release];
	[paragraphStyle release];
	
	[aString autorelease];
	
	return aString;
}
*/

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
		string emulationPath = getCPPString([[absoluteURL path]
										   stringByAppendingString:@"/"]);
		
		[emulationWindowController updateWindowPosition];
		
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
	NSString *path = [USER_TEMPLATES_FOLDER stringByExpandingTildeInPath];
	
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
	if (!emulation)
		return @"";
	
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
	((OEEmulation *)emulation)->setOptions(getCPPString(value));
	[self unlockEmulation];
}

- (void)addEDL:(NSString *)path
   connections:(NSDictionary *)connections
{
	if (!emulation)
		return;
	
	string pathString = getCPPString(path);
	map<string, string> connectionsMap;
	
	NSEnumerator *i = [connections keyEnumerator];
	NSString *inletRef;
	
	while (inletRef = [i nextObject])
	{
		NSString *outletRef = [connections objectForKey:inletRef];
		
		string inletRefString = getCPPString(inletRef);
		string outletRefString = getCPPString(outletRef);
		
		connectionsMap[inletRefString] = outletRefString;
	}
	
	[self lockEmulation];
	bool isAdded = ((OEEmulation *)emulation)->addEDL(pathString, connectionsMap);
	[self unlockEmulation];
	
	if (!isAdded)
	{
		NSString *messageText = @"The device could not be added.";
		
		NSAlert *alert = [[NSAlert alloc] init];
		[alert setMessageText:NSLocalizedString(messageText, messageText)];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert runModal];
		[alert release];
	}
	
	[self updateChangeCount:NSChangeDone];
}

- (void)removeDevice:(NSString *)deviceId
{
	if (!emulation)
		return;
	
//	NSString *deviceRef = [dict objectForKey:@"ref"];
//	NSString *deviceLabel = [dict objectForKey:@"label"];
	
//	string refString = getCPPString(deviceRef);
	
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
	
/*	[self lockEmulation];
	bool isRemoved = ((OEEmulation *)emulation)->removeDevice(refString);
	[self unlockEmulation];
	
	if (!isRemoved)
	{
		NSString *messageText = @"The device could not be removed.";
		
		NSAlert *alert = [[NSAlert alloc] init];
		[alert setMessageText:NSLocalizedString(messageText, messageText)];
		[alert runModal];
		[alert release];
	}
	
	[self updateChangeCount:NSChangeDone];*/
}

- (NSString *)getValue:(NSString *)name forComponent:(NSString *)theId
{
	string value;
	if (emulation)
	{
		OEEmulation *theEmulation = (OEEmulation *)emulation;
		
		[self lockEmulation];
		OEComponent *component = theEmulation->getComponent(getCPPString(theId));
		if (component)
			component->getValue(getCPPString(name), value);
		[self unlockEmulation];
	}
	
	return getNSString(value);
}

- (BOOL)mount:(NSString *)path
{
	BOOL result = NO;
	if (emulation)
	{
		OEEmulation *theEmulation = (OEEmulation *)emulation;
		string thePath = getCPPString(path);
		
		[self lockEmulation];
		result = theEmulation->postMessage(NULL, (int)OEEMULATION_MOUNT, &thePath);
		[self unlockEmulation];
	}
	
	return result;
}

- (BOOL)mount:(NSString *)path inComponent:(NSString *)theId
{
	BOOL result = NO;
	if (emulation)
	{
		OEEmulation *theEmulation = (OEEmulation *)emulation;
		string thePath = getCPPString(path);
		
		[self lockEmulation];
		OEComponent *component = theEmulation->getComponent(getCPPString(theId));
		if (component)
			result = component->postMessage(NULL, (int)STORAGE_MOUNT, &thePath);
		[self unlockEmulation];
	}
	
	return result;
}

- (BOOL)mountable:(NSString *)path
{
	BOOL result = NO;
	if (emulation)
	{
		OEComponent *component = (OEComponent *)emulation;
		string thePath = getCPPString(path);
		
		[self lockEmulation];
		result = component->postMessage(NULL, (int)OEEMULATION_IS_MOUNTABLE, &thePath);
		[self unlockEmulation];
	}
	
	return result;
}

@end
