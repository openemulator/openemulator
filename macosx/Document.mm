
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
#import "CanvasPrintView.h"
#import "StringConversion.h"

#import "OEEmulation.h"
#import "PAAudio.h"
#import "OpenGLCanvas.h"

#import "DeviceInterface.h"
#import "StorageInterface.h"

#import <sstream>

@implementation Document

// Callbacks

void didUpdate(void *userData)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	Document *document = (Document *)userData;
	
	[document performSelectorOnMainThread:@selector(didUpdate:)
							   withObject:nil
							waitUntilDone:NO];
	
	[pool drain];
}

OEComponent *createCanvas(void *userData, OEComponent *device)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	OpenGLCanvas *canvas = new OpenGLCanvas(getCPPString([[NSBundle mainBundle] resourcePath]));
	string label;
	device->postMessage(NULL, DEVICE_GET_LABEL, &label);
	
	Document *document = (Document *)userData;
	NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
						  [NSValue valueWithPointer:device], @"device",
						  getNSString(label), @"label",
						  [NSValue valueWithPointer:canvas], @"canvas",
						  nil];
	
	if ([NSThread isMainThread])
		[document createCanvas:dict];
	else
		[document performSelectorOnMainThread:@selector(createCanvas:)
								   withObject:dict
								waitUntilDone:NO];
	
	[pool drain];
	
	return canvas;
}

void destroyCanvas(void *userData, OEComponent *canvas)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	Document *document = (Document *)userData;
	NSValue *canvasValue = [NSValue valueWithPointer:canvas];
	
	if ([NSThread isMainThread])
		[document destroyCanvas:canvasValue];
	else
		[document performSelectorOnMainThread:@selector(destroyCanvas:)
								   withObject:canvasValue
								waitUntilDone:NO];
	
	[pool drain];
}

// Init/dealloc

- (id)initWithTemplateURL:(NSURL *)absoluteURL
					error:(NSError **)outError
{
	NSLog(@"Document init");
	
	if (self = [super init])
	{
		if ([self readFromURL:absoluteURL
					   ofType:nil
						error:outError])
		{
			return self;
		}
	}
	
	if (outError)
		*outError = [NSError errorWithDomain:NSCocoaErrorDomain
										code:NSFileReadUnknownError
									userInfo:nil];
	
	return nil;
}

- (void)dealloc
{
	NSLog(@"Document dealloc");
	
	[self destroyEmulation];
	
	[emulationWindowController release];
	[canvasWindowControllers release];
	
	[super dealloc];
}

// Document

- (NSWindow *)windowForSheet
{
	return [NSApp mainWindow];
}

- (BOOL)readFromURL:(NSURL *)absoluteURL
			 ofType:(NSString *)typeName
			  error:(NSError **)outError
{
	if (emulation)
		[self destroyEmulation];
	
	[self createEmulation:absoluteURL];
	
	OEEmulation *theEmulation = (OEEmulation *)emulation;
	if (theEmulation)
	{
		if (theEmulation->isOpen())
		{
			if (theEmulation->isActive())
				[self updateChangeCount:NSChangeDone];
			
			return YES;
		}
		
		[self destroyEmulation];
	}
	
	if (outError)
		*outError = [NSError errorWithDomain:NSCocoaErrorDomain
										code:NSFileReadUnknownError
									userInfo:nil];
	
	return NO;
}

- (BOOL)writeToURL:(NSURL *)absoluteURL
			ofType:(NSString *)typeName
			 error:(NSError **)outError
{
	OEEmulation *theEmulation = (OEEmulation *)emulation;
	if (theEmulation)
	{
		string emulationPath = getCPPString([[absoluteURL path]
											 stringByAppendingString:@"/"]);
		
		[self lockEmulation];
		bool isSaved = theEmulation->save(emulationPath);
		[self unlockEmulation];
		
		if (isSaved)
			return YES;
	}
	
	if (outError)
		*outError = [NSError errorWithDomain:NSCocoaErrorDomain
										code:NSFileWriteUnknownError
									userInfo:nil];
	
	return NO;
}

- (BOOL)saveToURL:(NSURL *)absoluteURL
		   ofType:(NSString *)typeName
 forSaveOperation:(NSSaveOperationType)saveOperation
			error:(NSError **)outError
{
	BOOL result = [super saveToURL:absoluteURL
							ofType:typeName
				  forSaveOperation:saveOperation
							 error:outError];
	
	OEEmulation *theEmulation = (OEEmulation *)emulation;
	if (theEmulation->isActive())
		[self updateChangeCount:NSChangeDone];
	
	return result;
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
	NSLog(@"Document makeWindowControllers");
	
	emulationWindowController = [[EmulationWindowController alloc] init];
	
	if ([canvasWindowControllers count])
	{
		CanvasWindowController *canvasWindowController;
		canvasWindowController = [canvasWindowControllers objectAtIndex:0];
		[self addWindowController:canvasWindowController];
	}
	else
		[self addWindowController:emulationWindowController];
}

- (IBAction)showEmulation:(id)sender
{
	if (![[self windowControllers] containsObject:emulationWindowController])
		[self addWindowController:emulationWindowController];
	
	[emulationWindowController showWindow:self];
}

- (void)didUpdate:(id)sender
{
	if (emulation && ((OEEmulation *)emulation)->isActive())
		[self updateChangeCount:NSChangeDone];
	
	[emulationWindowController updateEmulation:self];
}

- (void)createCanvas:(NSDictionary *)dict
{
	NSLog(@"Document createCanvas");
	
	void *device = [[dict objectForKey:@"device"] pointerValue];
	NSString *label = [dict objectForKey:@"label"];
	OpenGLCanvas *canvas = (OpenGLCanvas *)[[dict objectForKey:@"canvas"]
											pointerValue];
	
	CanvasWindowController *canvasWindowController;
	canvasWindowController = [[CanvasWindowController alloc] initWithDevice:device
																	  title:label
																	 canvas:canvas];
	[canvasWindowControllers addObject:canvasWindowController];
	[canvasWindowController release];
	
	[emulationWindowController updateEmulation:self];
}

- (void)destroyCanvas:(NSValue *)canvasValue
{
	NSLog(@"Document destroyCanvas");
	
	OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasValue pointerValue];
	for (int i = 0; i < [canvasWindowControllers count]; i++)
	{
		CanvasWindowController *canvasWindowController;
		canvasWindowController = [canvasWindowControllers objectAtIndex:i];
		if ([canvasWindowController canvas] == canvas)
		{
			[canvasWindowController closeWindow];
			
			[self removeWindowController:canvasWindowController];
			[canvasWindowControllers removeObjectAtIndex:i];
			
			[emulationWindowController updateEmulation:self];
			
			delete canvas;
			
			break;
		}
	}
}

- (void)showCanvas:(NSValue *)canvasValue
{
	NSLog(@"Document showCanvas");
	
	void *canvas = [canvasValue pointerValue];
	for (int i = 0; i < [canvasWindowControllers count]; i++)
	{
		CanvasWindowController *canvasWindowController;
		canvasWindowController = [canvasWindowControllers objectAtIndex:i];
		if ([canvasWindowController canvas] == canvas)
		{
			if (![[self windowControllers] containsObject:canvasWindowController])
				[self addWindowController:canvasWindowController];
			
			[canvasWindowController showWindow:self];
			
			break;
		}
	}
}

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem >)anItem
{
	SEL action = [anItem action];
	
	if (action == @selector(runPageLayout:))
		return ([[NSApp mainWindow] windowController] !=
				emulationWindowController);
	else if (action == @selector(printDocument:))
		return ([[NSApp mainWindow] windowController] !=
				emulationWindowController);
	
	return YES;
}

- (void)printDocument:(id)sender
{
	CanvasWindowController *windowController = [[NSApp mainWindow] windowController];
	if (![windowController respondsToSelector:@selector(canvasView)])
		return;
	
	CanvasView *canvasView = [windowController canvasView];
	CanvasPrintView *view = [[[CanvasPrintView alloc] initWithCanvasView:canvasView]
							 autorelease];
	
	NSPrintOperation *op = [NSPrintOperation printOperationWithView:view];
	NSPrintInfo *printInfo = [op printInfo];
	
	if ([canvasView isPaperCanvas])
	{
		[printInfo setHorizontalPagination:NSFitPagination];
		[printInfo setHorizontallyCentered:NO];
		[printInfo setVerticallyCentered:NO];
		[printInfo setTopMargin:0.0 * 72.0];
		[printInfo setRightMargin:0.0 * 72.0];
		[printInfo setBottomMargin:0.0 * 72.0];
		[printInfo setLeftMargin:0.0 * 72.0];
	}
	else
	{
		[printInfo setHorizontalPagination:NSFitPagination];
		[printInfo setVerticalPagination:NSFitPagination];
		[printInfo setTopMargin:0.5 * 72.0];
		[printInfo setRightMargin:0.5 * 72.0];
		[printInfo setBottomMargin:0.5 * 72.0];
		[printInfo setLeftMargin:0.5 * 72.0];
	}
	
	NSPrintPanel *panel = [op printPanel];
	[panel setOptions:([panel options] |
					   NSPrintPanelShowsPaperSize |
					   NSPrintPanelShowsOrientation)];
	
	[op runOperationModalForWindow:[NSApp mainWindow]
						  delegate:self
					didRunSelector:NULL
					   contextInfo:NULL];
}

- (void)createEmulation:(NSURL *)url
{
	NSLog(@"Document createEmulation");
	
	if (!canvasWindowControllers)
		canvasWindowControllers = [[NSMutableArray alloc] init];
	
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PAAudio *paAudio = (PAAudio *)[documentController paAudio];
	
	OEEmulation *theEmulation = new OEEmulation();
	
	theEmulation->setResourcePath(getCPPString([[NSBundle mainBundle] resourcePath]));
	theEmulation->setCreateCanvas(createCanvas);
	theEmulation->setDestroyCanvas(destroyCanvas);
	theEmulation->setUserData(self);
	
	theEmulation->addComponent("audio", paAudio);
	
	[self lockEmulation];
	theEmulation->open(getCPPString([url path]));
	theEmulation->setDidUpdate(didUpdate);
	paAudio->addEmulation(theEmulation);
	[self unlockEmulation];
	
	emulation = theEmulation;
}

- (void)destroyEmulation
{
	NSLog(@"Document destroyEmulation");
	
	if (!emulation)
		return;
	
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PAAudio *paAudio = (PAAudio *)[documentController paAudio];
	
	[self lockEmulation];
	OEEmulation *theEmulation = (OEEmulation *)emulation;
	paAudio->removeEmulation(theEmulation);
	delete theEmulation;
	[self unlockEmulation];
	
	emulation = nil;
}

- (void)lockEmulation
{
//	NSLog(@"Document lockEmulation");
	
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PAAudio *paAudio = (PAAudio *)[documentController paAudio];
	
	paAudio->lock();
}

- (void)unlockEmulation
{
//	NSLog(@"Document unlockEmulation");
	
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PAAudio *paAudio = (PAAudio *)[documentController paAudio];
	
	paAudio->unlock();
}

- (void *)emulation
{
	return emulation;
}

// Storage

- (BOOL)isMountable:(NSString *)path
{
	BOOL success = NO;
	
	[self lockEmulation];
	
	OEIds *devices = (OEIds *)((OEEmulation *)emulation)->getDevices();
	for (OEIds::iterator i = devices->begin();
		 i != devices->end();
		 i++)
	{
		OEComponent *device = ((OEEmulation *)emulation)->getComponent(*i);
		OEComponents storages;
		
		device->postMessage(NULL, DEVICE_GET_STORAGES, &storages);
		for (OEComponents::iterator j = storages.begin(); 
			 j != storages.end();
			 j++)
		{
			if ((*j)->postMessage(NULL, STORAGE_IS_AVAILABLE, NULL))
			{
				string thePath = getCPPString(path);
				if ((*j)->postMessage(NULL, STORAGE_TESTMOUNT, &thePath))
				{
					[self updateChangeCount:NSChangeDone];
					
					success = YES;
					break;
				}
			}
		}
		
		if (success)
			break;
	}
	
	[self unlockEmulation];
	
	return success;
}

- (BOOL)mount:(NSString *)path
{
	BOOL success = NO;
	
	[self lockEmulation];
	
	OEIds *devices = (OEIds *)((OEEmulation *)emulation)->getDevices();
	for (OEIds::iterator i = devices->begin();
		 i != devices->end();
		 i++)
	{
		OEComponent *device = ((OEEmulation *)emulation)->getComponent(*i);
		OEComponents storages;
		
		device->postMessage(NULL, DEVICE_GET_STORAGES, &storages);
		for (OEComponents::iterator j = storages.begin(); 
			 j != storages.end();
			 j++)
		{
			if ((*j)->postMessage(NULL, STORAGE_IS_AVAILABLE, NULL))
			{
				string thePath = getCPPString(path);
				if ((*j)->postMessage(NULL, STORAGE_MOUNT, &thePath))
				{
					[self updateChangeCount:NSChangeDone];
					
					success = YES;
					break;
				}
			}
		}
		
		if (success)
			break;
	}
	
	[self unlockEmulation];
	
	return success;
}

- (BOOL)testMount:(NSString *)path
{
	BOOL success = NO;
	
	[self lockEmulation];
	
	OEIds *devices = (OEIds *)((OEEmulation *)emulation)->getDevices();
	for (OEIds::iterator i = devices->begin();
		 i != devices->end();
		 i++)
	{
		OEComponent *device = ((OEEmulation *)emulation)->getComponent(*i);
		OEComponents storages;
		
		device->postMessage(NULL, DEVICE_GET_STORAGES, &storages);
		for (OEComponents::iterator j = storages.begin(); 
			 j != storages.end();
			 j++)
		{
			string thePath = getCPPString(path);
			if ((*j)->postMessage(NULL, STORAGE_TESTMOUNT, NULL))
			{
				success = YES;
				break;
			}
		}
		
		if (success)
			break;
	}
	
	[self unlockEmulation];
	
	return success;
}

- (void)sendSystemEvent:(DocumentSystemEvent)event toDevice:(void *)device
{
	if (!device)
		return;
	
	int message;
	
	switch (event)
	{
		case DOCUMENT_POWERDOWN:
			message = DEVICE_POWERDOWN;
			break;
		case DOCUMENT_SLEEP:
			message = DEVICE_SLEEP;
			break;
		case DOCUMENT_WAKEUP:
			message = DEVICE_WAKEUP;
			break;
		case DOCUMENT_COLDRESTART:
			message = DEVICE_COLDRESTART;
			break;
		case DOCUMENT_WARMRESTART:
			message = DEVICE_WARMRESTART;
			break;
		case DOCUMENT_DEBUGGERBREAK:
			message = DEVICE_DEBUGGERBREAK;
			break;
		default:
			return;
	}
	
	[self lockEmulation];
	
	((OEComponent *)device)->postMessage(NULL, message, NULL);
	
	[self unlockEmulation];
}

@end
