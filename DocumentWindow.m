
/**
 * OpenEmulator
 * Mac OS X Document Window
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import "DocumentWindow.h"

@implementation DocumentWindow

- (BOOL)canBecomeKeyWindow
{
	return YES;
}

- (BOOL)isExcludedFromWindowsMenu
{
	return NO;
}

@end
