
/**
 * OpenEmulator
 * OpenEmulator portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator portaudio interface.
 */

#include <math.h>
#include <iostream>

#include "OEPortAudio.h"

#include "HostInterface.h"

using namespace std;

static int oePortAudioRun(const void *inputBuffer,
						  void *outputBuffer,
						  unsigned long framesPerBuffer,
						  const PaStreamCallbackTimeInfo* timeInfo,
						  PaStreamCallbackFlags statusFlags,
						  void *userData)
{
	((OEPortAudio *) userData)->runAudio(inputBuffer,
										 outputBuffer,
										 framesPerBuffer);
	
	return paContinue;
}

void *oePortAudioRunAudioTimer(void *arg)
{
	((OEPortAudio *) arg)->runAudioTimer();
	
	return NULL;
}

void *oepaRunEmulations(void *arg)
{
	((OEPortAudio *) arg)->runEmulations();
	
	return NULL;
}

OEPA::OEPA()
{
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
	
	emulationsThreadShouldRun = false;
	
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
	
	if (openEmulations())
	{
		if (openAudio())
			return true;
		
		closeEmulations();
	}
	
	return false;
}

void OEPA::close()
{
	closeAudio();
	closeEmulations();
}

bool OEPA::addEmulation(OEPAEmulation *emulation)
{
	lockEmulations();
	
	emulations.push_back(emulation);
	
	unlockEmulations();
	
	return true;
}

void OEPA::removeEmulation(OEPAEmulation *emulation)
{
	lockEmulations();
	
	OEPAEmulations::iterator first = emulations.begin();
	OEPAEmulations::iterator last = emulations.end();
	OEPAEmulations::iterator i = remove(first, last, emulation);
	
	if (i != last)
		emulations.erase(i, last);
	
	unlockEmulations();
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
									  this);
		if ((status != paNoError) && fullDuplex)
		{
			OEPALog("could not open audio stream, error " + status);
			OEPALog("attempting half-duplex");
			
			status = Pa_OpenDefaultStream(&audioStream,
										  0,
										  channelNum,
										  paFloat32,
										  sampleRate,
										  framesPerBuffer,
										  oepaRunAudio,
										  this);
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
				OEPALog("could not start audio stream, error " + status);
			
			Pa_CloseStream(audioStream);
		}
		else
			OEPALog("could not open audio stream, error " + status);
	}
	else
		OEPALog("could not init portaudio, error " + status);
	
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
								   oepaRunAudioTimer,
								   this);
			if (!error)
			{
				OEPALog("started silent timer thread");
				audioOpen = true;
				return true;
			}
			else
				OEPALog("could not create timer thread, error " + error);
		}
		else
			OEPALog("could not attr timer thread, error " + error);
	}
	else
		OEPALog("could not init timer thread, error " + error);
	
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
	
	lockEmulations();
	
	return state;
}

void OEPA::enableAudio(bool state)
{
	initBuffer();
	
	unlockEmulations();
	
	if (state)
		openAudio();
}

void OEPA::runAudio(const void *inputBuffer,
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
		
		return;
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
	
	return;
}

void OEPA::runAudioTimer()
{
	while (timerThreadShouldRun)
	{
		usleep(1E6 * framesPerBuffer / sampleRate);
		
		if (getBufferOutputSize() > 0)
			incrementBufferInputIndex();
	}
}

//
// Emulations
//
bool OEPA::openEmulations()
{
	int error;
	
	playing = false;
	recording = false;
	
	error = pthread_mutex_init(&emulationsMutex, NULL);
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
				int curr = param.sched_priority;
				int max = sched_get_priority_max(SCHED_RR);
				
				param.sched_priority += (max - curr) / 2;
				pthread_attr_setschedparam(&attr, &param);
			}
			
			error = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
			if (!error)
			{
				emulationsThreadShouldRun = true;
				error = pthread_create(&emulationsThread,
									   &attr,
									   oepaRunEmulations,
									   this);
				if (!error)
					return true;
				else
					OEPALog("could not create eulations thread, error " + error);
			}
			else
				OEPALog("could not attr emulations thread, error " + error);
		}
		else
			OEPALog("could not init emulations attr, error " + error);
	}
	else
		OEPALog("could not init emulations mutex, error " + error);
	
	return false;
}

void OEPA::closeEmulations()
{
	if (!emulationsThreadShouldRun)
		return;
	
	emulationsThreadShouldRun = false;
	void *status;
	pthread_join(emulationsThread, &status);
	
	pthread_mutex_destroy(&emulationsMutex);
}

void OEPA::lockEmulations()
{
	pthread_mutex_lock(&emulationsMutex);
}

void OEPA::unlockEmulations()
{
	pthread_mutex_unlock(&emulationsMutex);
}

void OEPA::runEmulations()
{
	vector<float> inputBuffer;
	vector<float> outputBuffer;
	
	int cachedFramesPerBuffer = 0;
	int cachedChannelNum = 0;
	
	int samplesPerBuffer;
	int bytesPerBuffer;
	
	while (emulationsThreadShouldRun)
	{
		if (getBufferInputSize() <= 0)
		{
			usleep(1000);
			continue;
		}
		
		lockEmulations();
		
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
		HostAudioNotification hostAudioBuffer =
		{
			sampleRate,
			channelNum,
			framesPerBuffer,
			&inputBuffer[0],
			&outputBuffer[0],
		};
		
		for (OEPAEmulations::iterator i = emulations.begin();
			 i != emulations.end();
			 i++)
		{
			OEComponent *component = (*i)->getComponent("");
			if (!component)
				continue;
			
			component->notify(NULL, HOST_AUDIO_FRAME_WILL_BEGIN,
							  &hostAudioBuffer);
			component->notify(NULL, HOST_AUDIO_FRAME_WILL_RENDER,
							  &hostAudioBuffer);
			component->notify(NULL, HOST_AUDIO_FRAME_WILL_END,
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
		
		unlockEmulations();
		
		incrementBufferOutputIndex();
	}
}

//
// Play/Recording
//
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
	
	lockEmulations();
	
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
			OEPALog("could not init sample rate converter, error " + error);
			sf_close(playFile);
		}
	}
	else
		OEPALog("could not open file " + path);
	
	unlockEmulations();
	
	return playing;
}

void OEPA::stopPlaying()
{
	if (!playing)
		return;
	
	lockEmulations();
	
	sf_close(playFile);
	playing = false;
	
	unlockEmulations();
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
	
	lockEmulations();
	
	recordingFrameNum = 0;
	recordingFile = sf_open(path.c_str(), SFM_WRITE, &sfInfo);
	
	if (recordingFile)
		recording = true;
	else
		OEPALog("could not open temporary file " + path);
	
	unlockEmulations();
	
	return recording;
}

void OEPA::stopRecording()
{
	if (!recording)
		return;
	
	lockEmulations();
	
	sf_close(recordingFile);
	recording = false;
	
	unlockEmulations();
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

void OEPA::playAudio(float *buffer, int frameNum, int channelNum)
{
	vector<float> srcBuffer;
	srcBuffer.resize(frameNum * playChannelNum);
	
	int playBufferFrameNum = playBuffer.size() / playChannelNum;
	
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
