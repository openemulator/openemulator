
/**
 * OpenEmulator
 * OpenEmulator/portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/portaudio interface.
 */

#include "math.h"
#include <iostream>

#include <pthread.h>

#include "portaudio.h"

#include "oepa.h"

// Configuration variables
bool oepaFullDuplex = false;
double oepaSampleRate = OEPA_SAMPLERATE;
int oepaChannelNum = OEPA_CHANNELNUM;
int oepaFramesPerBuffer = OEPA_FRAMESPERBUFFER;
int oepaBufferNum = OEPA_BUFFERNUM;

// Audio callback
bool oepaAudioOpen = false;
PaStream *oepaAudioStream = NULL;
volatile int oepaAudioBufferIndex = 0;
volatile int oepaEmulationBufferIndex = 0;
vector<float> oepaInputBuffer;
vector<float> oepaOutputBuffer;

// Timer thread (when there is no audio)
bool oepaTimerThreadOpen = false;
pthread_t oepaTimerThread;

// Emulations thread variables
bool oepaEmulationsThreadOpen = false;
pthread_t oepaEmulationsThread;
pthread_mutex_t oepaEmulationsMutex;
vector<OEEmulation *> oepaEmulations;

//
// Audio and timer
//
static int oepaCallbackAudio(const void *inputBuffer,
							 void *outputBuffer,
							 unsigned long framesPerBuffer,
							 const PaStreamCallbackTimeInfo* timeInfo,
							 PaStreamCallbackFlags statusFlags,
							 void *userData)
{
	int bufferSize = oepaFramesPerBuffer * oepaChannelNum;
	
	if (oepaAudioBufferIndex == oepaEmulationBufferIndex)
	{
		float *out = (float *) outputBuffer;
		for (int i = 0; i < bufferSize; i++)
			*out++ = (float) (((short) rand()) / 32768);
		
		return paContinue;
	}
	
	if (inputBuffer)
	{
		memcpy(&oepaInputBuffer[bufferSize * oepaAudioBufferIndex],
			   inputBuffer,
			   bufferSize * sizeof(float));
	}
	memcpy(outputBuffer,
		   &oepaOutputBuffer[bufferSize * oepaAudioBufferIndex],
		   bufferSize * sizeof(float));
	
	oepaAudioBufferIndex = (oepaAudioBufferIndex + 1) % oepaBufferNum;
	
	return paContinue;
}

void *oepaRunTimer(void *arg)
{
	while (oepaTimerThreadOpen)
	{
		usleep(1E6 * oepaFramesPerBuffer / oepaSampleRate);
		
		oepaAudioBufferIndex = (oepaAudioBufferIndex + 1) % oepaBufferNum;
	}
	
	return NULL;
}

bool oepaIsAudioOpen()
{
	return oepaAudioOpen;
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

void oepaSetFullDuplex(bool value)
{
	bool isAudioOpen = oepaAudioOpen;
	
	if (isAudioOpen)
		oepaCloseAudio();
	
	oepaFullDuplex = value;
	
	if (isAudioOpen)
		oepaOpenAudio();
}

void oepaSetSampleRate(double value)
{
	bool isAudioOpen = oepaAudioOpen;
	
	if (isAudioOpen)
		oepaCloseAudio();
	
	oepaSampleRate = value;
	
	if (isAudioOpen)
		oepaOpenAudio();
}

void oepaSetChannelNum(int value)
{
	bool isAudioOpen = oepaAudioOpen;
	
	if (isAudioOpen)
		oepaCloseAudio();
	
	oepaChannelNum = value;
	
	if (isAudioOpen)
		oepaOpenAudio();
}

void oepaSetFramesPerBuffer(int value)
{
	bool isAudioOpen = oepaAudioOpen;
	
	if (isAudioOpen)
		oepaCloseAudio();
	
	oepaFramesPerBuffer = value;
	
	if (isAudioOpen)
		oepaOpenAudio();
}

void oepaSetBufferNum(int value)
{
	bool isAudioOpen = oepaAudioOpen;
	
	if (isAudioOpen)
		oepaCloseAudio();
	
	oepaBufferNum = value;
	
	if (isAudioOpen)
		oepaOpenAudio();
}

//
// Emulations
//
void *oepaRunEmulations(void *arg)
{
	vector<float> inputBuffer;
	vector<float> outputBuffer;
	int framesPerBuffer = 0;
	int channelNum = 0;
	
	while (oepaEmulationsThreadOpen)
	{
		if (oepaAudioBufferIndex != oepaEmulationBufferIndex)
		{
			pthread_mutex_lock(&oepaEmulationsMutex);
			
			if ((oepaFramesPerBuffer != framesPerBuffer) ||
				(oepaChannelNum != channelNum))
			{
				inputBuffer.resize(oepaFramesPerBuffer * oepaChannelNum * 2);
				outputBuffer.resize(oepaFramesPerBuffer * oepaChannelNum * 2);
				framesPerBuffer = oepaFramesPerBuffer;
			}
			
			HostAudioBuffer buffer = {
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
			
			pthread_mutex_unlock(&oepaEmulationsMutex);
			
			oepaEmulationBufferIndex = (oepaEmulationBufferIndex + 1) % oepaBufferNum;
		}
		
		usleep(1000);
	}
	
	return NULL;
}

void oepaCloseEmulations()
{
	
}

bool oepaOpenEmulations()
{
	pthread_mutex_init(oepaEmulationsMutex, NULL);
	
	int error;
	pthread_attr_t attr;
	error = pthread_attr_init(&attr);
	if (!error)
	{
		error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		if (!error)
		{
			oepaEmulationsThreadOpen = true;
			error = pthread_create(&oepaEmulationsThread,
								   &attr,
								   oepaRunEmulations,
								   NULL);
			if (!error)
			{
				oepaAudioOpen = true;
				return true;
			}
			else
				cerr << "oepa: couldn't create emulations thread, error " << error << "\n";
		}
		else
			cerr << "oepa: couldn't attr emulations thread, error " << error << "\n";
	}
	else
		cerr << "oepa: couldn't init emulations thread, error " << error << "\n";
	
	return false;
}



void oepaOpen()
{
	
	oepaOpenThread();
	oepaOpenAudio();
}

void oepaClose()
{
	oepaCloseAudio();
	oepaCloseThread();
	
	pthread_mutex_destroy(&oepaMutex);
}

OEEmulation *oepaConstruct(string path, string resourcePath)
{
	
	pthread_mutex_lock(&oepaMutex);
	
	OEPAEmulation *oepaEmulation = new OEPAEmulation;
	if (!oepaEmulation)
		return NULL;
	
	oepaEmulation->emulation = new OEEmulation(path, resourcePath);
	if (oepaEmulation->emulation)
	{
		oepaEmulation->isRunning = true;
		
		oepaUpdateAudio(oepaEmulation);
		
		pthread_mutex_init(&oepaEmulation->mutex, NULL);
		pthread_cond_init(&oepaEmulation->cond, NULL);
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&oepaEmulation->thread,
					   &attr,
					   oepaRunEmulation,
					   oepaEmulation);
		
		oepaEmulations.push_back(oepaEmulation);
		pthread_mutex_unlock(&oepaMutex);
	}
	else
	{
		delete oepaEmulation;
		oepaEmulation = NULL;
	}
	
	return oepaEmulation;
}

void oepaDestroy(OEPAEmulation *oepaEmulation)
{
	if (!oepaEmulation)
		return;
	
	pthread_mutex_lock(&oepaMutex);
	
	for (OEPAEmulations::iterator i = oepaEmulations.begin();
		 i != oepaEmulations.end();
		 i++)
	{
		if ((*i) == oepaEmulation)
		{
			oepaEmulations.erase(i);
			break;
		}
	}
	
	pthread_mutex_unlock(&oepaMutex);
	
	oepaEmulation->isRunning = false;
	pthread_cond_signal(&oepaEmulation->cond);
	void *status;
	pthread_join(oepaEmulation->thread, &status);
	pthread_mutex_destroy(&oepaEmulation->mutex);
	pthread_cond_destroy(&oepaEmulation->cond);
	
	if (oepaEmulation->emulation)
		delete oepaEmulation->emulation;
	
	delete oepaEmulation;
}

void oepaLock(OEPAEmulation *oepaEmulation)
{
	pthread_mutex_lock(&oepaMutex);
	if (oepaEmulation)
		pthread_mutex_lock(&oepaEmulation->mutex);
}

void oepaUnlock(OEPAEmulation *oepaEmulation)
{
	if (oepaEmulation)
		pthread_mutex_unlock(&oepaEmulation->mutex);
	pthread_mutex_unlock(&oepaMutex);
}

bool oepaIsLoaded(OEPAEmulation *oepaEmulation)
{
	return oepaEmulation->emulation->isLoaded();
}

bool oepaSave(OEPAEmulation *oepaEmulation,
			  string path)
{
	oepaLock(oepaEmulation);
	bool status = oepaEmulation->emulation->save(path);
	oepaUnlock(oepaEmulation);
	
	return status;
}

int oepaIoctl(OEPAEmulation *oepaEmulation,
			  string device, int message, void *data)
{
	oepaLock(oepaEmulation);
	int status = oepaEmulation->emulation->ioctl(device, message, data);
	oepaUnlock(oepaEmulation);
	
	return status;
}

xmlDocPtr oepaGetDML(OEPAEmulation *oepaEmulation)
{
	return oepaEmulation->emulation->getDML();
}

bool oepaAddDevices(OEPAEmulation *oepaEmulation, string path,
					OEStringRefMap connections)
{
	oepaLock(oepaEmulation);
	bool status = oepaEmulation->emulation->addDevices(path, connections);
	oepaUnlock(oepaEmulation);
	
	return status;
}

bool oepaIsDeviceTerminal(OEPAEmulation *oepaEmulation, OERef ref)
{
	oepaLock(oepaEmulation);
	bool status = oepaEmulation->emulation->isDeviceTerminal(ref);
	oepaUnlock(oepaEmulation);
	
	return status;
}

bool oepaRemoveDevice(OEPAEmulation *oepaEmulation, OERef ref)
{
	oepaLock(oepaEmulation);
	bool status = oepaEmulation->emulation->removeDevice(ref);
	oepaUnlock(oepaEmulation);
	
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
