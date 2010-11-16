
/**
 * OpenEmulator
 * OpenEmulator portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the OpenEmulator portaudio interface.
 */

#ifndef _OEPORTAUDIO_H
#define _OEPORTAUDIO_H

#include <pthread.h>

#include "portaudio.h"
#include "sndfile.h"
#include "samplerate.h"

#include "OEPortAudioEmulation.h"

#define OEPORTAUDIO_SAMPLERATE			48000.0
#define OEPORTAUDIO_CHANNELNUM			2
#define OEPORTAUDIO_FRAMESPERBUFFER		512
#define OEPORTAUDIO_BUFFERNUM			3

#define OEPORTAUDIO_VOLUMEFILTERFREQ	20.0

typedef vector<OEPortAudioEmulation *> OEPortAudioEmulations;

class OEPortAudio : public OEComponent
{
public:
	OEPortAudio();
	
	void setFullDuplex(bool value);
	void setSampleRate(double value);
	void setChannelNum(int value);
	void setFramesPerBuffer(int value);
	void setBufferNum(int value);
	void setVolume(float value);
	void setPlayThrough(bool value);
	
	bool open();
	void close();
	
	bool addEmulation(OEPortAudioEmulation *emulation);
	void removeEmulation(OEPortAudioEmulation *emulation);
	
	void lockEmulations();
	void unlockEmulations();
	void runEmulations();
	
	void runAudio(const float *input,
				  float *output,
				  int frameCount);
	void runTimer();
	
	bool startPlaying(string path);
	void stopPlaying();
	bool isPlaying();
	float getPlayTime();
	float getPlayDuration();
	bool startRecording(string path);
	void stopRecording();
	bool isRecording();
	float getRecordingTime();
	long long getRecordingSize();
	
private:
	bool fullDuplex;
	bool playThrough;
	double sampleRate;
	int channelNum;
	int framesPerBuffer;
	int bufferNum;
	float volume;
	
	volatile int bufferAudioIndex;
	volatile int bufferEmulationIndex;
	vector<float> bufferInput;
	vector<float> bufferOutput;
	
	bool audioOpen;
	PaStream *audioStream;
	float instantVolume;
	bool timerThreadShouldRun;
	pthread_t timerThread;
	
	bool emulationsThreadShouldRun;
	pthread_t emulationsThread;
	pthread_mutex_t emulationsMutex;
	pthread_cond_t emulationsCond;
	OEPortAudioEmulations emulations;
	
	bool playing;
	SNDFILE *playFile;
	long long playFrameIndex;
	long long playFrameNum;
	int playChannelNum;
	double playSRCRatio;
	SRC_STATE *playSRC;
	bool playEnd;
	vector<float> playBuffer;
	int playBufferFrameBegin;
	int playBufferFrameEnd;
	
	bool recording;
	SNDFILE *recordingFile;
	long long recordingFrameNum;
	
	void initBuffer();
	bool isAudioBufferEmpty();
	float *getAudioInputBuffer();
	float *getAudioOutputBuffer();
	void advanceAudioBuffer();
	bool isEmulationsBufferEmpty();
	float *getEmulationsInputBuffer();
	float *getEmulationsOutputBuffer();
	void advanceEmulationsBuffer();
	
	bool openAudio();
	void closeAudio();
	bool disableAudio();
	void enableAudio(bool state);
	
	bool openEmulations();
	void closeEmulations();
	
	void playAudio(float *buffer, int frameNum, int channelNum);
	void recordAudio(float *buffer, int frameNum, int channelNum);
};

#endif
