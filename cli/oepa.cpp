
/**
 * OpenEmulator
 * OpenEmulator/portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/portaudio interface.
 */

#include <iostream>

#include <pthread.h>

#include "portaudio.h"

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
bool oepaTimerThreadOpen = false;
pthread_t oepaTimerThread;

// Emulations
bool oepaEmulationsThreadOpen = false;
pthread_t oepaEmulationsThread;
pthread_mutex_t oepaEmulationsMutex;
vector<OEEmulation *> oepaEmulations;

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
	int bufferSampleNum = oepaFramesPerBuffer * oepaChannelNum;
	
	if (oepaAudioBufferIndex == oepaEmulationsBufferIndex)
	{
		float *out = (float *) outputBuffer;
		for (int i = 0; i < bufferSampleNum; i++)
			*out++ = rand() * (0.1 / RAND_MAX);
		
		return paContinue;
	}
	
	if (inputBuffer)
		memcpy(&oepaInputBuffer[oepaAudioBufferIndex * bufferSampleNum],
			   inputBuffer, bufferSampleNum * sizeof(float));
	else
		memset(&oepaInputBuffer[oepaAudioBufferIndex * bufferSampleNum],
			   0, bufferSampleNum * sizeof(float));
	memcpy(outputBuffer,
		   &oepaOutputBuffer[oepaAudioBufferIndex * bufferSampleNum],
		   bufferSampleNum * sizeof(float));
	
	oepaAudioBufferIndex = (oepaAudioBufferIndex + 1) % oepaBufferNum;
	
	return paContinue;
}

void *oepaRunTimer(void *arg)
{
	while (oepaTimerThreadOpen)
	{
		usleep(1E6 * oepaFramesPerBuffer / oepaSampleRate);
		
		if (oepaAudioBufferIndex == oepaEmulationsBufferIndex)
			continue;
		
		oepaAudioBufferIndex = (oepaAudioBufferIndex + 1) % oepaBufferNum;
	}
	
	return NULL;
}

void *oepaRunEmulations(void *arg)
{
	vector<float> inputBuffer;
	vector<float> outputBuffer;
	int framesPerBuffer = 0;
	int channelNum = 0;
	int bufferSampleNum = 0;
	int bufferByteNum = 0;
	
	while (oepaEmulationsThreadOpen)
	{
		int audioIndex = oepaAudioBufferIndex;
		int nextIndex = (oepaEmulationsBufferIndex + 1) % oepaBufferNum;
		
		if (audioIndex == nextIndex)
			usleep(1000);
		else
		{
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
			
			memcpy(&inputBuffer[0],
				   &inputBuffer[bufferSampleNum],
				   bufferByteNum);
			memcpy(&inputBuffer[bufferSampleNum],
				   &oepaInputBuffer[oepaEmulationsBufferIndex * bufferSampleNum],
				   bufferByteNum);
			
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
			
			memcpy(&oepaOutputBuffer[oepaEmulationsBufferIndex * bufferSampleNum],
				   &outputBuffer[0],
				   bufferByteNum);
			memcpy(&outputBuffer[0],
				   &outputBuffer[bufferSampleNum],
				   bufferByteNum);
			
			pthread_mutex_unlock(&oepaEmulationsMutex);
			
			oepaEmulationsBufferIndex = nextIndex;
		}
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
		if (status == paNoError)
		{
			status = Pa_StartStream(oepaAudioStream);
			if (status == paNoError)
			{
				oepaAudioOpen = true;
				return true;
			}
			else
				cerr <<  "oepa: couldn't start audio stream, error " << status << "\n";
			
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
				cerr << "oepa: couldn't create timer thread, error " << error << "\n";
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
	if (!oepaEmulationsThreadOpen)
		return;
	
	oepaEmulationsThreadOpen = false;
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
				oepaEmulationsThreadOpen = true;
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

bool oepaDisableAudio()
{
	bool state = oepaAudioOpen;
	
	if (state)
		oepaCloseAudio();
	
	if (oepaEmulationsThreadOpen)
		pthread_mutex_lock(&oepaEmulationsMutex);
	
	return state;
}

void oepaEnableAudio(bool state)
{
	if (oepaEmulationsThreadOpen)
		pthread_mutex_unlock(&oepaEmulationsMutex);
	
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
	
	oepaSampleRate = value;
	
	oepaEnableAudio(state);
}

void oepaSetChannelNum(int value)
{
	bool state = oepaDisableAudio();
	
	oepaChannelNum = value;
	
	oepaEnableAudio(state);
}

void oepaSetFramesPerBuffer(int value)
{
	bool state = oepaDisableAudio();
	
	oepaFramesPerBuffer = value;
	
	oepaEnableAudio(state);
}

void oepaSetBufferNum(int value)
{
	bool state = oepaDisableAudio();
	
	oepaBufferNum = value + 1;
	
	oepaEnableAudio(state);
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

extern "C" void c_oepaOpen()
{
	oepaOpen();
}

extern "C" void c_oepaClose()
{
	oepaClose();
}
