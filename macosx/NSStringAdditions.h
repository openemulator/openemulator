
/**
 * OpenEmulator
 * Mac OS X NSString string/wstring conversion
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * NSString string/wstring conversion additions
 */

#import <Cocoa/Cocoa.h>

#import <string>

@interface NSString (NSStringAdditions)

+ (NSString *)stringWithCPPString:(const std::string&)s;
- (std::string)cppString;

+ (NSString *)stringWithCPPWString:(const std::wstring&)ws;
- (std::wstring)cppWString;

@end
