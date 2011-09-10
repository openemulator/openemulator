
/**
 * OpenEmulator
 * Mac OS X C++/Obj-C string conversion
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * C++/Obj-C string conversion helper
 */

#define getNSString(s) ([NSString stringWithUTF8String:(s).c_str()])
#define getCPPString(s) (string([(s) UTF8String]))
