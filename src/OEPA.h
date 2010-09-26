
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

inline void OEPALog(string text)
{
	cerr << "oepa: " << text << endl;
}

typedef vector<OEPAEmulation *> OEPAEmulations;

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
	void runAudioTimer();
	
	void lockEmulations();
	void unlockEmulations();
	void runEmulations();
	
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
	
	bool emulationsThreadShouldRun;
	pthread_t emulationsThread;
	pthread_mutex_t emulationsMutex;
	OEPAEmulations emulations;
	
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
	
	bool openEmulations();
	void closeEmulations();
	
	void playAudio(float *buffer, int frameNum, int channelNum);
	void recordAudio(float *buffer, int frameNum, int channelNum);
};

#endif
