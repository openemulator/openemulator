
/**
 * OpenEmulator
 * OpenEmulator portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator portaudio interface.
 */

#ifndef _OEPA_H
#define _OEPA_H

#include <pthread.h>
#include "portaudio.h"
#include "sndfile.h"
#include "samplerate.h"

#include "OEPAEmulation.h"

#define OEPA_SAMPLERATE			48000.0
#define OEPA_CHANNELNUM			2
#define OEPA_FRAMESPERBUFFER	512
#define OEPA_BUFFERNUM			3

#define OEPA_VOLUMEFILTERFREQ	20.0

#define oepaLog(text) cerr << "oepa: " << text << endl

class OEPA
{
public:
	OEPA();
	
	void setFullDuplex(bool value);
	void setSampleRate(double value);
	void setChannelNum(int value);
	void setFramesPerBuffer(int value);
	void setBufferNum(int value);
	
	void setVolume(float value);
	void setPlayThrough(bool value);
	
	bool open();
	void close();
	bool addEmulation(OEPAEmulation *emulation);
	void removeEmulation(OEPAEmulation *emulation);
	
	void runAudio(const void *inputBuffer,
				  void *outputBuffer,
				  int frameNum);
	void runTimer();
	
	void lockProcess();
	void unlockProcess();
	void runProcess();
	
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
	
	volatile int bufferInputIndex;
	volatile int bufferOutputIndex;
	vector<float> buffer;
	
	bool audioOpen;
	PaStream *audioStream;
	float volume;
	float instantVolume;
	float volumeAlpha;
	bool timerThreadShouldRun;
	pthread_t timerThread;
	
	bool processThreadShouldRun;
	pthread_t processThread;
	pthread_mutex_t processMutex;
	vector<OEPAEmulation *> emulations;
	
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
	
	int getBufferInputSize();
	int getBufferOutputSize();
	float *getBufferInput();
	float *getBufferOutput();
	void incrementBufferInputIndex();
	void incrementBufferOutputIndex();
	void initBuffer();
	
	bool openAudio();
	void closeAudio();
	bool disableAudio();
	void enableAudio(bool state);
	
	bool openProcess();
	void closeProcess();
	
	void playAudio(float *buffer, int frameNum, int channelNum);
	void recordAudio(float *buffer, int frameNum, int channelNum);
};

#endif
