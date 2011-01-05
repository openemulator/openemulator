
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import "Document.h"
#import "DocumentController.h"
#import "EmulationWindowController.h"
#import "CanvasWindowController.h"
#import "StringConversion.h"

#import "PortAudioHAL.h"
#import "Emulation.h"
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
	PortAudioHAL *portAudioHAL = (PortAudioHAL *)[documentController portAudioHAL];
	
	Emulation *theEmulation = new Emulation();
	
	theEmulation->setResourcePath(getCPPString([[NSBundle mainBundle] resourcePath]));
	
	theEmulation->setComponent("audio", portAudioHAL);
	
	theEmulation->setRunAlertCallback(runAlert);
	theEmulation->setAddCanvasCallback(addCanvas, self);
	theEmulation->setRemoveCanvasCallback(removeCanvas, self);
	theEmulation->setDevicesDidUpdateCallback(devicesDidUpdate);
	
	theEmulation->open(getCPPString([url path]));
	
	portAudioHAL->addEmulation((Emulation *)emulation);
	
	emulation = theEmulation;
}

- (void)deleteEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PortAudioHAL *portAudioHAL = (PortAudioHAL *)[documentController portAudioHAL];
	
	portAudioHAL->removeEmulation((Emulation *)emulation);
	
	delete (Emulation *)emulation;
	emulation = nil;
}

- (void)lockEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PortAudioHAL *portAudioHAL = (PortAudioHAL *)[documentController portAudioHAL];
	
	portAudioHAL->lockEmulations();
}

- (void)unlockEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PortAudioHAL *portAudioHAL = (PortAudioHAL *)[documentController portAudioHAL];
	
	portAudioHAL->unlockEmulations();
}

- (BOOL)readFromURL:(NSURL *)absoluteURL
			 ofType:(NSString *)typeName
			  error:(NSError **)outError
{
	[self deleteEmulation];
	[self newEmulation:absoluteURL];
	
	if (emulation)
	{
		if (((Emulation *)emulation)->isOpen())
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
		bool isSaved = ((Emulation *)emulation)->save(emulationPath);
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

	CanvasWindowController *canvasWindowController = [[CanvasWindowController alloc] initWithCanvasComponent:nil];
	[self addWindowController:canvasWindowController];
}



- (void *)emulationInfo
{
	Emulation *theEmulation = (Emulation *)emulation;
	return theEmulation->getEmulationInfo();
}

- (BOOL)mount:(NSString *)path
{
	Emulation *theEmulation = (Emulation *)emulation;
	
	[self lockEmulation];
	BOOL result = theEmulation->mount(getCPPString(path));
	[self unlockEmulation];
	
	return result;
}

- (BOOL)mount:(NSString *)path inDevice:(NSString *)deviceId
{
	return NO;
}

- (BOOL)mount:(NSString *)path inStorage:(void *)component
{
	if (!component)
		return NO;
	
	OEComponent *theComponent = (OEComponent *)component;
	string thePath = getCPPString(path);
	
	[self lockEmulation];
	BOOL result = theComponent->postMessage(NULL, (int)STORAGE_MOUNT, &thePath);
	[self unlockEmulation];
	
	return result;
}

- (BOOL)unmountStorage:(void *)component
{
	if (!component)
		return NO;
	
	OEComponent *theComponent = (OEComponent *)component;
	
	[self lockEmulation];
	BOOL result = theComponent->postMessage(NULL, (int)STORAGE_UNMOUNT, NULL);
	[self unlockEmulation];
	
	return result;
}

- (BOOL)mountable:(NSString *)path
{
	Emulation *theEmulation = (Emulation *)emulation;
	
	[self lockEmulation];
	BOOL result = theEmulation->isMountable(getCPPString(path));
	[self unlockEmulation];
	
	return result;
}

- (BOOL)getBoolForMessage:(int)message
				component:(void *)component
{
	if (!component)
		return NO;
	
	OEComponent *theComponent = (OEComponent *)component;
	BOOL value = NO;
	
	[self lockEmulation];
	theComponent->postMessage(NULL, message, &value);
	[self unlockEmulation];
	
	return value;
}

- (NSString *)getStringForMessage:(int)message
						component:(void *)component
{
	if (!component)
		return NO;
	
	OEComponent *theComponent = (OEComponent *)component;
	string value;
	
	[self lockEmulation];
	theComponent->postMessage(NULL, message, &value);
	[self unlockEmulation];
	
	return getNSString(value);
}

- (BOOL)storageMounted:(void *)component
{
	return [self getBoolForMessage:(int)STORAGE_IS_MOUNTED component:component];
}

- (BOOL)storageWritable:(void *)component
{
	return [self getBoolForMessage:(int)STORAGE_IS_WRITABLE component:component];
}

- (BOOL)storageLocked:(void *)component
{
	return [self getBoolForMessage:(int)STORAGE_IS_LOCKED component:component];
}

- (NSString *)storagePath:(void *)component
{
	return [self getStringForMessage:(int)STORAGE_GET_PATH component:component];
}

- (NSString *)storageFormat:(void *)component
{
	return [self getStringForMessage:(int)STORAGE_GET_FORMAT component:component];
}

- (NSString *)formatCapacity:(OEUInt64)value
{
	float mantissa;
	NSString *unit;
	
	if (value < 1E3)
	{
		mantissa = value / 1E0F;
		unit = @"";
	}
	else if (value < 1E6)
	{
		mantissa = value / 1E3F;
		unit = @"k";
	}
	else if (value < 1E9)
	{
		mantissa = value / 1E6F;
		unit = @"M";
	}
	else
	{
		mantissa = value / 1E9F;
		unit = @"G";
	}
	
	return [NSString localizedStringWithFormat:@"%3.2f %@B", mantissa, unit, value];
}

- (NSString *)storageCapacity:(void *)component
{
	if (!component)
		return NO;
	
	OEComponent *theComponent = (OEComponent *)component;
	OEUInt64 value = 0;
	
	[self lockEmulation];
	theComponent->postMessage(NULL, STORAGE_GET_CAPACITY, &value);
	[self unlockEmulation];
	
	return [self formatCapacity:value];
}



- (NSString *)valueOfProperty:(NSString *)theName
				 forComponent:(NSString *)theId
{
	if (!emulation)
		return @"";
	
	Emulation *theEmulation = (Emulation *)emulation;
	BOOL success = NO;
	string value;
		
	[self lockEmulation];
	OEComponent *component = theEmulation->getComponent(getCPPString(theId));
	if (component)
		success = component->getValue(getCPPString(theName), value);
	[self unlockEmulation];
		
	if (!success)
		NSLog(@"invalid property '%@' for '%@'", theName, theId);
	
	return getNSString(value);
}

- (void)setValue:(NSString *)theValue
	  ofProperty:(NSString *)theName
	forComponent:(NSString *)theId
{
	if (!emulation)
		return;
	
	Emulation *theEmulation = (Emulation *)emulation;
	BOOL success = NO;
	
	[self lockEmulation];
	OEComponent *component = theEmulation->getComponent(getCPPString(theId));
	if (component)
		success = component->setValue(getCPPString(theName), getCPPString(theValue));
	[self unlockEmulation];
	
	if (!success)
		NSLog(@"could not set property '%@' for '%@'", theName, theId);
}

- (void)addEDL:(NSString *)path
   connections:(NSDictionary *)connections
{
	/*	if (!emulation)
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
	 bool isAdded = ((Emulation *)emulation)->addEDL(pathString, connectionsMap);
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
	 
	 [self updateChangeCount:NSChangeDone];*/
}

- (void)removeDevice:(NSString *)deviceId
{
	/*	if (!emulation)
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
	 }
	 
	 [self lockEmulation];
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

@end
