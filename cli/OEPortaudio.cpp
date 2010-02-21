
/**
 * OpenEmulator
 * OpenEmulator/portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/portaudio interface.
 */

#include "math.h"

#include <vector>

#include "portaudio.h"

#include "OEPortaudio.h"
#include "HostAudio.h"

#define OEPA_SAMPLERATE 48000.0;
#define OEPA_CHANNELNUM 2;
#define OEPA_FRAMESPERBUFFER 512;

typedef struct
{
	bool isRunning;
	OEEmulation *emulation;
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	vector<float> inputAudio;
	vector<float> outputAudio;
} OEPAEmulation;

typedef vector<OEPAEmulation *> OEPAEmulations;

bool oepaFullDuplex = false;
double oepaSampleRate = OEPA_SAMPLERATE;
int oepaChannelNum = OEPA_CHANNELNUM;
int oepaFramesPerBuffer = OEPA_FRAMESPERBUFFER;

PaStream *oepaStream = NULL;

pthread_mutex_t oepaMutex;
OEPAEmulations oepaEmulations;

static int oepaCallback(const void *inputBuffer,
						void *outputBuffer,
						unsigned long framesPerBuffer,
						const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags,
						void *userData)
{
	memset(outputBuffer, 0, sizeof(float) * oepaChannelNum * framesPerBuffer);
	
	pthread_mutex_lock(&oepaMutex);
	
	OEPAEmulations::iterator i;
	for (i = oepaEmulations.begin(); i != oepaEmulations.end(); i++)
	{
		pthread_mutex_lock(&(*i)->mutex);
		
		// Call getBuffer
//		(*i)->emulation.ioctl("host::audio", 0, &hostAudio);
		
		pthread_cond_signal(&(*i)->cond);
		pthread_mutex_unlock(&(*i)->mutex);
		
		float *in = (float *)inputBuffer;
		float *out = (float *)outputBuffer;
		
		for(int n = 0; n < framesPerBuffer; n++)
		{
			static double phase = 0;
			
			float value = 0;
			if (in)
				value += *in;
			
			//		float value = 0.01 * (rand() & 0xffff) / 65535.0;
			value += 0.05 * sin(phase);
			phase += 2 * M_PI * 440 / oepaSampleRate;
			*out += value;
			
			if (in)
				in += oepaChannelNum;
			out += oepaChannelNum;
		}
	}
	
	pthread_mutex_unlock(&oepaMutex);
	
	return paContinue;
}

void *oepaRunEmulation(void *arg)
{
	OEPAEmulation *oepaEmulation = (OEPAEmulation *) arg;
	
	pthread_mutex_lock(&oepaEmulation->mutex);
	while (oepaEmulation->isRunning)
	{
		// Call renderBuffer
	//	oepaEmulation->emulation.ioctl("host::audio", 0, &hostAudio);
		
		for (int i = 0; i < 100000; i++);
		
		pthread_cond_wait(&oepaEmulation->cond, &oepaEmulation->mutex);
	}
	pthread_mutex_unlock(&oepaEmulation->mutex);
	
	return NULL;
}

void oepaGetVideo(OEEmulation *emulation)
{
	
}

OEPAEmulation *oepaGetEmulation(OEEmulation *emulation)
{
	OEPAEmulations::iterator i;
	for (i = oepaEmulations.begin(); i != oepaEmulations.end(); i++)
	{
		if ((*i)->emulation == emulation)
			return *i;
	}
	
	return NULL;
}

OEPAEmulation *oepaEraseEmulation(OEEmulation *emulation)
{
	OEPAEmulations::iterator i;
	for (i = oepaEmulations.begin(); i != oepaEmulations.end(); i++)
	{
		if ((*i)->emulation == emulation)
		{
			OEPAEmulation *oepaEmulation = *i;
			oepaEmulations.erase(i);
			return oepaEmulation;
		}
	}
	
	return false;
}

bool oepaAudioOpen()
{
	if (!oepaStream)
	{
		int status = Pa_Initialize();
		if (status == paNoError)
		{
			status = Pa_OpenDefaultStream(&oepaStream,
										  oepaFullDuplex ? oepaChannelNum : 0,
										  oepaChannelNum,
										  paFloat32,
										  oepaSampleRate,
										  oepaFramesPerBuffer,
										  oepaCallback,
										  NULL);
			if (status == paNoError)
				status = Pa_StartStream(oepaStream);
		}
		
		if (status != paNoError)
		{
			fprintf(stderr, "portaudio: couldn't open audio, error %d\n", status);
			return false;
		}
	}
	
	return true;
}

void oepaAudioClose()
{
	if (oepaStream)
	{
		Pa_StopStream(oepaStream);
		Pa_CloseStream(oepaStream);
		
		oepaStream = NULL;
	}
}

bool oepaAudioIsOpen()
{
	return (oepaStream != NULL);
}

bool oepaReopen()
{
	bool status;
	
	pthread_mutex_lock(&oepaMutex);
	oepaAudioClose();
	status = oepaAudioOpen();
	pthread_mutex_unlock(&oepaMutex);
	
	return status;
}

void oepaSetFullDuplex(bool value)
{
	oepaFullDuplex = value;
	
	if (oepaAudioIsOpen())
		oepaReopen();
}

void oepaOpen()
{
	pthread_mutex_init(&oepaMutex, NULL);
	
	oepaAudioOpen();
}

void oepaClose()
{
	oepaAudioClose();
	
	pthread_mutex_destroy(&oepaMutex);
}

OEEmulation *oepaConstruct(string path, string resourcePath)
{
	OEPAEmulation *oepaEmulation = new OEPAEmulation;
	if (!oepaEmulation)
		return NULL;
	
	oepaEmulation->emulation = new OEEmulation(path, resourcePath);
	if (oepaEmulation->emulation)
	{
		oepaEmulation->isRunning = true;
		pthread_mutex_init(&oepaEmulation->mutex, NULL);
		pthread_cond_init(&oepaEmulation->cond, NULL);
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&oepaEmulation->thread,
					   &attr,
					   oepaRunEmulation,
					   (void *) oepaEmulation);
		
		pthread_mutex_lock(&oepaMutex);
		oepaEmulations.push_back(oepaEmulation);
		pthread_mutex_unlock(&oepaMutex);
	}
	else
		delete oepaEmulation;
	
	return oepaEmulation->emulation;
}

void oepaDestroy(OEEmulation *emulation)
{
	pthread_mutex_lock(&oepaMutex);
	OEPAEmulation *oepaEmulation = oepaEraseEmulation(emulation);
	pthread_mutex_unlock(&oepaMutex);
	
	if (oepaEmulation)
	{
		oepaEmulation->isRunning = false;
		pthread_cond_signal(&oepaEmulation->cond);
		void *status;
		pthread_join(oepaEmulation->thread, &status);
		pthread_mutex_destroy(&oepaEmulation->mutex);
		pthread_cond_destroy(&oepaEmulation->cond);
	}
	
	delete emulation;
}

void oepaLock(OEEmulation *emulation)
{
	pthread_mutex_lock(&oepaMutex);
	OEPAEmulation *oepaEmulation = oepaGetEmulation(emulation);
	if (oepaEmulation)
		pthread_mutex_lock(&oepaEmulation->mutex);
}

void oepaUnlock(OEEmulation *emulation)
{
	OEPAEmulation *oepaEmulation = oepaGetEmulation(emulation);
	if (oepaEmulation)
		pthread_mutex_unlock(&oepaEmulation->mutex);
	pthread_mutex_unlock(&oepaMutex);
}

bool oepaSave(OEEmulation *emulation,
			  string path)
{
	oepaLock(emulation);
	bool status = emulation->save(path);
	oepaUnlock(emulation);
	
	return status;
}

int oepaIoctl(OEEmulation *emulation,
			   string device, int message, void *data)
{
	oepaLock(emulation);
	int status = emulation->ioctl(device, message, data);
	oepaUnlock(emulation);
	
	return status;
}

bool oepaAddDevices(OEEmulation *emulation, string path,
					OEStringRefMap connections)
{
	oepaLock(emulation);
	bool status = emulation->addDevices(path, connections);
	oepaUnlock(emulation);
	
	return status;
}

bool oepaIsDeviceTerminal(OEEmulation *emulation, OERef ref)
{
	oepaLock(emulation);
	bool status = emulation->isDeviceTerminal(ref);
	oepaUnlock(emulation);
	
	return status;
}

bool oepaRemoveDevice(OEEmulation *emulation, OERef ref)
{
	oepaLock(emulation);
	bool status = emulation->removeDevice(ref);
	oepaUnlock(emulation);
	
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
