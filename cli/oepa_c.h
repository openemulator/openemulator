
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

int c_oepaStartPlayback(char *path);
float c_oepaGetPlaybackTime();
void c_oepaStopPlayback();
int c_oepaStartRecording(char *path);
float c_oepaGetRecordingTime();
void c_oepaStopRecording();

void c_oepaOpen();
void c_oepaClose();

#ifdef __CPLUSPLUS
}
#endif
