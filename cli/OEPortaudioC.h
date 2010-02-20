
/**
 * OpenEmulator
 * OpenEmulator-portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator-portaudio common interface.
 */

#ifdef __CPLUSPLUS
extern "C"
{
#endif

void c_oepaSetFullDuplex(int value);
void c_oepaOpen();
void c_oepaClose();

#ifdef __CPLUSPLUS
}
#endif
