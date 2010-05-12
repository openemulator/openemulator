
/**
 * OpenEmulator
 * OpenEmulator/portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/portaudio interface.
 */

#include <math.h>
#include <iostream>

#include <pthread.h>

#include "portaudio.h"
#include "sndfile.h"
#include "samplerate.h"

#include "oepa.h"

// Configuration
bool oepaFullDuplex = false;
double oepaSampleRate = OEPA_SAMPLERATE;
int oepaChannelNum = OEPA_CHANNELNUM;
int oepaFramesPerBuffer = OEPA_FRAMESPERBUFFER;
int oepaBufferNum = OEPA_BUFFERNUM;

// Audio
bool oepaAudioOpen = false;
volatile int oepaAudioBufferIndex = 0;
volatile int oepaEmulationsBufferIndex = 0;
vector<float> oepaInputBuffer;
vector<float> oepaOutputBuffer;

PaStream *oepaAudioStream = NULL;
float oepaVolume = 1.0F;
float oepaInstantVolume = 1.0F;
float oepaVolumeAlpha = 0.0F;
bool oepaTimerThreadOpen = false;
pthread_t oepaTimerThread;

// Emulations
bool oepaEmulationsOpen = false;
pthread_t oepaEmulationsThread;
pthread_mutex_t oepaEmulationsMutex;
vector<OEEmulation *> oepaEmulations;

// Audio playback
bool oepaPlayback;
long long oepaPlaybackFrameNum;
SNDFILE *oepaPlaybackFile;
int oepaPlaybackChannelNum;
int oepaPlaybackSRCRatio;
SRC_STATE *oepaPlaybackSRC;
bool oepaPlaybackEndOfInput;
int oepaPlaybackBufferFramesAvailable;
vector<float> oepaPlaybackBuffer;

// Audio recording
bool oepaRecording;
long long oepaRecordingFrameNum;
SNDFILE *oepaRecordingFile;

//
// Audio
//
static int oepaCallbackAudio(const void *inputBuffer,
							 void *outputBuffer,
							 unsigned long framesPerBuffer,
							 const PaStreamCallbackTimeInfo* timeInfo,
							 PaStreamCallbackFlags statusFlags,
							 void *userData)
{
	int nextIndex = (oepaAudioBufferIndex + 1) % oepaBufferNum;
	
	int sampleNum = oepaFramesPerBuffer * oepaChannelNum;
	int sampleIndex = nextIndex * sampleNum;
	
	if (nextIndex == oepaEmulationsBufferIndex)
	{
		float *out = (float *) outputBuffer;
		for (int i = 0; i < sampleNum; i++)
			*out++ = rand() * (0.1 / RAND_MAX);
		
		return paContinue;
	}
	
	if (inputBuffer)
		memcpy(&oepaInputBuffer[sampleIndex],
			   inputBuffer, sampleNum * sizeof(float));
	else
		memset(&oepaInputBuffer[sampleIndex],
			   0, sampleNum * sizeof(float));
	
	float *in = &oepaOutputBuffer[sampleIndex];
	float *out = (float *) outputBuffer;
	for (int i = 0; i < oepaFramesPerBuffer; i++)
	{
		for (int c = 0; c < oepaChannelNum; c++)
			*out++ = *in++ * oepaInstantVolume;
		
		oepaInstantVolume += (oepaVolume - oepaInstantVolume) * oepaVolumeAlpha;
	}
	
	oepaAudioBufferIndex = nextIndex;
	
	return paContinue;
}

void *oepaRunTimer(void *arg)
{
	while (oepaTimerThreadOpen)
	{
		usleep(1E6 * oepaFramesPerBuffer / oepaSampleRate);
		
		int nextIndex = (oepaAudioBufferIndex + 1) % oepaBufferNum;
		
		if (oepaAudioBufferIndex == nextIndex)
			continue;
		
		oepaAudioBufferIndex = nextIndex;
	}
	
	return NULL;
}

void oepaPlaybackAudio(float *inputBuffer, int frameNum)
{
	vector<float> srcBuffer;
	srcBuffer.resize(frameNum * oepaChannelNum);
	
	int bufferFrameNum = oepaPlaybackBuffer.size() / oepaPlaybackChannelNum;
	
	do
	{
		if (!oepaPlaybackBufferFramesAvailable)
		{
			int frameNumRead = sf_readf_float(oepaPlaybackFile,
											  &oepaPlaybackBuffer[0],
											  bufferFrameNum);
			
			oepaPlaybackBufferFramesAvailable = frameNumRead;
			if (frameNumRead != bufferFrameNum);
				oepaPlaybackEndOfInput = true;
		}
		
		SRC_DATA srcData =
		{
			&oepaPlaybackBuffer[bufferFrameNum -
								oepaPlaybackBufferFramesAvailable *
								oepaPlaybackChannelNum],
			&srcBuffer[0],
			oepaPlaybackBufferFramesAvailable,
			frameNum,
			0,
			0,
			oepaPlaybackEndOfInput,
			oepaPlaybackSRCRatio,
		};
		
		src_process(oepaPlaybackSRC, &srcData);
		
		float *in = &srcBuffer[0];
		for (int i = 0; i < srcData.output_frames_gen; i++)
		{
			for (int ch = 0; ch < oepaChannelNum; ch++)
				inputBuffer[ch] += in[ch % oepaChannelNum];
			
			in += oepaPlaybackChannelNum;
			inputBuffer += oepaChannelNum;
		}
		
		oepaPlaybackBufferFramesAvailable -= srcData.input_frames_used;
		frameNum -= srcData.output_frames_gen;
	} while (frameNum > 0);
	
	if (oepaPlaybackEndOfInput && !frameNum)
	{
		sf_close(oepaPlaybackFile);
		src_delete(oepaPlaybackSRC);
		oepaPlayback = false;
		
		return;
	}
}

void oepaRecordAudio(float *outputBuffer, int frameNum)
{
	int writeFrameNum = sf_writef_float(oepaRecordingFile,
										outputBuffer,
										frameNum);
	oepaRecordingFrameNum += frameNum;
	
	if (writeFrameNum != frameNum)
	{
		sf_close(oepaRecordingFile);
		oepaRecording = false;
	}
}

void *oepaRunEmulations(void *arg)
{
	vector<float> inputBuffer;
	vector<float> outputBuffer;
	int framesPerBuffer = 0;
	int channelNum = 0;
	int bufferSampleNum = 0;
	int bufferByteNum = 0;
	
	while (oepaEmulationsOpen)
	{
		if (oepaAudioBufferIndex == oepaEmulationsBufferIndex)
		{
			usleep(1000);
			continue;
		}
		
		pthread_mutex_lock(&oepaEmulationsMutex);
		
		if ((oepaFramesPerBuffer != framesPerBuffer) ||
			(oepaChannelNum != channelNum))
		{
			framesPerBuffer = oepaFramesPerBuffer;
			channelNum = oepaChannelNum;
			bufferSampleNum = framesPerBuffer * channelNum;
			bufferByteNum = bufferSampleNum * sizeof(float);
			
			inputBuffer.resize(2 * bufferSampleNum);
			memset(&inputBuffer[0], 0,
				   2 * bufferByteNum);
			outputBuffer.resize(2 * bufferSampleNum);
			memset(&outputBuffer[0], 0,
				   2 * bufferByteNum);
		}
		
		int bufferIndex = oepaEmulationsBufferIndex * bufferSampleNum;
		memcpy(&inputBuffer[0],
			   &inputBuffer[bufferSampleNum],
			   bufferByteNum);
		memcpy(&inputBuffer[bufferSampleNum],
			   &oepaInputBuffer[bufferIndex],
			   bufferByteNum);
		
		if (oepaPlayback)
			oepaPlaybackAudio(&inputBuffer[bufferIndex], oepaFramesPerBuffer);
		
		HostAudioBuffer buffer =
		{
			oepaSampleRate,
			oepaChannelNum,
			oepaFramesPerBuffer,
			&inputBuffer[0],
			&outputBuffer[0],
		};
		
		for (vector<OEEmulation *>::iterator i = oepaEmulations.begin();
			 i != oepaEmulations.end();
			 i++)
			(*i)->ioctl("host::audio", HOSTAUDIO_RENDERBUFFER, &buffer);
		
		if (oepaRecording)
			oepaRecordAudio(&outputBuffer[0], oepaFramesPerBuffer);
		
		memcpy(&oepaOutputBuffer[bufferIndex],
			   &outputBuffer[0],
			   bufferByteNum);
		memcpy(&outputBuffer[0],
			   &outputBuffer[bufferSampleNum],
			   bufferByteNum);
		
		pthread_mutex_unlock(&oepaEmulationsMutex);
		
		oepaEmulationsBufferIndex = (oepaEmulationsBufferIndex + 1) % oepaBufferNum;
	}
	
	return NULL;
}

void oepaCloseAudio()
{
	if (!oepaAudioOpen)
		return;
	
	if (oepaAudioStream)
	{
		Pa_StopStream(oepaAudioStream);
		Pa_CloseStream(oepaAudioStream);
		oepaAudioStream = NULL;
	}
	
	if (oepaTimerThreadOpen)
	{
		oepaTimerThreadOpen = false;
		
		void *status;
		pthread_join(oepaTimerThread, &status);
	}
	
	oepaAudioOpen = false;
}

bool oepaOpenAudio()
{
	if (oepaAudioOpen)
		oepaCloseAudio();
	
	oepaAudioBufferIndex = 0;
	
	float t = 1.0 / oepaSampleRate;
	float rc = 1 / 2.0 / M_PI / OEPA_VOLUMEFILTERFREQ;
	oepaVolumeAlpha = t / (rc + t);
	oepaInstantVolume = oepaVolume;
	
	int status = Pa_Initialize();
	if (status == paNoError)
	{
		status = Pa_OpenDefaultStream(&oepaAudioStream,
									  oepaFullDuplex ? oepaChannelNum : 0,
									  oepaChannelNum,
									  paFloat32,
									  oepaSampleRate,
									  oepaFramesPerBuffer,
									  oepaCallbackAudio,
									  NULL);
		if ((status != paNoError) && oepaFullDuplex)
		{
			cerr <<  "oepa: couldn't start audio stream, error " << status << 
				", attempting half-duplex\n";
			
			status = Pa_OpenDefaultStream(&oepaAudioStream,
										  0,
										  oepaChannelNum,
										  paFloat32,
										  oepaSampleRate,
										  oepaFramesPerBuffer,
										  oepaCallbackAudio,
										  NULL);
		}
			
		if (status == paNoError)
		{
			status = Pa_StartStream(oepaAudioStream);
			if (status == paNoError)
			{
				oepaAudioOpen = true;
				return true;
			}
			else
				cerr <<  "oepa: couldn't start audio stream, error " << status <<
				"\n";
			
			Pa_CloseStream(oepaAudioStream);
		}
		else
			cerr <<  "oepa: couldn't open audio stream, error " << status << "\n";
	}
	else
		cerr <<  "oepa: couldn't init portaudio, error " << status << "\n";
	
	int error;
	pthread_attr_t attr;
	error = pthread_attr_init(&attr);
	if (!error)
	{
		error = pthread_attr_setdetachstate(&attr,
											PTHREAD_CREATE_JOINABLE);
		if (!error)
		{
			oepaTimerThreadOpen = true;
			error = pthread_create(&oepaTimerThread,
								   &attr,
								   oepaRunTimer,
								   NULL);
			if (!error)
			{
				oepaAudioOpen = true;
				return true;
			}
			else
				cerr << "oepa: couldn't create timer thread, error " << error <<
				"\n";
		}
		else
			cerr << "oepa: couldn't attr timer thread, error " << error << "\n";
	}
	else
		cerr << "oepa: couldn't init timer thread, error " << error << "\n";
	
	return false;
}

//
// Emulations
//
void oepaCloseEmulations()
{
	if (!oepaEmulationsOpen)
		return;
	
	oepaEmulationsOpen = false;
	void *status;
	pthread_join(oepaEmulationsThread, &status);
	
	pthread_mutex_destroy(&oepaEmulationsMutex);
}

bool oepaOpenEmulations()
{
	int error;
	
	int bufferSize = oepaBufferNum * oepaFramesPerBuffer * oepaChannelNum;
	oepaInputBuffer.resize(bufferSize);
	oepaOutputBuffer.resize(bufferSize);
	
	oepaEmulationsBufferIndex = 0;
	oepaPlayback = false;
	oepaRecording = false;
	
	error = pthread_mutex_init(&oepaEmulationsMutex, NULL);
	if (!error)
	{
		pthread_attr_t attr;
		error = pthread_attr_init(&attr);
		if (!error)
		{
			sched_param param;
			
			error = pthread_attr_getschedparam(&attr, &param);
			if (!error)
			{
				param.sched_priority *= 1.5;
				pthread_attr_setschedparam(&attr, &param);
			}
			
			error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
			if (!error)
			{
				oepaEmulationsOpen = true;
				error = pthread_create(&oepaEmulationsThread,
									   &attr,
									   oepaRunEmulations,
									   NULL);
				if (!error)
					return true;
				else
					cerr << "oepa: couldn't create emulations thread, error " <<
					error << "\n";
			}
			else
				cerr << "oepa: couldn't attr emulations thread, error " <<
				error << "\n";
		}
		else
			cerr << "oepa: couldn't init emulations attr, error " <<
			error << "\n";		
	}
	else
		cerr << "oepa: couldn't init emulations mutex, error " <<
		error << "\n";
	
	return false;
}

void oepaLockEmulations()
{
	if (oepaEmulationsOpen)
		pthread_mutex_lock(&oepaEmulationsMutex);
}

void oepaUnlockEmulations()
{
	if (oepaEmulationsOpen)
		pthread_mutex_unlock(&oepaEmulationsMutex);
}

bool oepaDisableAudio()
{
	bool state = oepaAudioOpen;
	
	if (state)
		oepaCloseAudio();
	
	oepaLockEmulations();
	
	return state;
}

void oepaEnableAudio(bool state)
{
	oepaUnlockEmulations();
	
	if (state)
		oepaOpenAudio();
}

//
// Interface
//
void oepaSetFullDuplex(bool value)
{
	bool state = oepaDisableAudio();
	
	oepaFullDuplex = value;
	
	oepaEnableAudio(state);
}

void oepaSetSampleRate(double value)
{
	bool state = oepaDisableAudio();
	
	oepaStopPlayback();
	oepaStopRecording();
	
	oepaSampleRate = value;
	
	oepaEnableAudio(state);
}

void oepaSetChannelNum(int value)
{
	bool state = oepaDisableAudio();
	
	oepaStopPlayback();
	oepaStopRecording();
	
	oepaChannelNum = value;
	
	oepaEnableAudio(state);
}

void oepaSetFramesPerBuffer(int value)
{
	bool state = oepaDisableAudio();
	
	oepaStopPlayback();
	oepaStopRecording();
	
	oepaFramesPerBuffer = value;
	
	oepaEnableAudio(state);
}

void oepaSetBufferNum(int value)
{
	bool state = oepaDisableAudio();
	
	oepaBufferNum = value + 1;
	
	oepaEnableAudio(state);
}

void oepaSetVolume(float value)
{
	oepaVolume = value;
}

void oepaStopPlayback()
{
	if (!oepaPlaybackFile)
		return;
	
	oepaLockEmulations();
	
	sf_close(oepaPlaybackFile);
	oepaPlayback = false;
	
	oepaUnlockEmulations();
}

bool oepaStartPlayback(string path)
{
	oepaStopPlayback();
	
	SF_INFO sfInfo = 
	{
		0,
		0,
		0,
		0,
		0,
		0,
	};
	
	oepaLockEmulations();
	
	oepaPlaybackFrameNum = 0;
	oepaPlaybackFile = sf_open(path.c_str(), SFM_READ, &sfInfo);
	if (oepaPlaybackFile)
	{
		oepaPlaybackChannelNum = sfInfo.channels;
		
		int error;
		oepaPlaybackSRCRatio = (double) oepaSampleRate / sfInfo.samplerate;
		oepaPlaybackSRC = src_new(SRC_SINC_FASTEST,
								  sfInfo.channels,
								  &error);
		
		if (oepaPlaybackSRC)
		{
			oepaPlaybackEndOfInput = false;
			oepaPlaybackBufferFramesAvailable = 0;
			oepaPlaybackBuffer.resize(oepaFramesPerBuffer * oepaPlaybackChannelNum * 
									 oepaPlaybackSRCRatio * 2);
			
			oepaPlayback = true;
		}
		else
		{
			cerr << "oepa: couldn't init sample rate converter, error " << error << "\n";
			sf_close(oepaPlaybackFile);
		}
	}
	
	oepaUnlockEmulations();
	
	return oepaPlayback;
}

float oepaGetPlaybackTime()
{
	if (!oepaRecording)
		return -1.0;
	
	return (float) oepaPlaybackFrameNum / oepaSampleRate;
}

void oepaStopRecording()
{
	if (!oepaRecording)
		return;
	
	oepaLockEmulations();
	
	sf_close(oepaRecordingFile);
	oepaRecording = false;
	
	oepaUnlockEmulations();
}

bool oepaStartRecording(string path)
{
	oepaStopRecording();
	
	SF_INFO sfInfo = 
	{
		0,
		oepaSampleRate,
		oepaChannelNum,
		SF_FORMAT_WAV,
		0,
		0,
	};
	
	oepaLockEmulations();
	
	oepaRecordingFrameNum = 0;
	oepaRecordingFile = sf_open(path.c_str(), SFM_WRITE, &sfInfo);
	
	if (oepaRecordingFile)
		oepaRecording = true;
	
	oepaUnlockEmulations();
	
	return oepaRecording;
}

float oepaGetRecordingTime()
{
	if (!oepaRecording)
		return -1.0;
	
	return (float) oepaRecordingFrameNum / oepaSampleRate;
}

void oepaOpen()
{
	oepaOpenEmulations();
	oepaOpenAudio();
}

void oepaClose()
{
	oepaCloseAudio();
	oepaCloseEmulations();
}

OEEmulation *oepaConstruct(string path, string resourcePath)
{
	OEEmulation *emulation = new OEEmulation(path, resourcePath);
	if (emulation)
	{
		pthread_mutex_lock(&oepaEmulationsMutex);
		oepaEmulations.push_back(emulation);
		pthread_mutex_unlock(&oepaEmulationsMutex);
	}
	
	return emulation;
}

void oepaDestroy(OEEmulation *emulation)
{
	if (!emulation)
		return;
	
	pthread_mutex_lock(&oepaEmulationsMutex);
	
	for (vector<OEEmulation *>::iterator i = oepaEmulations.begin();
		 i != oepaEmulations.end();
		 i++)
	{
		if ((*i) == emulation)
		{
			oepaEmulations.erase(i);
			break;
		}
	}
	
	pthread_mutex_unlock(&oepaEmulationsMutex);
	
	delete emulation;
}

bool oepaIsLoaded(OEEmulation *emulation)
{
	return emulation->isLoaded();
}

bool oepaSave(OEEmulation *emulation, string path)
{
	pthread_mutex_lock(&oepaEmulationsMutex);
	bool status = emulation->save(path);
	pthread_mutex_unlock(&oepaEmulationsMutex);
	
	return status;
}

int oepaIoctl(OEEmulation *emulation,
			  string device, int message, void *data)
{
	pthread_mutex_lock(&oepaEmulationsMutex);
	int status = emulation->ioctl(device, message, data);
	pthread_mutex_unlock(&oepaEmulationsMutex);
	
	return status;
}

xmlDocPtr oepaGetDML(OEEmulation *emulation)
{
	return emulation->getDML();
}

bool oepaAddDevices(OEEmulation *emulation, string path, OEStringRefMap connections)
{
	pthread_mutex_lock(&oepaEmulationsMutex);
	bool status = emulation->addDevices(path, connections);
	pthread_mutex_unlock(&oepaEmulationsMutex);
	
	return status;
}

bool oepaIsDeviceTerminal(OEEmulation *emulation, OERef ref)
{
	pthread_mutex_lock(&oepaEmulationsMutex);
	bool status = emulation->isDeviceTerminal(ref);
	pthread_mutex_unlock(&oepaEmulationsMutex);
	
	return status;
}

bool oepaRemoveDevice(OEEmulation *emulation, OERef ref)
{
	pthread_mutex_lock(&oepaEmulationsMutex);
	bool status = emulation->removeDevice(ref);
	pthread_mutex_unlock(&oepaEmulationsMutex);
	
	return status;
}

extern "C" void c_oepaSetFullDuplex(int value)
{
	oepaSetFullDuplex(value);
}

extern "C" void c_oepaSetVolume(float value)
{
	oepaSetVolume(value);
}

extern "C" int c_oepaStartPlayback(char *path)
{
	return oepaStartPlayback(path);
}

extern "C" float c_oepaGetPlaybackTime()
{
	return oepaGetPlaybackTime();
}

extern "C" void c_oepaStopPlayback()
{
	oepaStopPlayback();
}

extern "C" int c_oepaStartRecording(char *path)
{
	return oepaStartRecording(path);
}

extern "C" float c_oepaGetRecordingTime()
{
	return oepaGetRecordingTime();
}

extern "C" void c_oepaStopRecording()
{
	oepaStopRecording();
}

extern "C" void c_oepaOpen()
{
	oepaOpen();
}

extern "C" void c_oepaClose()
{
	oepaClose();
}
