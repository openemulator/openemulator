
/**
 * OpenEmulator
 * OpenEmulator/portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/portaudio interface.
 */

#include "OEEmulation.h"
#include "HostAudio.h"

#define OEPA_SAMPLERATE			48000.0
#define OEPA_CHANNELNUM			2
#define OEPA_FRAMESPERBUFFER	512
#define OEPA_BUFFERNUM			2

#define OEPA_VOLUMEFILTERFREQ	20.0

void oepaSetFullDuplex(bool value);
void oepaSetSampleRate(double value);
void oepaSetChannelNum(int value);
void oepaSetFramesPerBuffer(int value);
void oepaSetBufferNum(int value);

void oepaSetVolume(float value);

bool oepaStartPlayback(string path);
void oepaStopPlayback();
bool oepaIsPlayback();
float oepaGetPlaybackTime();
bool oepaStartRecording(string path);
void oepaStopRecording();
bool oepaIsRecording();
float oepaGetRecordingTime();

void oepaOpen();
void oepaClose();

OEEmulation *oepaConstruct(string path, string resourcePath);
void oepaDestroy(OEEmulation *emulation);

bool oepaIsLoaded(OEEmulation *emulation);
bool oepaSave(OEEmulation *emulation, string path);
int oepaIoctl(OEEmulation *emulation, string ref, int message, void *data);

xmlDocPtr oepaGetDML(OEEmulation *emulation);
bool oepaAddDevices(OEEmulation *emulation,
					string path,
					OEStringRefMap connections);
bool oepaIsDeviceTerminal(OEEmulation *emulation, OERef ref);
bool oepaRemoveDevice(OEEmulation *emulation, OERef ref);
