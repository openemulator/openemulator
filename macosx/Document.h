
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

- (void)lockEmulation;
- (void)unlockEmulation;

- (id)initWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;
- (IBAction)saveDocumentAsTemplate:(id)sender;

- (void *)emulationInfo;

- (BOOL)mount:(NSString *)path;
- (BOOL)mount:(NSString *)path inStorage:(void *)component;
- (BOOL)unmountStorage:(void *)component;
- (BOOL)isMountable:(NSString *)path;
- (BOOL)isMountable:(NSString *)path inStorage:(void *)component;
- (BOOL)isStorageMounted:(void *)component;
- (BOOL)isStorageWritable:(void *)component;
- (BOOL)isStorageLocked:(void *)component;
- (NSString *)getStoragePath:(void *)component;
- (NSString *)getStorageFormat:(void *)component;
- (NSString *)getStorageCapacity:(void *)component;

- (void)setValue:(NSString *)value
	  ofProperty:(NSString *)theName
	forComponent:(NSString *)theId;
- (NSString *)valueOfProperty:(NSString *)name
				 forComponent:(NSString *)theId;

- (void)addEDL:(NSString *)path connections:(NSDictionary *)connections;
- (void)removeDevice:(NSString *)deviceId;

@end
