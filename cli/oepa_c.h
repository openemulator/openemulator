
/**
 * OpenEmulator
 * OpenEmulator/portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/portaudio C interface.
 */


#ifdef __CPLUSPLUS
extern "C"
{
#endif

void c_oepaSetFullDuplex(int value);

void c_oepaSetVolume(float value);

int c_oepaStartRecord(char *path);
void C_oepaStopRecord();
int c_oepaStartPlayback(char *path);
void c_oepaStopPlayback();
	
	
void c_oepaOpen();
void c_oepaClose();

#ifdef __CPLUSPLUS
}
#endif
