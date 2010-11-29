
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

#include "HostAudioInterface.h"

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
	sampleRate = OEPORTAUDIO_SAMPLERATE;
	channelNum = OEPORTAUDIO_CHANNELNUM;
	framesPerBuffer = OEPORTAUDIO_FRAMESPERBUFFER;
	bufferNum = OEPORTAUDIO_BUFFERNUM;
	
	audioOpen = false;
	pthread_cond_init(&emulationsCond, NULL);
	timerThreadShouldRun = false;
	
	emulationsThreadShouldRun = false;
	
	playVolume = 1.0F;
	playThrough = false;
	playSNDFILE = NULL;
	playing = false;
	recordingSNDFILE = NULL;
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
	closePlayer();
	closeRecorder();
	
	bool state = disableAudio();
	
	sampleRate = value;
	
	enableAudio(state);
}

void OEPortAudio::setChannelNum(int value)
{
	closePlayer();
	closeRecorder();
	
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
	int delta = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
	return delta <= 0;
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
	int delta = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
	return (bufferNum - delta) <= 0;
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
// Emulations
//
bool OEPortAudio::openEmulations()
{
	int error;
	
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
					log("could not create eulations thread, error " + getString(error));
			}
			else
				log("could not attr emulations thread, error " + getString(error));
		}
		else
			log("could not init emulations attr, error " + getString(error));
	}
	else
		log("could not init emulations mutex, error " + getString(error));
	
	return false;
}

void OEPortAudio::closeEmulations()
{
	if (!emulationsThreadShouldRun)
		return;
	
	emulationsThreadShouldRun = false;
	
	pthread_cond_signal(&emulationsCond);
	
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
		lockEmulations();
		
		if (isEmulationsBufferEmpty())
			pthread_cond_wait(&emulationsCond, &emulationsMutex);
		
		int samplesPerBuffer = framesPerBuffer * channelNum;
		int bytesPerBuffer = samplesPerBuffer * sizeof(float);
		
		float *inputBuffer = getEmulationsInputBuffer();
		float *outputBuffer = getEmulationsOutputBuffer();
		
		// Init output
		memset(outputBuffer, 0, bytesPerBuffer);
		
		// Audio play
		playAudio(outputBuffer, inputBuffer, framesPerBuffer, channelNum);
		
		// Output
		HostAudioBuffer buffer =
		{
			sampleRate,
			channelNum,
			framesPerBuffer,
			inputBuffer,
			outputBuffer,
		};
		
		notify(this, HOST_AUDIO_FRAME_WILL_BEGIN_RENDER, &buffer);
		notify(this, HOST_AUDIO_FRAME_DID_BEGIN_RENDER, &buffer);
		notify(this, HOST_AUDIO_FRAME_WILL_END_RENDER, &buffer);
		notify(this, HOST_AUDIO_FRAME_DID_END_RENDER, &buffer);
		
		// Audio recording
		recordAudio(outputBuffer, framesPerBuffer, channelNum);
		
		unlockEmulations();
		
		advanceEmulationsBuffer();
	}
}

bool OEPortAudio::addEmulation(OEEmulation *emulation)
{
	lockEmulations();
	
	emulations.push_back(emulation);
	
	unlockEmulations();
	
	return true;
}

void OEPortAudio::removeEmulation(OEEmulation *emulation)
{
	lockEmulations();
	
	OEEmulations::iterator first = emulations.begin();
	OEEmulations::iterator last = emulations.end();
	OEEmulations::iterator i = remove(first, last, emulation);
	
	if (i != last)
		emulations.erase(i, last);
	
	unlockEmulations();
}

//
// Audio
//
bool OEPortAudio::openAudio()
{
	if (audioOpen)
		closeAudio();
	
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
			log("could not open audio stream, error " + getString(status));
			log("attempting half-duplex");
			
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
				log("could not start audio stream, error " + getString(status));
			
			Pa_CloseStream(audioStream);
		}
		else
			log("could not open audio stream, error " + getString(status));
	}
	else
		log("could not init portaudio, error " + getString(status));
	
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
				log("started timer thread");
				audioOpen = true;
				return true;
			}
			else
				log("could not create timer thread, error " + getString(error));
		}
		else
			log("could not attr timer thread, error " + getString(error));
	}
	else
		log("could not init timer thread, error " + getString(error));
	
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
	
	// Render comfort noise when no data is available
	if (isAudioBufferEmpty() || (frameCount != framesPerBuffer))
	{
		float k = 0.1 / RAND_MAX;
		
		for (int i = 0; i < samplesPerBuffer; i++)
			*output++ = k * rand();
		
		return;
	}
	
	// Copy input buffer
	float *inputBuffer = getAudioInputBuffer();
	if (input)
		memcpy(inputBuffer, input, bytesPerBuffer);
	else
		memset(inputBuffer, 0, bytesPerBuffer);
	
	// Copy output buffer
	float *outputBuffer = getAudioOutputBuffer();
	memcpy(output, outputBuffer, bytesPerBuffer);
	
	advanceAudioBuffer();
	
	pthread_cond_signal(&emulationsCond);
	
	return;
}

void OEPortAudio::runTimer()
{
	while (timerThreadShouldRun)
	{
		usleep(1E6 * framesPerBuffer / sampleRate);
		
		if (isAudioBufferEmpty())
			continue;
		
		advanceAudioBuffer();
		
		pthread_cond_signal(&emulationsCond);
	}
}

//
// Play
//
void OEPortAudio::setPlayVolume(float value)
{
	playVolume = value;
}

void OEPortAudio::setPlayThrough(bool value)
{
	playThrough = value;
}

void OEPortAudio::openPlayer(string path)
{
	closePlayer();
	
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
	
	playSNDFILE = sf_open(path.c_str(), SFM_READ, &sfInfo);
	if (playSNDFILE)
	{
		playChannelNum = sfInfo.channels;
		
		int error;
		playSRCRatio = (double) sampleRate / sfInfo.samplerate;
		playSRC = src_new(SRC_SINC_FASTEST,
						  sfInfo.channels,
						  &error);
		
		if (playSRC)
		{
			playSRCBufferFrameBegin = 0;
			playSRCBufferFrameEnd = 0;
			
			int frameNum = 2 * playSRCRatio * framesPerBuffer;
			playSRCBuffer.resize(frameNum * playChannelNum);
			
			playFrameIndex = 0;
			playFrameNum = sfInfo.frames * playSRCRatio;
		}
		else
		{
			log("could not init sample rate converter, error " + getString(error));
			sf_close(playSNDFILE);
		}
	}
	else
		log("could not open file " + path);
	
	unlockEmulations();
}

void OEPortAudio::closePlayer()
{
	if (!playSNDFILE)
		return;
	
	lockEmulations();
	
	sf_close(playSNDFILE);
	playSNDFILE = NULL;
	playing = false;
	playFrameIndex = 0;
	playFrameNum = 0;
	
	unlockEmulations();
}

void OEPortAudio::setPlayPosition(float time)
{
	if (!playSNDFILE)
		return;
	
	lockEmulations();
	
	playFrameIndex = time * sampleRate;
	sf_seek(playSNDFILE, playFrameIndex / playSRCRatio, SEEK_SET);

	if (!playing)
	{
		playSRCBufferFrameBegin = 0;
		playSRCBufferFrameEnd = 0;
	}
	
	unlockEmulations();
}

void OEPortAudio::play()
{
	if (playSNDFILE)
		playing = true;
	
	if ((getPlayTime() + 0.1) >= getPlayDuration())
		setPlayPosition(0.0);
}

void OEPortAudio::pause()
{
	if (playSNDFILE)
		playing = false;
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

void OEPortAudio::playAudio(float *outputBuffer,
							float *inputBuffer, int frameNum, int channelNum)
{
	if (!playing)
		return;
	
	vector<float> srcBuffer;
	srcBuffer.resize(frameNum * playChannelNum);
	
	int playSRCBufferFrameNum = playSRCBuffer.size() / playChannelNum;
	
	do
	{
		if (playSRCBufferFrameBegin >= playSRCBufferFrameEnd)
		{
			int n = sf_readf_float(playSNDFILE,
								   &playSRCBuffer.front(),
								   playSRCBufferFrameNum);
			
			playSRCBufferFrameBegin = 0;
			playSRCBufferFrameEnd = n;
			
			if (n != playSRCBufferFrameNum)
				playing = false;
		}
		
		int index = playSRCBufferFrameBegin * playChannelNum;
		int inputFrameNum = (playSRCBufferFrameEnd - 
							 playSRCBufferFrameBegin);
		SRC_DATA srcData =
		{
			&playSRCBuffer[index],
			&srcBuffer.front(),
			inputFrameNum,
			frameNum,
			0,
			0,
			0,
			playSRCRatio,
		};
		src_process(playSRC, &srcData);
		
		if (!srcData.output_frames_gen)
		{
			playing = false;
			return;
		}
		
		float *in = &srcBuffer.front();
		float linearVolume = 20.0 * pow(10.0, (playVolume - 1.0) * 100.0 / 20.0);
		for (int i = 0; i < srcData.output_frames_gen; i++)
		{
			for (int ch = 0; ch < channelNum; ch++)
				inputBuffer[ch] += in[ch % playChannelNum] * linearVolume;
			
			in += playChannelNum;
			inputBuffer += channelNum;
		}

		if (playThrough)
		{
			float *in = &srcBuffer.front();
			for (int i = 0; i < srcData.output_frames_gen; i++)
			{
				for (int ch = 0; ch < channelNum; ch++)
					outputBuffer[ch] += in[ch % playChannelNum] * playVolume;
				
				in += playChannelNum;
				outputBuffer += channelNum;
			}
		}
		
		playFrameIndex += srcData.output_frames_gen;
		playSRCBufferFrameBegin += srcData.input_frames_used;
		
		frameNum -= srcData.output_frames_gen;
	} while (frameNum > 0);
}

//
// Recording
//
void OEPortAudio::openRecorder(string path)
{
	closeRecorder();
	
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
	
	if (!(recordingSNDFILE = sf_open(path.c_str(), SFM_WRITE, &sfInfo)))
		log("could not open temporary file " + path);
	recordingFrameNum = 0;
	
	unlockEmulations();
}

void OEPortAudio::closeRecorder()
{
	if (!recordingSNDFILE)
		return;
	
	lockEmulations();
	
	sf_close(recordingSNDFILE);
	recordingSNDFILE = NULL;
	recording = false;
	
	unlockEmulations();
}

void OEPortAudio::record()
{
	if (recordingSNDFILE)
		recording = true;
}

void OEPortAudio::stop()
{
	if (recordingSNDFILE)
		recording = false;
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

void OEPortAudio::recordAudio(float *outputBuffer, int frameNum, int channelNum)
{
	if (!recording)
		return;
	
	int n = sf_writef_float(recordingSNDFILE, outputBuffer, frameNum);
	recordingFrameNum += n;
	
	if (frameNum != n)
	{
		sf_close(recordingSNDFILE);
		recording = false;
		recordingSNDFILE = NULL;
		recordingFrameNum = 0;
	}
}
