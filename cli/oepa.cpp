
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

#include "oepa.h"

#include "Host.h"

using namespace std;

#define oepaLog(text) cerr << "oepa: " << text << endl

static int oepaRunAudio(const void *inputBuffer,
						void *outputBuffer,
						unsigned long framesPerBuffer,
						const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags,
						void *userData)
{
	((OEPA *) userData)->runAudio(inputBuffer, outputBuffer, framesPerBuffer);
}

void *oepaRunTimer(void *arg)
{
	((OEPA *) arg)->runTimer();
	
	return NULL;
}

void *oepaRunProcess(void *arg)
{
	((OEPA *) arg)->runProcess();
	
	return NULL;
}

//
// OEPAEmulation
//

OEPAEmulation::OEPAEmulation(OEPA *oepa, string path, string resourcePath);
{
	if (!emulation)
		return;
	
}

OEPAEmulation::~OEPAEmulation()
{
	
}

bool OEPAEmulation::isLoaded()
{
	return emulation->isLoaded();
}

bool OEPAEmulation::save(string path)
{
	oepa->lockProcess();
	
	bool status = emulation->save(path);
	
	pthread_mutex_unlock(&processMutex);
	
	return status;
}

bool OEPAEmulation::setProperty(string ref, string name, string value)
{
	pthread_mutex_lock(&processMutex);
	
	OEComponent *component = emulation->getComponent(ref);
	bool status;
	if (component)
		status = component->setProperty(name, value);
	else
	{
		status = false;
		oepaLog("could not set property " << ref << "." << name <<
				" (ref not found)");
	}
	
	pthread_mutex_unlock(&oepaProcessMutex);
	
	return status;
}

bool OEPAEmulation::getProperty(string ref, string name, string &value)
{
	pthread_mutex_lock(&processMutex);
	
	OEComponent *component = emulation->getComponent(ref);
	bool status;
	if (component)
		status = component->getProperty(name, value);
	else
	{
		status = false;
		oepaLog("could not get property " << ref << "." << name << 
				" (ref not found)");
	}
	
	pthread_mutex_unlock(&processMutex);
	
	return status;
}

void OEPAEmulation::postNotification(string ref, int notification, void *data)
{
	pthread_mutex_lock(&processMutex);
	
	OEComponent *component = emulation->getComponent(ref);
	if (component)
		component->postNotification(notification, data);
	else
	{
		oepaLog("could not post notification to " << ref <<
				" (ref not found)");
	}
	
	pthread_mutex_unlock(&processMutex);
}

int OEPAEmulation::ioctl(string ref, int message, void *data)
{
	pthread_mutex_lock(&processMutex);
	
	OEComponent *component = emulation->getComponent(ref);
	int status = 0;
	if (component)
		status = component->ioctl(message, data);
	else
	{
		oepaLog("could not ioctl " << ref <<
				" (ref not found)");
	}
	
	pthread_mutex_unlock(&processMutex);
	
	return status;
}

xmlDocPtr OEPAEmulation::getDML()
{
	return emulation->getDML();
}

bool OEPAEmulation::addDevices(string path, OEStringRefMap connections)
{
	pthread_mutex_lock(&processMutex);
	
	bool status = emulation->addDevices(path, connections);
	
	pthread_mutex_unlock(&processMutex);
	
	return status;
}

bool OEPAEmulation::isDeviceTerminal(OERef ref)
{
	pthread_mutex_lock(&processMutex);
	
	bool status = emulation->isDeviceTerminal(ref);
	
	pthread_mutex_unlock(&processMutex);
	
	return status;
}

bool OEPAEmulation::removeDevice(OERef ref)
{
	pthread_mutex_lock(&processMutex);
	
	bool status = emulation->removeDevice(ref);
	
	pthread_mutex_unlock(&processMutex);
	
	return status;
}

//
// OEPA class
//

OEPA::OEPA(string resourcePath)
{
	this->resourcePath = resourcePath;
	
	fullDuplex = false;
	playThrough = false;
	sampleRate = OEPA_SAMPLERATE;
	channelNum = OEPA_CHANNELNUM;
	framesPerBuffer = OEPA_FRAMESPERBUFFER;
	bufferNum = OEPA_BUFFERNUM;
	
	bufferInputIndex = 0;
	bufferOutputIndex = 0;
	
	audioOpen = false;
	audioStream = NULL;
	volume = 1.0F;
	instantVolume = 1.0F;
	volumeAlpha = 0.0F;
	timerThreadShouldRun = false;
	
	processThreadShouldRun = false;
	
	playing = false;
	recording = false;
}

void OEPA::setFullDuplex(bool value)
{
	bool state = disableAudio();
	
	fullDuplex = value;
	
	enableAudio(state);
}

void OEPA::setSampleRate(double value)
{
	stopPlaying();
	stopRecording();
	
	bool state = disableAudio();
	
	sampleRate = value;
	
	enableAudio(state);
}

void OEPA::setChannelNum(int value)
{
	stopPlaying();
	stopRecording();
	
	bool state = disableAudio();
	
	channelNum = value;
	
	enableAudio(state);
}

void OEPA::setFramesPerBuffer(int value)
{
	bool state = disableAudio();
	
	framesPerBuffer = value;
	
	enableAudio(state);
}

void OEPA::setBufferNum(int value)
{
	bool state = disableAudio();
	
	bufferNum = value;
	
	enableAudio(state);
}

void OEPA::setVolume(float value)
{
	volume = value;
}

void OEPA::setPlayThrough(bool value)
{
	playThrough = value;
}

bool OEPA::open()
{
	initBuffer();
	
	if (openProcess())
	{
		if (openAudio())
			return true;
		closeProcess();
	}
	
	return false;
}

void OEPA::close()
{
	closeAudio();
	closeProcess();
}

OEPAEmulation *OEPA::constructEmulation(string path)
{
	OEEmulation *emulation = new OEEmulation(path, resourcePath);
	if (!emulation)
		return NULL;
	
	pthread_mutex_lock(&processMutex);
	
	emulations.push_back(emulation);
	
	pthread_mutex_unlock(&processMutex);
	
	return emulation;
}

void OEPA::destroyEmulation(OEPAEmulation *emulation)
{
	pthread_mutex_lock(&processMutex);
	
	for (vector<OEPAEmulation *>::iterator i = emulations.begin();
		 i != emulations.end();
		 i++)
	{
		if ((*i) == emulation)
		{
			emulations.erase(i);
			break;
		}
	}
	
	pthread_mutex_unlock(&processMutex);
	
	delete emulation;
}


//
// Audio buffering
//

int OEPA::getBufferInputSize()
{
	int stateNum = 2 * bufferNum;
	int index = (bufferOutputIndex - bufferInputIndex + stateNum) % stateNum;
	return bufferNum - index;
}

int OEPA::getBufferOutputSize()
{
	int stateNum = 2 * bufferNum;
	int index = (bufferOutputIndex - bufferInputIndex + stateNum) % stateNum;
	return index;
}

float *OEPA::getBufferInput()
{
 	int index = bufferInputIndex % bufferNum;
	int samplesPerBuffer = framesPerBuffer * channelNum;
	return &buffer[index * samplesPerBuffer];
}

float *OEPA::getBufferOutput()
{
 	int index = bufferOutputIndex % bufferNum;
	int samplesPerBuffer = framesPerBuffer * channelNum;
	return &buffer[index * samplesPerBuffer];
}

void OEPA::incrementBufferInputIndex()
{
	int stateNum = 2 * bufferNum;
	bufferInputIndex = (bufferInputIndex + 1) % stateNum;
}

void OEPA::incrementBufferOutputIndex()
{
	int stateNum = 2 * bufferNum;
	bufferOutputIndex = (bufferOutputIndex + 1) % stateNum;
}

void OEPA::initBuffer()
{
	int bufferSize = bufferNum * framesPerBuffer * channelNum;
	buffer.resize(bufferSize);
	
	bufferInputIndex = 0;
	bufferOutputIndex = bufferNum;
}

//
// Audio
//

bool OEPA::openAudio()
{
	if (audioOpen)
		closeAudio();
	
	float t = 1.0 / sampleRate;
	float rc = 1 / 2.0 / M_PI / OEPA_VOLUMEFILTERFREQ;
	volumeAlpha = t / (rc + t);
	instantVolume = volume;
	
	int status = Pa_Initialize();
	if (status == paNoError)
	{
		status = Pa_OpenDefaultStream(&audioStream,
									  fullDuplex ? channelNum : 0,
									  channelNum,
									  paFloat32,
									  sampleRate,
									  framesPerBuffer,
									  oepaRunAudio,
									  NULL);
		if ((status != paNoError) && fullDuplex)
		{
			oepaLog("couldn't open audio stream (error " << status <<
					"), attempting half-duplex");
			
			status = Pa_OpenDefaultStream(&audioStream,
										  0,
										  channelNum,
										  paFloat32,
										  sampleRate,
										  framesPerBuffer,
										  oepaRunAudio,
										  NULL);
		}
		
		if (status == paNoError)
		{
			status = Pa_StartStream(audioStream);
			if (status == paNoError)
			{
				audioOpen = true;
				return true;
			}
			else
				oepaLog("couldn't start audio stream (error " << status << ")");
			
			Pa_CloseStream(audioStream);
		}
		else
			oepaLog("couldn't open audio stream (error " << status << ")");
	}
	else
		oepaLog("couldn't init portaudio (error " << status << ")");
	
	int error;
	pthread_attr_t attr;
	error = pthread_attr_init(&attr);
	if (!error)
	{
		error = pthread_attr_setdetachstate(&attr,
											PTHREAD_CREATE_JOINABLE);
		if (!error)
		{
			timerThreadShouldRun = true;
			error = pthread_create(&timerThread,
								   &attr,
								   oepaRunTimer,
								   NULL);
			if (!error)
			{
				oepaLog("started silent timer thread");
				audioOpen = true;
				return true;
			}
			else
				oepaLog("couldn't create timer thread (error " << error << ")");
		}
		else
			oepaLog("couldn't attr timer thread (error " << error << ")");
	}
	else
		oepaLog("couldn't init timer thread (error " << error << ")");
	
	return false;
}

void OEPA::closeAudio()
{
	if (!audioOpen)
		return;
	
	if (audioStream)
	{
		Pa_StopStream(audioStream);
		Pa_CloseStream(audioStream);
		
		audioStream = NULL;
	}
	
	if (timerThreadShouldRun)
	{
		timerThreadShouldRun = false;
		
		void *status;
		pthread_join(timerThread, &status);
	}
	
	audioOpen = false;
}

bool OEPA::disableAudio()
{
	bool state = audioOpen;
	
	if (state)
		closeAudio();
	
	lockProcess();
	
	return state;
}

void OEPA::enableAudio(bool state)
{
	initBuffer();
	
	unlockProcess();
	
	if (state)
		openAudio();
}

int OEPA::runAudio(const void *inputBuffer,
				   void *outputBuffer,
				   int frameNum)
{
	if ((getBufferOutputSize() <= 0) ||
		(frameNum != framesPerBuffer))
	{
		int samplesPerBuffer = frameNum * channelNum;
		float *out = (float *) outputBuffer;
		
		// To-Do: Replace volume with last energy average
		
		for (int i = 0; i < samplesPerBuffer; i++)
			*out++ = rand() * (0.1 / RAND_MAX);
		
		return paContinue;
	}
	
	int samplesPerBuffer = frameNum * channelNum;
	int bytesPerBuffer = samplesPerBuffer * sizeof(float);
	float *buffer = getBufferInput();
	
	// Output
	{
		float *in = buffer;
		float *out = (float *) outputBuffer;
		
		for (int i = 0; i < frameNum; i++)
		{
			for (int c = 0; c < channelNum; c++)
				*out++ = *in++ * instantVolume;
			
			instantVolume += (volume - instantVolume) * volumeAlpha;
		}
	}
	
	// Input
	if (inputBuffer)
		memcpy(buffer, inputBuffer, bytesPerBuffer);
	else
		memset(buffer, 0, bytesPerBuffer);
	
	incrementBufferInputIndex();
	
	return paContinue;
}

void OEPA::runTimer()
{
	while (timerThreadShouldRun)
	{
		usleep(1E6 * framesPerBuffer / sampleRate);
		
		if (getBufferOutputSize() > 0)
			incrementBufferInputIndex();
	}
}

//
// Emulation
//

bool OEPA::openProcess()
{
	int error;
	
	playing = false;
	recording = false;
	
	error = pthread_mutex_init(&processMutex, NULL);
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
				processThreadShouldRun = true;
				error = pthread_create(&processThread,
									   &attr,
									   oepaRunProcess,
									   NULL);
				if (!error)
					return true;
				else
					oepaLog("couldn't create process thread (error " <<
							error << ")");
			}
			else
				oepaLog("couldn't attr process thread (error " <<
						error << ")");
		}
		else
			oepaLog("couldn't init process attr (error " <<
					error << ")");
	}
	else
		oepaLog("couldn't init process mutex (error " <<
				error << ")");
	
	return false;
}

void OEPA::closeProcess()
{
	if (!processThreadShouldRun)
		return;
	
	processThreadShouldRun = false;
	void *status;
	pthread_join(processThread, &status);
	
	pthread_mutex_destroy(&processMutex);
}

bool OEPA::startPlaying(string path)
{
	stopPlaying();
	
	SF_INFO sfInfo = 
	{
		0,
		0,
		0,
		0,
		0,
		0,
	};
	
	lockProcess();
	
	playFrameIndex = 0;
	playFile = sf_open(path.c_str(), SFM_READ, &sfInfo);
	if (playFile)
	{
		playChannelNum = sfInfo.channels;
		
		int error;
		playSRCRatio = (double) sampleRate / sfInfo.samplerate;
		playSRC = src_new(SRC_SINC_FASTEST,
						  sfInfo.channels,
						  &error);
		
		if (playSRC)
		{
			playEnd = false;
			playBufferFrameBegin = 0;
			playBufferFrameEnd = 0;
			
			int frameNum = 2 * playSRCRatio * framesPerBuffer;
			playBuffer.resize(frameNum * playChannelNum);
			
			playFrameNum = sfInfo.frames * playSRCRatio;
			
			playing = true;
		}
		else
		{
			oepaLog("couldn't init sample rate converter (error " <<
					error << ")");
			sf_close(playFile);
		}
	}
	else
		oepaLog("couldn't open file " << path);
	
	unlockProcess();
	
	return playing;
}

void OEPA::stopPlaying()
{
	if (!playing)
		return;
	
	lockProcess();
	
	sf_close(playFile);
	playing = false;
	
	unlockProcess();
}

bool OEPA::isPlaying()
{
	return playing;
}

float OEPA::getPlayTime()
{
	return (float) playFrameIndex / sampleRate;
}

float OEPA::getPlayDuration()
{
	return (float) playFrameNum / sampleRate;
}

bool OEPA::startRecording(string path)
{
	stopRecording();
	
	SF_INFO sfInfo = 
	{
		0,
		sampleRate,
		channelNum,
		SF_FORMAT_WAV | SF_FORMAT_PCM_16,
		0,
		0,
	};
	
	lockProcess();
	
	recordingFrameNum = 0;
	recordingFile = sf_open(path.c_str(), SFM_WRITE, &sfInfo);
	
	if (recordingFile)
		recording = true;
	else
		oepaLog("couldn't open temporary file " << path);
	
	unlockProcess();
	
	return recording;
}

void OEPA::stopRecording()
{
	if (!recording)
		return;
	
	lockProcess();
	
	sf_close(recordingFile);
	recording = false;
	
	unlockProcess();
}

bool OEPA::isRecording()
{
	return recording;
}

float OEPA::getRecordingTime()
{
	return (float) recordingFrameNum / sampleRate;
}

long long OEPA::getRecordingSize()
{
	return (long long) recordingFrameNum * channelNum * sizeof(short);
}

void OEPA::lockProcess()
{
	if (processThreadShouldRun)
		pthread_mutex_lock(&processMutex);
}

void OEPA::unlockProcess()
{
	if (processThreadShouldRun)
		pthread_mutex_unlock(&processMutex);
}

void OEPA::playAudio(float *buffer, int frameNum, int channelNum)
{
	vector<float> srcBuffer;
	srcBuffer.resize(frameNum * playChannelNum);
	
	int playBufferFrameNum = playBuffer.size() /
		playChannelNum;
	
	do
	{
		if (playBufferFrameBegin >= playBufferFrameEnd)
		{
			int n = sf_readf_float(playFile,
								   &playBuffer[0],
								   playBufferFrameNum);
			
			playBufferFrameBegin = 0;
			playBufferFrameEnd = n;
			
			if (n != playBufferFrameNum)
				playEnd = true;
		}
		
		int index = playBufferFrameBegin * playChannelNum;
		int inputFrameNum = (playBufferFrameEnd - 
							 playBufferFrameBegin);
		SRC_DATA srcData =
		{
			&playBuffer[index],
			&srcBuffer[0],
			inputFrameNum,
			frameNum,
			0,
			0,
			playEnd,
			playSRCRatio,
		};
		src_process(playSRC, &srcData);
		
		if (!srcData.output_frames_gen)
		{
			sf_close(playFile);
			src_delete(playSRC);
			
			playing = false;
			
			return;
		}
		
		float *in = &srcBuffer[0];
		for (int i = 0; i < srcData.output_frames_gen; i++)
		{
			for (int ch = 0; ch < channelNum; ch++)
				buffer[ch] += in[ch % playChannelNum];
			
			in += playChannelNum;
			buffer += channelNum;
		}
		
		playFrameIndex += srcData.output_frames_gen;
		playBufferFrameBegin += srcData.input_frames_used;
		frameNum -= srcData.output_frames_gen;
	} while (frameNum > 0);
}

void OEPA::recordAudio(float *buffer, int frameNum, int channelNum)
{
	int n = sf_writef_float(recordingFile, buffer, frameNum);
	recordingFrameNum += n;
	
	if (frameNum != n)
	{
		sf_close(recordingFile);
		
		recording = false;
	}
}

void OEPA::runProcess()
{
	vector<float> inputBuffer;
	vector<float> outputBuffer;
	
	int cachedFramesPerBuffer = 0;
	int cachedChannelNum = 0;
	
	int samplesPerBuffer;
	int bytesPerBuffer;
	
	while (processThreadShouldRun)
	{
		if (getBufferInputSize() <= 0)
		{
			usleep(1000);
			continue;
		}
		
		pthread_mutex_lock(&processMutex);
		
		// Update audio format
		if ((framesPerBuffer != cachedFramesPerBuffer) ||
			(channelNum != cachedChannelNum))
		{
			cachedFramesPerBuffer = framesPerBuffer;
			cachedChannelNum = channelNum;
			
			samplesPerBuffer = framesPerBuffer * channelNum;
			bytesPerBuffer = samplesPerBuffer * sizeof(float);
			
			inputBuffer.resize(2 * samplesPerBuffer);
			memset(&inputBuffer[0], 0, 2 * bytesPerBuffer);
			outputBuffer.resize(2 * samplesPerBuffer);
			memset(&outputBuffer[0], 0, 2 * bytesPerBuffer);
		}
		
		float *buffer = getBufferOutput();
		
		// Input
		if (playing)
			playAudio(buffer, framesPerBuffer, channelNum);
		
		memcpy(&inputBuffer[0],
			   &inputBuffer[samplesPerBuffer],
			   bytesPerBuffer);
		memcpy(&inputBuffer[samplesPerBuffer],
			   buffer,
			   bytesPerBuffer);
		
		// Play through
		if (playThrough)
		{
			for (int i = 0; i < samplesPerBuffer; i++)
				outputBuffer[i] += inputBuffer[i];
		}
		
		// Output
		HostAudioBuffer hostAudioBuffer =
		{
			sampleRate,
			channelNum,
			framesPerBuffer,
			&inputBuffer[0],
			&outputBuffer[0],
		};
		
		for (vector<OEEmulation *>::iterator i = emulations.begin();
			 i != emulations.end();
			 i++)
		{
			OEComponent *component = (*i)->getComponent("host::host");
			if (!component)
				continue;
			
			component->postNotification(HOST_AUDIO_RENDER_WILL_BEGIN,
										&hostAudioBuffer);
			component->postNotification(HOST_AUDIO_RENDER_DID_BEGIN,
										&hostAudioBuffer);
			component->postNotification(HOST_AUDIO_RENDER_WILL_END,
										&hostAudioBuffer);
			component->postNotification(HOST_AUDIO_RENDER_DID_END,
										&hostAudioBuffer);
		}
		
		memcpy(buffer,
			   &outputBuffer[0],
			   bytesPerBuffer);
		memcpy(&outputBuffer[0],
			   &outputBuffer[samplesPerBuffer],
			   bytesPerBuffer);
		
		if (recording)
			recordAudio(buffer, framesPerBuffer, channelNum);
		
		pthread_mutex_unlock(&processMutex);
		
		incrementBufferOutputIndex();
	}
}

