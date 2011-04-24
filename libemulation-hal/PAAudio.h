
/**
 * libemulation-hal
 * PortAudio Audio
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a PortAudio audio component.
 */

#ifndef _PAAUDIO_H
#define _PAAUDIO_H

#include <pthread.h>

#include "portaudio.h"
#include "sndfile.h"
#include "samplerate.h"

#include "OEEmulation.h"

#define OEPORTAUDIO_SAMPLERATE			48000
#define OEPORTAUDIO_CHANNELNUM			2
#define OEPORTAUDIO_FRAMESPERBUFFER		512
#define OEPORTAUDIO_BUFFERNUM			3

class PAAudio : public OEComponent
{
public:
	PAAudio();
	
	void setFullDuplex(bool value);
	void setSampleRate(double value);
	void setChannelNum(int value);
	void setFramesPerBuffer(int value);
	void setBufferNum(int value);
	
	bool open();
	void close();
	
	void lockEmulations();
	void unlockEmulations();
	void runEmulations();
	bool addEmulation(OEEmulation *emulation);
	void removeEmulation(OEEmulation *emulation);
	
	void runAudio(const float *input,
				  float *output,
				  int frameCount);
	void runTimer();
	
	void setPlayVolume(float value);
	void setPlayThrough(bool value);
	void openPlayer(string path);
	void closePlayer();
	void setPlayPosition(float time);
	void play();
	void pause();
	bool isPlaying();
	float getPlayTime();
	float getPlayDuration();
	
	void openRecorder(string path);
	void closeRecorder();
	void record();
	void stop();
	bool isRecording();
	float getRecordingTime();
	long long getRecordingSize();
	
private:
	bool fullDuplex;
	double sampleRate;
	int channelNum;
	int framesPerBuffer;
	int bufferNum;
	
	volatile int bufferAudioIndex;
	volatile int bufferEmulationIndex;
	vector<float> bufferInput;
	vector<float> bufferOutput;
	
	bool emulationsThreadShouldRun;
	pthread_t emulationsThread;
	pthread_mutex_t emulationsMutex;
	pthread_cond_t emulationsCond;
	vector<OEEmulation *> emulations;
	
	bool audioOpen;
	PaStream *audioStream;
	bool timerThreadShouldRun;
	pthread_t timerThread;
	
	float playVolume;
	bool playThrough;
	SNDFILE *playSNDFILE;
	bool playing;
	long long playFrameIndex;
	long long playFrameNum;
	int playChannelNum;
	double playSRCRatio;
	SRC_STATE *playSRC;
	vector<float> playSRCBuffer;
	int playSRCBufferFrameBegin;
	int playSRCBufferFrameEnd;
	
	SNDFILE *recordingSNDFILE;
	bool recording;
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
	
	void playAudio(float *outputBuffer,
				   float *inputBuffer, int frameNum, int channelNum);
	
	void recordAudio(float *outputBuffer, int frameNum, int channelNum);
};

#endif
