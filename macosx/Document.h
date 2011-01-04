
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import <Cocoa/Cocoa.h>

#define USER_TEMPLATES_FOLDER @"~/Library/Application Support/OpenEmulator/Templates"

@class EmulationWindowController;

@interface Document : NSDocument
{
	void *emulation;
	
	EmulationWindowController *emulationWindowController;
}

- (void)showEmulation:(id)sender;

- (void)newEmulation:(NSURL *)url;
- (void)deleteEmulation;

- (id)initWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;
- (IBAction)saveDocumentAsTemplate:(id)sender;

- (void *)emulationInfo;

- (void)addEDL:(NSString *)path connections:(NSDictionary *)connections;
- (void)removeDevice:(NSString *)deviceId;

- (NSString *)valueOfProperty:(NSString *)name
				 forComponent:(NSString *)theId;
- (void)setValue:(NSString *)value
	  ofProperty:(NSString *)theName
	forComponent:(NSString *)theId;

- (BOOL)mount:(NSString *)path;
- (BOOL)mountable:(NSString *)path;

- (BOOL)mount:(NSString *)path inStorage:(void *)component;
- (BOOL)unmountStorage:(void *)component;
- (NSString *)pathOfImageInStorage:(void *)component;
- (BOOL)storageLocked:(void *)component;

@end
