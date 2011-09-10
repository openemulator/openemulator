
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#import <Cocoa/Cocoa.h>

#define USER_TEMPLATES_FOLDER @"~/Library/Application Support/OpenEmulator/Templates"

@class EmulationWindowController;

@interface Document : NSDocument
{
    void *emulation;
    
    EmulationWindowController *emulationWindowController;
    NSMutableArray *canvasWindowControllers;
}

- (id)initWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;
- (IBAction)saveDocumentAsTemplate:(id)sender;
- (void)showEmulation:(id)sender;
- (void)showCanvas:(NSValue *)canvasValue;

- (void)constructEmulation:(NSURL *)url;
- (void)destroyEmulation;
- (void)lockEmulation;
- (void)unlockEmulation;
- (void *)emulation;

- (void)constructCanvas:(NSDictionary *)dict;
- (void)destroyCanvas:(NSValue *)canvasValue;

- (BOOL)canMountNow:(NSString *)path;
- (BOOL)mount:(NSString *)path;
- (BOOL)canMount:(NSString *)path;

@end
