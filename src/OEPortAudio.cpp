
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

static int oePortAudioRunAudio(const void *input,
							   void *output,
							   unsigned long frameCount,
							   const PaStreamCallbackTimeInfo* timeInfo,
							   PaStreamCallbackFlags statusFlags,
							   void *userData)
{
	((OEPortAudio *) userData)->runAudio((const float *)input,
										 (float *)output,
										 frameCount);
	
	return paContinue;
}

void *oePortAudioRunTimer(void *arg)
{
	((OEPortAudio *) arg)->runTimer();
	
	return NULL;
}

void *oePortAudioRunEmulations(void *arg)
{
	((OEPortAudio *) arg)->runEmulations();
	
	return NULL;
}

OEPortAudio::OEPortAudio()
{
	fullDuplex = false;
	playThrough = false;
	sampleRate = OEPORTAUDIO_SAMPLERATE;
	channelNum = OEPORTAUDIO_CHANNELNUM;
	framesPerBuffer = OEPORTAUDIO_FRAMESPERBUFFER;
	bufferNum = OEPORTAUDIO_BUFFERNUM;
	
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

void OEPortAudio::setFullDuplex(bool value)
{
	bool state = disableAudio();
	
	fullDuplex = value;
	
	enableAudio(state);
}

void OEPortAudio::setSampleRate(double value)
{
	stopPlaying();
	stopRecording();
	
	bool state = disableAudio();
	
	sampleRate = value;
	
	enableAudio(state);
}

void OEPortAudio::setChannelNum(int value)
{
	stopPlaying();
	stopRecording();
	
	bool state = disableAudio();
	
	channelNum = value;
	
	enableAudio(state);
}

void OEPortAudio::setFramesPerBuffer(int value)
{
	bool state = disableAudio();
	
	framesPerBuffer = value;
	
	enableAudio(state);
}

void OEPortAudio::setBufferNum(int value)
{
	bool state = disableAudio();
	
	bufferNum = value;
	
	enableAudio(state);
}

void OEPortAudio::setVolume(float value)
{
	volume = value;
}

void OEPortAudio::setPlayThrough(bool value)
{
	playThrough = value;
}

bool OEPortAudio::open()
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

void OEPortAudio::close()
{
	closeAudio();
	closeEmulations();
}

bool OEPortAudio::addEmulation(OEPortAudioEmulation *emulation)
{
	lockEmulations();
	
	emulations.push_back(emulation);
	
	unlockEmulations();
	
	return true;
}

void OEPortAudio::removeEmulation(OEPortAudioEmulation *emulation)
{
	lockEmulations();
	
	OEPortAudioEmulations::iterator first = emulations.begin();
	OEPortAudioEmulations::iterator last = emulations.end();
	OEPortAudioEmulations::iterator i = remove(first, last, emulation);
	
	if (i != last)
		emulations.erase(i, last);
	
	unlockEmulations();
}

//
// Audio buffering
//
void OEPortAudio::initBuffer()
{
	int bufferSize = bufferNum * framesPerBuffer * channelNum;
	bufferInput.resize(bufferSize);
	bufferOutput.resize(bufferSize);
	
	bufferAudioIndex = 0;
	bufferEmulationIndex = bufferNum;
}

bool OEPortAudio::isAudioBufferEmpty()
{
	int stateNum = 2 * bufferNum;
	int index = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
	return (bufferNum - index) <= 0;
}

float *OEPortAudio::getAudioInputBuffer()
{
 	int index = bufferAudioIndex % bufferNum;
	int samplesPerBuffer = framesPerBuffer * channelNum;
	return &bufferInput[index * samplesPerBuffer];
}

float *OEPortAudio::getAudioOutputBuffer()
{
 	int index = bufferAudioIndex % bufferNum;
	int samplesPerBuffer = framesPerBuffer * channelNum;
	return &bufferOutput[index * samplesPerBuffer];
}

void OEPortAudio::advanceAudioBuffer()
{
	int stateNum = 2 * bufferNum;
	bufferAudioIndex = (bufferAudioIndex + 1) % stateNum;
}

bool OEPortAudio::isEmulationsBufferEmpty()
{
	int stateNum = 2 * bufferNum;
	int index = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
	return index <= 0;
}

float *OEPortAudio::getEmulationsInputBuffer()
{
 	int index = bufferEmulationIndex % bufferNum;
	int samplesPerBuffer = framesPerBuffer * channelNum;
	return &bufferInput[index * samplesPerBuffer];
}

float *OEPortAudio::getEmulationsOutputBuffer()
{
 	int index = bufferEmulationIndex % bufferNum;
	int samplesPerBuffer = framesPerBuffer * channelNum;
	return &bufferOutput[index * samplesPerBuffer];
}

void OEPortAudio::advanceEmulationsBuffer()
{
	int stateNum = 2 * bufferNum;
	bufferEmulationIndex = (bufferEmulationIndex + 1) % stateNum;
}

//
// Audio
//
bool OEPortAudio::openAudio()
{
	if (audioOpen)
		closeAudio();
	
	float t = 1.0 / sampleRate;
	float rc = 1 / 2.0 / M_PI / OEPORTAUDIO_VOLUMEFILTERFREQ;
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
									  oePortAudioRunAudio,
									  this);
		if ((status != paNoError) && fullDuplex)
		{
			OEPortAudioLog("could not open audio stream, error " + status);
			OEPortAudioLog("attempting half-duplex");
			
			status = Pa_OpenDefaultStream(&audioStream,
										  0,
										  channelNum,
										  paFloat32,
										  sampleRate,
										  framesPerBuffer,
										  oePortAudioRunAudio,
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
				OEPortAudioLog("could not start audio stream, error " + status);
			
			Pa_CloseStream(audioStream);
		}
		else
			OEPortAudioLog("could not open audio stream, error " + status);
	}
	else
		OEPortAudioLog("could not init portaudio, error " + status);
	
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
								   oePortAudioRunTimer,
								   this);
			if (!error)
			{
				OEPortAudioLog("started timer thread");
				audioOpen = true;
				return true;
			}
			else
				OEPortAudioLog("could not create timer thread, error " + error);
		}
		else
			OEPortAudioLog("could not attr timer thread, error " + error);
	}
	else
		OEPortAudioLog("could not init timer thread, error " + error);
	
	return false;
}

void OEPortAudio::closeAudio()
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

bool OEPortAudio::disableAudio()
{
	bool state = audioOpen;
	
	if (state)
		closeAudio();
	
	lockEmulations();
	
	return state;
}

void OEPortAudio::enableAudio(bool state)
{
	initBuffer();
	
	unlockEmulations();
	
	if (state)
		openAudio();
}

void OEPortAudio::runAudio(const float *input,
					float *output,
					int frameCount)
{
	int samplesPerBuffer = frameCount * channelNum;
	int bytesPerBuffer = samplesPerBuffer * sizeof(float);
	
	if (isAudioBufferEmpty() ||
		(frameCount != framesPerBuffer))
	{
		// To-Do: Replace volume with last energy average
		// Decrease volume
		for (int i = 0; i < samplesPerBuffer; i++)
			*output++ = rand() * (0.1 / RAND_MAX);
		
		return;
	}
	
	float *inputBuffer = getAudioInputBuffer();
	float *outputBuffer = getAudioOutputBuffer();
	
	// Input
	if (input)
		memcpy(inputBuffer, input, bytesPerBuffer);
	else
		memset(inputBuffer, 0, bytesPerBuffer);
	
	// Output
	{
		float volumeDifference = (volume - instantVolume);
		
		if ((volumeDifference < -0.0001) || (volumeDifference > 0.0001))
		{
			for (int i = 0; i < frameCount; i++)
			{
				for (int c = 0; c < channelNum; c++)
					*output++ = *outputBuffer++ * instantVolume;
				
				instantVolume += volumeDifference * volumeAlpha;
			}
		}
		else
			for (int i = 0; i < samplesPerBuffer; i++)
				*output++ = *outputBuffer++ * instantVolume;
	}
	
	advanceAudioBuffer();
	
	return;
}

void OEPortAudio::runTimer()
{
	while (timerThreadShouldRun)
	{
		usleep(1E6 * framesPerBuffer / sampleRate);
		
		if (!isAudioBufferEmpty())
			advanceAudioBuffer();
	}
}

//
// Emulations
//
bool OEPortAudio::openEmulations()
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
									   oePortAudioRunEmulations,
									   this);
				if (!error)
					return true;
				else
					OEPortAudioLog("could not create eulations thread, error " + error);
			}
			else
				OEPortAudioLog("could not attr emulations thread, error " + error);
		}
		else
			OEPortAudioLog("could not init emulations attr, error " + error);
	}
	else
		OEPortAudioLog("could not init emulations mutex, error " + error);
	
	return false;
}

void OEPortAudio::closeEmulations()
{
	if (!emulationsThreadShouldRun)
		return;
	
	emulationsThreadShouldRun = false;
	void *status;
	pthread_join(emulationsThread, &status);
	
	pthread_mutex_destroy(&emulationsMutex);
}

void OEPortAudio::lockEmulations()
{
	pthread_mutex_lock(&emulationsMutex);
}

void OEPortAudio::unlockEmulations()
{
	pthread_mutex_unlock(&emulationsMutex);
}

void OEPortAudio::runEmulations()
{
	while (emulationsThreadShouldRun)
	{
		// To-Do: use a condition
		if (isEmulationsBufferEmpty())
		{
			usleep(1000);
			continue;
		}
		
		lockEmulations();
		
		float *inputBuffer = getEmulationsInputBuffer();
		float *outputBuffer = getEmulationsOutputBuffer();
		
		// Audio play
		if (playing)
			playAudio(inputBuffer, framesPerBuffer, channelNum);
		
		// Output
		HostAudioNotification hostAudioBuffer =
		{
			sampleRate,
			channelNum,
			framesPerBuffer,
			inputBuffer,
			outputBuffer,
		};
		
		for (OEPortAudioEmulations::iterator i = emulations.begin();
			 i != emulations.end();
			 i++)
		{
			// To-Do: Request audio components and iterate
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
		
		// Audio recording
		if (recording)
			recordAudio(outputBuffer, framesPerBuffer, channelNum);
		
		// Audio play through
		if (playThrough)
		{
			int samplesPerBuffer = framesPerBuffer * channelNum;
			
			for (int i = 0; i < samplesPerBuffer; i++)
				outputBuffer[i] += inputBuffer[i];
		}
		
		unlockEmulations();
		
		advanceEmulationsBuffer();
	}
}

//
// Play/Recording
//
bool OEPortAudio::startPlaying(string path)
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
			OEPortAudioLog("could not init sample rate converter, error " + error);
			sf_close(playFile);
		}
	}
	else
		OEPortAudioLog("could not open file " + path);
	
	unlockEmulations();
	
	return playing;
}

void OEPortAudio::stopPlaying()
{
	if (!playing)
		return;
	
	lockEmulations();
	
	sf_close(playFile);
	playing = false;
	
	unlockEmulations();
}

bool OEPortAudio::isPlaying()
{
	return playing;
}

float OEPortAudio::getPlayTime()
{
	return (float) playFrameIndex / sampleRate;
}

float OEPortAudio::getPlayDuration()
{
	return (float) playFrameNum / sampleRate;
}

bool OEPortAudio::startRecording(string path)
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
		OEPortAudioLog("could not open temporary file " + path);
	
	unlockEmulations();
	
	return recording;
}

void OEPortAudio::stopRecording()
{
	if (!recording)
		return;
	
	lockEmulations();
	
	sf_close(recordingFile);
	recording = false;
	
	unlockEmulations();
}

bool OEPortAudio::isRecording()
{
	return recording;
}

float OEPortAudio::getRecordingTime()
{
	return (float) recordingFrameNum / sampleRate;
}

long long OEPortAudio::getRecordingSize()
{
	return (long long) recordingFrameNum * channelNum * sizeof(short);
}

void OEPortAudio::playAudio(float *buffer, int frameNum, int channelNum)
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

void OEPortAudio::recordAudio(float *buffer, int frameNum, int channelNum)
{
	int n = sf_writef_float(recordingFile, buffer, frameNum);
	recordingFrameNum += n;
	
	if (frameNum != n)
	{
		sf_close(recordingFile);
		
		recording = false;
	}
}
