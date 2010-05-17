
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

void c_oepaOpen();
void c_oepaClose();

int c_oepaStartPlayback(char *path);
void c_oepaStopPlayback();
int c_oepaIsPlayback();
float c_oepaGetPlaybackTime();
int c_oepaStartRecording(char *path);
void c_oepaStopRecording();
int c_oepaIsRecording();
float c_oepaGetRecordingTime();

#ifdef __CPLUSPLUS
}
#endif
