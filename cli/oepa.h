
/**
 * OpenEmulator
 * OpenEmulator/portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/portaudio interface.
 */

#ifndef _OEPA_H
#define _OEPA_H

#include <pthread.h>

#include "OEEmulation.h"

#include "portaudio.h"
#include "sndfile.h"
#include "samplerate.h"

#define OEPA_SAMPLERATE			48000.0
#define OEPA_CHANNELNUM			2
#define OEPA_FRAMESPERBUFFER	512
#define OEPA_BUFFERNUM			3

#define OEPA_VOLUMEFILTERFREQ	20.0

class OEPAEmulation : public OEEmulation
{
public:
	OEPAEmulation(OEPA *oepa, string path, string resourcePath);
	~OEPAEmulation();
	
	bool isLoaded();
	bool save(string path);
	
	bool setProperty(string ref, string name, string value);
	bool getProperty(string ref, string name, string &value);
	void postNotification(string ref, int notification, void *data);
	int ioctl(string ref, int message, void *data);
	
	xmlDocPtr getDML();
	bool addDevices(string path, OEStringRefMap connections);
	bool isDeviceTerminal(OERef ref);
	bool removeDevice(OERef ref);
	
private:
	OEEmulation *emulation;
};

class OEPA
{
public:
	OEPA(string resourcePath);
	~OEPA();
	
	void setFullDuplex(bool value);
	void setSampleRate(double value);
	void setChannelNum(int value);
	void setFramesPerBuffer(int value);
	void setBufferNum(int value);
	
	void setVolume(float value);
	void setPlayThrough(bool value);
	
	bool open();
	void close();
	
	int runAudio(const void *inputBuffer,
				 void *outputBuffer,
				 int frameNum);
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
	
	bool addEmulation(OEPAEmulation *emulation);
	bool removeEmulation(OEPAEmulation *emulation);
	void runProcess();
	
private:
	string resourcePath;
	
	// Configuration
	bool fullDuplex;
	bool playThrough;
	double sampleRate;
	int channelNum;
	int framesPerBuffer;
	int bufferNum;
	
	// Audio buffers
	volatile int bufferInputIndex;
	volatile int bufferOutputIndex;
	vector<float> buffer;
	
	// Audio thread
	bool audioOpen;
	PaStream *audioStream;
	float volume;
	float instantVolume;
	float volumeAlpha;
	bool timerThreadShouldRun;
	pthread_t timerThread;
	
	// Process thread
	bool processThreadShouldRun;
	pthread_t processThread;
	pthread_mutex_t processMutex;
	vector<OEEmulation *> emulations;
	
	// Audio playback
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
	
	// Audio recording
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
