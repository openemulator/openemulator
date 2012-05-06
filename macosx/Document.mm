
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#import "Document.h"

#import "NSStringAdditions.h"

#import "DocumentController.h"

#import "EmulationWindowController.h"
#import "CanvasWindowController.h"

#import "CanvasWindow.h"
#import "CanvasPrintView.h"

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

OEComponent *constructCanvas(void *userData, OEComponent *device)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    OpenGLCanvas *canvas = new OpenGLCanvas([[[NSBundle mainBundle] resourcePath] cppString]);
    string label;
    device->postMessage(NULL, DEVICE_GET_LABEL, &label);
    
    Document *document = (Document *)userData;
    NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                          [NSValue valueWithPointer:device], @"device",
                          [NSString stringWithCPPString:label], @"label",
                          [NSValue valueWithPointer:canvas], @"canvas",
                          nil];
    
    if ([NSThread isMainThread])
        [document constructCanvas:dict];
    else
        [document performSelectorOnMainThread:@selector(constructCanvas:)
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
    self = [super init];
    
    if (self)
    {
        if ([self readFromURL:absoluteURL
                       ofType:nil
                        error:outError])
        {
            newCanvases = [[NSMutableArray alloc] init];
            
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
    [self destroyEmulation];
    
    [emulationWindowController release];
    [canvasWindowControllers release];
    
    [newCanvases release];
    
    [super dealloc];
}

// Document

- (BOOL)readFromURL:(NSURL *)absoluteURL
             ofType:(NSString *)typeName
              error:(NSError **)outError
{
    if (emulation)
    {
        [emulationWindowController close];
        
        [self destroyEmulation];
        
        [emulationWindowController release];
        emulationWindowController = nil;
        
        [canvasWindowControllers release];
        canvasWindowControllers = nil;
        
        DocumentController *documentController;
        documentController = [NSDocumentController sharedDocumentController];
        [documentController openDocumentWithContentsOfURL:absoluteURL
                                                  display:YES
                                                    error:outError];
        
        return YES;
    }
    
    emulation = [self constructEmulation:absoluteURL];
    
    if (emulation)
    {
        if (((OEEmulation *)emulation)->isOpen())
        {
            if (((OEEmulation *)emulation)->isActive())
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
        NSString *s = [[absoluteURL path] stringByAppendingString:@"/"];
        string emulationPath = [s cppString];
        
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
    [panel setAllowedFileTypes:[NSArray arrayWithObject:@"emulation"]];
    [panel setDirectoryURL:[NSURL fileURLWithPath:path]];
    [panel beginSheetModalForWindow:[NSApp mainWindow]
                  completionHandler:^(NSInteger returnCode)
     {
         if (returnCode != NSOKButton)
             return;
         
         NSError *error;
         if (![self writeToURL:[panel URL]
                        ofType:nil
                         error:&error])
             [[NSAlert alertWithError:error] runModal];
     }];
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

// Emulation

- (void)didUpdate:(id)sender
{
    if (emulation && ((OEEmulation *)emulation)->isActive())
        [self updateChangeCount:NSChangeDone];
    
    [emulationWindowController updateWindow:self];
}

- (void *)constructEmulation:(NSURL *)url
{
    if (!canvasWindowControllers)
        canvasWindowControllers = [[NSMutableArray alloc] init];
    
    DocumentController *documentController;
    documentController = [NSDocumentController sharedDocumentController];
    PAAudio *paAudio = (PAAudio *)[documentController paAudio];
    OEComponent *hidJoystick = (OEComponent *)[documentController hidJoystick];
    
    OEEmulation *theEmulation = new OEEmulation();
    
    theEmulation->setResourcePath([[[NSBundle mainBundle] resourcePath] cppString]);
    theEmulation->setConstructCanvas(constructCanvas);
    theEmulation->setDestroyCanvas(destroyCanvas);
    theEmulation->setUserData(self);
    
    theEmulation->addComponent("audio", paAudio);
    theEmulation->addComponent("joystick", hidJoystick);
    
    [self lockEmulation];
    
    if (theEmulation->open([[url path] cppString]))
        theEmulation->setDidUpdate(didUpdate);
    else
    {
        delete theEmulation;
        theEmulation = nil;
    }
    
    [self unlockEmulation];
    
    return theEmulation;
}

- (void)destroyEmulation
{
    if (!emulation)
        return;
    
    [self lockEmulation];
    
    delete (OEEmulation *)emulation;
    emulation = NULL;
    
    [self unlockEmulation];
}

- (void)lockEmulation
{
    DocumentController *documentController;
    documentController = [NSDocumentController sharedDocumentController];
    
    [documentController lockEmulation];
}

- (void)unlockEmulation
{
    DocumentController *documentController;
    documentController = [NSDocumentController sharedDocumentController];
    
    [documentController unlockEmulation];
}

- (void *)emulation
{
    return emulation;
}

// Window controllers

- (void)makeWindowControllers
{
    emulationWindowController = [[EmulationWindowController alloc] init];
    
    if ([canvasWindowControllers count])
    {
        CanvasWindowController *canvasWindowController;
        canvasWindowController = [canvasWindowControllers objectAtIndex:0];
        [self addWindowController:canvasWindowController];
        
        [canvasWindowController showWindow:self];
    }
    else
        [self addWindowController:emulationWindowController];
}

- (IBAction)showEmulation:(id)sender
{
    [self addWindowController:emulationWindowController];
    
    [emulationWindowController showWindow:self];
}

- (void)constructCanvas:(NSDictionary *)dict
{
    void *device = [[dict objectForKey:@"device"] pointerValue];
    NSString *label = [dict objectForKey:@"label"];
    NSValue *canvasValue = [dict objectForKey:@"canvas"];
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasValue pointerValue];
    
    CanvasWindowController *canvasWindowController;
    canvasWindowController = [[CanvasWindowController alloc] initWithDevice:device
                                                                      title:label
                                                                     canvas:canvas];
    [canvasWindowControllers addObject:canvasWindowController];
    [canvasWindowController release];
    
    if (newCanvasesCapture)
        [newCanvases addObject:canvasValue];
}

- (void)destroyCanvas:(NSValue *)canvasValue
{
    OpenGLCanvas *canvas = (OpenGLCanvas *)[canvasValue pointerValue];
    for (int i = 0; i < [canvasWindowControllers count]; i++)
    {
        CanvasWindowController *canvasWindowController;
        canvasWindowController = [canvasWindowControllers objectAtIndex:i];
        if ([canvasWindowController canvas] == canvas)
        {
            [canvasWindowController close];
            
            if ([[self windowControllers] containsObject:canvasWindowController])
                [self removeWindowController:canvasWindowController];
            [canvasWindowControllers removeObjectAtIndex:i];
            
            delete canvas;
            
            return;
        }
    }
}

- (void)showCanvas:(NSValue *)canvasValue
{
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

- (void)captureNewCanvases:(BOOL)value
{
    newCanvasesCapture = value;
    
    if (!value)
        [newCanvases removeAllObjects];
}

- (void)showNewCanvases
{
    for (int i = 0; i < [newCanvases count]; i++)
        [self showCanvas:[newCanvases objectAtIndex:i]];
}

// Storage

- (BOOL)canMountNow:(NSString *)path
{
    BOOL success = NO;
    
    [self lockEmulation];
    
    OEIds deviceIds = ((OEEmulation *)emulation)->getDeviceIds();
    for (OEIds::iterator i = deviceIds.begin();
         i != deviceIds.end();
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
                string thePath = [path cppString];
                if ((*j)->postMessage(NULL, STORAGE_CAN_MOUNT, &thePath))
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
    
    OEIds deviceIds = ((OEEmulation *)emulation)->getDeviceIds();
    for (OEIds::iterator i = deviceIds.begin();
         i != deviceIds.end();
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
                string thePath = [path cppString];
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

- (BOOL)canMount:(NSString *)path
{
    BOOL success = NO;
    
    [self lockEmulation];
    
    OEIds deviceIds = ((OEEmulation *)emulation)->getDeviceIds();
    for (OEIds::iterator i = deviceIds.begin();
         i != deviceIds.end();
         i++)
    {
        OEComponent *device = ((OEEmulation *)emulation)->getComponent(*i);
        OEComponents storages;
        
        device->postMessage(NULL, DEVICE_GET_STORAGES, &storages);
        for (OEComponents::iterator j = storages.begin(); 
             j != storages.end();
             j++)
        {
            string thePath = [path cppString];
            if ((*j)->postMessage(NULL, STORAGE_CAN_MOUNT, &thePath))
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

- (BOOL)validateUserInterfaceItem:(id)anItem
{
    SEL action = [anItem action];
    
    if (action == @selector(printDocument:))
    {
        NSWindow *window = [NSApp mainWindow];
        
        return [window isMemberOfClass:[CanvasWindow class]];
    }
    
    return YES;
}

- (void)printDocument:(id)sender
{
    CanvasWindow *canvasWindow = (CanvasWindow *)[NSApp mainWindow];
    
    if (![canvasWindow isMemberOfClass:[CanvasWindow class]])
         return;
    
    CanvasView *canvasView = [[canvasWindow windowController] canvasView];
    
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
    
    [op runOperationModalForWindow:canvasWindow
                          delegate:self
                    didRunSelector:NULL
                       contextInfo:NULL];
}

@end
