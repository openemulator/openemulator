
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

/*
void calculate()
{
	HostAudioBuffer *buffer = &oepaEmulation->hostAudioBuffer;
	float *in = buffer->input;
	float *out = buffer->output;
	
	for(int i = 0; i < buffer->frameNum; i++)
	{
		static double phase = 0;
		float value = 0;
		//				value += *in;
		//				value += 0.01 * (rand() & 0xffff) / 65535.0;
		value += 0.05 * sin(phase);
		phase += 2 * M_PI * 440 / buffer->sampleRate;
		if (oh)
		{
			out[0] += value;
			out[1] = 0;
		}
		
		in += buffer->channelNum;
		out += buffer->channelNum;
	}
}
*/

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
	int sampleNum = oepaFramesPerBuffer * oepaChannelNum;
	
	if (oepaAudioBufferIndex == oepaEmulationBufferIndex)
	{
		float *out = (float *) outputBuffer;
		for (int i = 0; i < sampleNum; i++)
			*out++ = (float) (((short) rand()) / 32768);
		
		return paContinue;
	}
	
	if (inputBuffer)
	{
		memcpy(&oepaInputBuffer[bufferSize * oepaAudioBufferIndex],
			   inputBuffer,
			   sampleNum * sizeof(float));
	}
	memcpy(outputBuffer,
		   &oepaOutputBuffer[bufferSize * oepaAudioBufferIndex],
		   sampleNum * sizeof(float));
	
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

void *oepaRunEmulations(void *arg)
{
	vector<float> inputBuffer;
	vector<float> outputBuffer;
	int framesPerBuffer = 0;
	int channelNum = 0;
	int sampleNum = 0;
	
	while (oepaEmulationsThreadOpen)
	{
		if (oepaAudioBufferIndex != oepaEmulationBufferIndex)
		{
			pthread_mutex_lock(&oepaEmulationsMutex);
			
			if ((oepaFramesPerBuffer != framesPerBuffer) ||
				(oepaChannelNum != channelNum))
			{
				framesPerBuffer = oepaFramesPerBuffer;
				channelNum = oepaChannelNum;
				bufferSize = framesPerBuffer * channelNum * sizeof(float);
				
				inputBuffer.resize(framesPerBuffer * channelNum * 2);
				memset(&inputBuffer[0], 0, bufferSize);
				outputBuffer.resize(framesPerBuffer * channelNum * 2);
				memset(&outputBuffer[0], 0, bufferSize);
			}
			
			memcpy(&inputBuffer[0], oepaInputBuffer, bufferSize);
			
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
			
			memcpy(&inputBuffer[0], oepaInputBuffer, bufferSize);
			pthread_mutex_unlock(&oepaEmulationsMutex);
			
			oepaEmulationBufferIndex = (oepaEmulationBufferIndex + 1) % oepaBufferNum;
		}
		
		usleep(1000);
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
	
	error = pthread_mutex_init(&oepaEmulationsMutex, NULL);
	if (error)
	{
		cerr << "oepa: couldn't init emulations mutex, error " << error << "\n";
		return false;
	}
	
	pthread_attr_t attr;
	error = pthread_attr_init(&attr);
	if (error)
	{
		cerr << "oepa: couldn't init emulations attr, error " << error << "\n";
		return false;
	}
	
	error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	if (error)
	{
		cerr << "oepa: couldn't attr emulations thread, error " << error << "\n";
		return false;
	}
	
	oepaEmulationsThreadOpen = true;
	error = pthread_create(&oepaEmulationsThread,
						   &attr,
						   oepaRunEmulations,
						   NULL);
	if (!error)
	{
		cerr << "oepa: couldn't create emulations thread, error " << error << "\n";
		return false;
	}
	
	return true;
}

//
// OEPA Interface
//
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
