
/**
 * OpenEmulator
 * Mac OS X NSString string/wstring conversion
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * NSString string/wstring conversion additions
 */

#include "NSStringAdditions.h"

@implementation NSString (NSStringAdditions)

#if TARGET_RT_BIG_ENDIAN
const NSStringEncoding kEncoding_wchar_t = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32BE);
#else
const NSStringEncoding kEncoding_wchar_t = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
#endif

+ (NSString *)stringWithCPPString:(const std::string&)s
{
    return [NSString stringWithUTF8String:s.c_str()];
}

-(std::string)cppString
{
    return std::string([self UTF8String]);
}

+ (NSString *)stringWithCPPWString:(const std::wstring&)ws
{
    char *data = (char *)ws.data();
    size_t size = ws.size() * sizeof(wchar_t);
    
    NSString *result = [[[NSString alloc] initWithBytes:data length:size
                                               encoding:kEncoding_wchar_t] autorelease];
    return result;
}

-(std::wstring)cppWString
{
    NSData *asData = [self dataUsingEncoding:kEncoding_wchar_t];
    return std::wstring((wchar_t *)[asData bytes], [asData length] / sizeof(wchar_t));
}

@end
