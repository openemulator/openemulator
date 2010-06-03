
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
bool oepaPlayThrough = false;
double oepaSampleRate = OEPA_SAMPLERATE;
int oepaChannelNum = OEPA_CHANNELNUM;
int oepaFramesPerBuffer = OEPA_FRAMESPERBUFFER;
int oepaBufferNum = OEPA_BUFFERNUM;

// Buffers
volatile int oepaBufferInputIndex = 0;
volatile int oepaBufferOutputIndex = 0;
vector<float> oepaBuffer;

// Audio
bool oepaAudioOpen = false;
PaStream *oepaAudioStream = NULL;
float oepaVolume = 1.0F;
float oepaInstantVolume = 1.0F;
float oepaVolumeAlpha = 0.0F;
bool oepaTimerThreadOpen = false;
pthread_t oepaTimerThread;

// Processing
bool oepaProcessOpen = false;
pthread_t oepaProcessThread;
pthread_mutex_t oepaProcessMutex;
vector<OEEmulation *> oepaEmulations;

// Audio playback
bool oepaPlayback = false;
SNDFILE *oepaPlaybackFile;
long long oepaPlaybackFrameIndex = 0;
long long oepaPlaybackFrameNum = 0;
int oepaPlaybackChannelNum;
double oepaPlaybackSRCRatio;
SRC_STATE *oepaPlaybackSRC;
bool oepaPlaybackEndOfInput;
vector<float> oepaPlaybackBuffer;
int oepaPlaybackBufferFrameBegin;
int oepaPlaybackBufferFrameEnd;

// Audio recording
bool oepaRecording = false;
SNDFILE *oepaRecordingFile;
long long oepaRecordingFrameNum = 0;

// Buffers
int oepaGetBufferInputSize()
{
	int stateNum = 2 * oepaBufferNum;
	int index = (oepaBufferOutputIndex - oepaBufferInputIndex + stateNum) % stateNum;
	return oepaBufferNum - index;
}

int oepaGetBufferOutputSize()
{
	int stateNum = 2 * oepaBufferNum;
	int index = (oepaBufferOutputIndex - oepaBufferInputIndex + stateNum) % stateNum;
	return index;
}

float *oepaGetBufferInput()
{
 	int index = oepaBufferInputIndex % oepaBufferNum;
	int samplesPerBuffer = oepaFramesPerBuffer * oepaChannelNum;
	return &oepaBuffer[index * samplesPerBuffer];
}

float *oepaGetBufferOutput()
{
 	int index = oepaBufferOutputIndex % oepaBufferNum;
	int samplesPerBuffer = oepaFramesPerBuffer * oepaChannelNum;
	return &oepaBuffer[index * samplesPerBuffer];
}

void oepaIncrementBufferInputIndex()
{
	int stateNum = 2 * oepaBufferNum;
	oepaBufferInputIndex = (oepaBufferInputIndex + 1) % stateNum;
}

void oepaIncrementBufferOutputIndex()
{
	int stateNum = 2 * oepaBufferNum;
	oepaBufferOutputIndex = (oepaBufferOutputIndex + 1) % stateNum;
}

void oepaInitBuffer()
{
	int bufferSize = oepaBufferNum * oepaFramesPerBuffer * oepaChannelNum;
	oepaBuffer.resize(bufferSize);
	
	oepaBufferInputIndex = 0;
	oepaBufferOutputIndex = oepaBufferNum;
}

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
	if ((oepaGetBufferOutputSize() <= 0) ||
		(framesPerBuffer != oepaFramesPerBuffer))
	{
		int samplesPerBuffer = framesPerBuffer * oepaChannelNum;
		float *out = (float *) outputBuffer;
		
		// To-Do: Replace volume with last energy average
		
		for (int i = 0; i < samplesPerBuffer; i++)
			*out++ = rand() * (0.1 / RAND_MAX);
		
		return paContinue;
	}
	
	int samplesPerBuffer = framesPerBuffer * oepaChannelNum;
	int bytesPerBuffer = samplesPerBuffer * sizeof(float);
	float *buffer = oepaGetBufferInput();
	
	// Output
	{
		float *in = buffer;
		float *out = (float *) outputBuffer;
		
		for (int i = 0; i < framesPerBuffer; i++)
		{
			for (int c = 0; c < oepaChannelNum; c++)
				*out++ = *in++ * oepaInstantVolume;
			
			oepaInstantVolume += (oepaVolume - oepaInstantVolume) * oepaVolumeAlpha;
		}
	}
	
	// Input
	if (inputBuffer)
		memcpy(buffer, inputBuffer, bytesPerBuffer);
	else
		memset(buffer, 0, bytesPerBuffer);
	
	oepaIncrementBufferInputIndex();
	
	return paContinue;
}

void *oepaRunTimer(void *arg)
{
	while (oepaTimerThreadOpen)
	{
		usleep(1E6 * oepaFramesPerBuffer / oepaSampleRate);
		
		if (oepaGetBufferOutputSize() > 0)
			oepaIncrementBufferInputIndex();
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
			cerr <<  "oepa: couldn't open audio stream (error " <<
			status << "), attempting half-duplex\n";
			
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
				cerr <<  "oepa: couldn't start audio stream (error " <<
				status << ")\n";
			
			Pa_CloseStream(oepaAudioStream);
		}
		else
			cerr <<  "oepa: couldn't open audio stream (error " <<
			status << ")\n";
	}
	else
		cerr <<  "oepa: couldn't init portaudio (error " <<
		status << ")\n";
	
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
				cerr << "oepa: couldn't create timer thread (error " <<
				error << ")\n";
		}
		else
			cerr << "oepa: couldn't attr timer thread (error " <<
			error << ")\n";
	}
	else
		cerr << "oepa: couldn't init timer thread (error " <<
		error << ")\n";
	
	return false;
}

//
// Processing
//
void oepaPlaybackAudio(float *buffer, int frameNum, int channelNum)
{
	vector<float> srcBuffer;
	srcBuffer.resize(frameNum * oepaPlaybackChannelNum);
	
	int playbackBufferFrameNum = oepaPlaybackBuffer.size() /
		oepaPlaybackChannelNum;
	
	do
	{
		if (oepaPlaybackBufferFrameBegin >= oepaPlaybackBufferFrameEnd)
		{
			int n = sf_readf_float(oepaPlaybackFile,
								   &oepaPlaybackBuffer[0],
								   playbackBufferFrameNum);
			
			oepaPlaybackBufferFrameBegin = 0;
			oepaPlaybackBufferFrameEnd = n;
			
			if (n != playbackBufferFrameNum)
				oepaPlaybackEndOfInput = true;
		}
		
		int index = oepaPlaybackBufferFrameBegin * oepaPlaybackChannelNum;
		int inputFrameNum = (oepaPlaybackBufferFrameEnd - 
							 oepaPlaybackBufferFrameBegin);
		SRC_DATA srcData =
		{
			&oepaPlaybackBuffer[index],
			&srcBuffer[0],
			inputFrameNum,
			frameNum,
			0,
			0,
			oepaPlaybackEndOfInput,
			oepaPlaybackSRCRatio,
		};
		src_process(oepaPlaybackSRC, &srcData);
		
		if (!srcData.output_frames_gen)
		{
			sf_close(oepaPlaybackFile);
			src_delete(oepaPlaybackSRC);
			
			oepaPlayback = false;
			
			return;
		}
		
		float *in = &srcBuffer[0];
		for (int i = 0; i < srcData.output_frames_gen; i++)
		{
			for (int ch = 0; ch < channelNum; ch++)
				buffer[ch] += in[ch % oepaPlaybackChannelNum];
			
			in += oepaPlaybackChannelNum;
			buffer += channelNum;
		}
		
		oepaPlaybackFrameIndex += srcData.output_frames_gen;
		oepaPlaybackBufferFrameBegin += srcData.input_frames_used;
		frameNum -= srcData.output_frames_gen;
	} while (frameNum > 0);
}

void oepaRecordAudio(float *buffer, int frameNum, int channelNum)
{
	int n = sf_writef_float(oepaRecordingFile, buffer, frameNum);
	oepaRecordingFrameNum += n;
	
	if (frameNum != n)
	{
		sf_close(oepaRecordingFile);
		
		oepaRecording = false;
	}
}

void *oepaProcess(void *arg)
{
	vector<float> inputBuffer;
	vector<float> outputBuffer;
	
	int framesPerBuffer = 0;
	int samplesPerBuffer;
	int bytesPerBuffer;
	int channelNum = 0;
	
	while (oepaProcessOpen)
	{
		if (oepaGetBufferInputSize() <= 0)
		{
			usleep(1000);
			continue;
		}
		
		pthread_mutex_lock(&oepaProcessMutex);
		
		// Update audio format
		if ((oepaFramesPerBuffer != framesPerBuffer) ||
			(oepaChannelNum != channelNum))
		{
			framesPerBuffer = oepaFramesPerBuffer;
			samplesPerBuffer = framesPerBuffer * oepaChannelNum;
			bytesPerBuffer = samplesPerBuffer * sizeof(float);
			channelNum = oepaChannelNum;
			
			inputBuffer.resize(2 * samplesPerBuffer);
			memset(&inputBuffer[0], 0, 2 * bytesPerBuffer);
			outputBuffer.resize(2 * samplesPerBuffer);
			memset(&outputBuffer[0], 0, 2 * bytesPerBuffer);
		}
		
		float *buffer = oepaGetBufferOutput();
		
		// Input
		if (oepaPlayback)
			oepaPlaybackAudio(buffer, framesPerBuffer, channelNum);
		
		memcpy(&inputBuffer[0],
			   &inputBuffer[samplesPerBuffer],
			   bytesPerBuffer);
		memcpy(&inputBuffer[samplesPerBuffer],
			   buffer,
			   bytesPerBuffer);
		
		// Play through
		if (oepaPlayThrough)
		{
			for (int i = 0; i < samplesPerBuffer; i++)
				outputBuffer[i] += inputBuffer[i];
		}
		
		// Output
		HostAudioBuffer hostAudioBuffer =
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
		{
			OEComponent *component = (*i)->getComponent("host::audio");
			component->postNotification(HOSTAUDIO_RENDER_WILL_BEGIN,
										&hostAudioBuffer);
			component->postNotification(HOSTAUDIO_RENDER_DID_BEGIN,
										&hostAudioBuffer);
			component->postNotification(HOSTAUDIO_RENDER_WILL_END,
										&hostAudioBuffer);
			component->postNotification(HOSTAUDIO_RENDER_DID_END,
										&hostAudioBuffer);
		}
		
		memcpy(buffer,
			   &outputBuffer[0],
			   bytesPerBuffer);
		memcpy(&outputBuffer[0],
			   &outputBuffer[samplesPerBuffer],
			   bytesPerBuffer);
		
		if (oepaRecording)
			oepaRecordAudio(buffer, framesPerBuffer, channelNum);
		
		pthread_mutex_unlock(&oepaProcessMutex);
		
		oepaIncrementBufferOutputIndex();
	}
	
	return NULL;
}

void oepaCloseProcess()
{
	if (!oepaProcessOpen)
		return;
	
	oepaProcessOpen = false;
	void *status;
	pthread_join(oepaProcessThread, &status);
	
	pthread_mutex_destroy(&oepaProcessMutex);
}

bool oepaOpenProcess()
{
	int error;
	
	oepaPlayback = false;
	oepaRecording = false;
	
	error = pthread_mutex_init(&oepaProcessMutex, NULL);
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
				param.sched_priority *= 2;
				pthread_attr_setschedparam(&attr, &param);
			}
			
			error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
			if (!error)
			{
				oepaProcessOpen = true;
				error = pthread_create(&oepaProcessThread,
									   &attr,
									   oepaProcess,
									   NULL);
				if (!error)
					return true;
				else
					cerr << "oepa: couldn't create process thread (error " <<
					error << ")\n";
			}
			else
				cerr << "oepa: couldn't attr process thread (error " <<
				error << ")\n";
		}
		else
			cerr << "oepa: couldn't init process attr (error " <<
			error << ")\n";		
	}
	else
		cerr << "oepa: couldn't init process mutex (error " <<
		error << ")\n";
	
	return false;
}

void oepaLockProcess()
{
	if (oepaProcessOpen)
		pthread_mutex_lock(&oepaProcessMutex);
}

void oepaUnlockProcess()
{
	if (oepaProcessOpen)
		pthread_mutex_unlock(&oepaProcessMutex);
}

//
// Interface
//
void oepaOpen()
{
	oepaInitBuffer();
	
	oepaOpenProcess();
	oepaOpenAudio();
}

void oepaClose()
{
	oepaCloseAudio();
	oepaCloseProcess();
}

void oepaStopPlayback()
{
	if (!oepaPlayback)
		return;
	
	oepaLockProcess();
	
	sf_close(oepaPlaybackFile);
	oepaPlayback = false;
	
	oepaUnlockProcess();
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
	
	oepaLockProcess();
	
	oepaPlaybackFrameIndex = 0;
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
			oepaPlaybackBufferFrameBegin = 0;
			oepaPlaybackBufferFrameEnd = 0;
			
			int framesPerBuffer = 2 * oepaPlaybackSRCRatio * oepaFramesPerBuffer;
			oepaPlaybackBuffer.resize(framesPerBuffer * oepaPlaybackChannelNum);

			oepaPlaybackFrameNum = sfInfo.frames * oepaPlaybackSRCRatio;
			
			oepaPlayback = true;
		}
		else
		{
			cerr << "oepa: couldn't init sample rate converter (error " <<
			error << ")\n";
			sf_close(oepaPlaybackFile);
		}
	}
	else
		cerr << "oepa: couldn't open file " << path << "\n";
	
	oepaUnlockProcess();
	
	return oepaPlayback;
}

bool oepaIsPlayback()
{
	return oepaPlayback;
}

float oepaGetPlaybackTime()
{
	return (float) oepaPlaybackFrameIndex / oepaSampleRate;
}

float oepaGetPlaybackDuration()
{
	return (float) oepaPlaybackFrameNum / oepaSampleRate;
}

void oepaStopRecording()
{
	if (!oepaRecording)
		return;
	
	oepaLockProcess();
	
	sf_close(oepaRecordingFile);
	oepaRecording = false;
	
	oepaUnlockProcess();
}

bool oepaStartRecording(string path)
{
	oepaStopRecording();
	
	SF_INFO sfInfo = 
	{
		0,
		oepaSampleRate,
		oepaChannelNum,
		SF_FORMAT_WAV | SF_FORMAT_PCM_16,
		0,
		0,
	};
	
	oepaLockProcess();
	
	oepaRecordingFrameNum = 0;
	oepaRecordingFile = sf_open(path.c_str(), SFM_WRITE, &sfInfo);
	
	if (oepaRecordingFile)
		oepaRecording = true;
	else
		cerr << "oepa: couldn't open temporary file " << path << "\n";
	
	oepaUnlockProcess();
	
	return oepaRecording;
}

bool oepaIsRecording()
{
	return oepaRecording;
}

float oepaGetRecordingTime()
{
	return (float) oepaRecordingFrameNum / oepaSampleRate;
}

long long oepaGetRecordingSize()
{
	return (long long) oepaRecordingFrameNum * oepaChannelNum * sizeof(short);
}

bool oepaDisableAudio()
{
	bool state = oepaAudioOpen;
	
	if (state)
		oepaCloseAudio();
	
	oepaLockProcess();
	
	return state;
}

void oepaEnableAudio(bool state)
{
	oepaInitBuffer();
	
	oepaUnlockProcess();
	
	if (state)
		oepaOpenAudio();
}

void oepaSetFullDuplex(bool value)
{
	bool state = oepaDisableAudio();
	
	oepaFullDuplex = value;
	
	oepaEnableAudio(state);
}

void oepaSetSampleRate(double value)
{
	oepaStopPlayback();
	oepaStopRecording();
	
	bool state = oepaDisableAudio();
	
	oepaSampleRate = value;
	
	oepaEnableAudio(state);
}

void oepaSetChannelNum(int value)
{
	oepaStopPlayback();
	oepaStopRecording();
	
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
	
	oepaBufferNum = value;
	
	oepaEnableAudio(state);
}

void oepaSetVolume(float value)
{
	oepaVolume = value;
}

void oepaSetPlayThrough(bool value)
{
	oepaPlayThrough = value;
}

OEEmulation *oepaConstruct(string path, string resourcePath)
{
	OEEmulation *emulation = new OEEmulation(path, resourcePath);
	if (emulation)
	{
		pthread_mutex_lock(&oepaProcessMutex);
		
		oepaEmulations.push_back(emulation);
		
		pthread_mutex_unlock(&oepaProcessMutex);
	}
	
	return emulation;
}

void oepaDestroy(OEEmulation *emulation)
{
	if (!emulation)
		return;
	
	pthread_mutex_lock(&oepaProcessMutex);
	
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
	
	pthread_mutex_unlock(&oepaProcessMutex);
	
	delete emulation;
}

bool oepaIsLoaded(OEEmulation *emulation)
{
	return emulation->isLoaded();
}

bool oepaSave(OEEmulation *emulation, string path)
{
	pthread_mutex_lock(&oepaProcessMutex);
	
	bool status = emulation->save(path);
	
	pthread_mutex_unlock(&oepaProcessMutex);
	
	return status;
}

bool oepaSetProperty(OEEmulation *emulation, string ref, string name, string value)
{
	pthread_mutex_lock(&oepaProcessMutex);
	
	OEComponent *component = emulation->getComponent(ref);
	if (!component)
		return false;
	
	bool status = component->setProperty(name, value);
	
	pthread_mutex_unlock(&oepaProcessMutex);
	
	return status;
}

bool oepaGetProperty(OEEmulation *emulation, string ref, string name, string &value)
{
	pthread_mutex_lock(&oepaProcessMutex);
	
	OEComponent *component = emulation->getComponent(ref);
	if (!component)
		return false;
	
	bool status = component->setProperty(name, value);
	
	pthread_mutex_unlock(&oepaProcessMutex);
	
	return status;
}

void oepaPostNotification(OEEmulation *emulation,
						  string ref, int notification, void *data)
{
	pthread_mutex_lock(&oepaProcessMutex);
	
	OEComponent *component = emulation->getComponent(ref);
	if (!component)
		return;
	
	component->postNotification(notification, data);
	
	pthread_mutex_unlock(&oepaProcessMutex);
}

int oepaIoctl(OEEmulation *emulation,
			  string ref, int message, void *data)
{
	pthread_mutex_lock(&oepaProcessMutex);
	
	OEComponent *component = emulation->getComponent(ref);
	if (!component)
		return false;
	
	bool status = component->ioctl(message, data);
	
	pthread_mutex_unlock(&oepaProcessMutex);
	
	return status;
}

xmlDocPtr oepaGetDML(OEEmulation *emulation)
{
	return emulation->getDML();
}

bool oepaAddDevices(OEEmulation *emulation, string path, OEStringRefMap connections)
{
	pthread_mutex_lock(&oepaProcessMutex);
	
	bool status = emulation->addDevices(path, connections);
	
	pthread_mutex_unlock(&oepaProcessMutex);
	
	return status;
}

bool oepaIsDeviceTerminal(OEEmulation *emulation, OERef ref)
{
	pthread_mutex_lock(&oepaProcessMutex);
	
	bool status = emulation->isDeviceTerminal(ref);
	
	pthread_mutex_unlock(&oepaProcessMutex);
	
	return status;
}

bool oepaRemoveDevice(OEEmulation *emulation, OERef ref)
{
	pthread_mutex_lock(&oepaProcessMutex);
	
	bool status = emulation->removeDevice(ref);
	
	pthread_mutex_unlock(&oepaProcessMutex);
	
	return status;
}
