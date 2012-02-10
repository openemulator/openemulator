
/**
 * libemulation-hal
 * PortAudio audio
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a PortAudio audio component
 */

#ifndef _PAAUDIO_H
#define _PAAUDIO_H

#include <pthread.h>

#include "portaudio.h"
#include "sndfile.h"
#include "samplerate.h"

#include "OEEmulation.h"

class PAAudio : public OEComponent
{
public:
    PAAudio();
    ~PAAudio();
    
    void setFullDuplex(bool value);
    void setSampleRate(double value);
    void setChannelNum(OEUInt32 value);
    void setFramesPerBuffer(OEUInt32 value);
    void setBufferNum(OEUInt32 value);
    
    bool open();
    void close();
    
    void lock();
    void unlock();
    
    void runEmulations();
    
    void runAudio(const float *input,
                  float *output,
                  OEUInt32 frameCount);
    void runTimer();
    
    void openPlayer(string path);
    void closePlayer();
    void setPlayVolume(float value);
    void setPlayThrough(bool value);
    void setPlayPosition(float value);
    float getPlayPosition();
    float getPlayTime();
    bool isPlaying();
    void play();
    void pause();
    
    void openRecorder(string path);
    void closeRecorder();
    float getRecordingTime();
    OEUInt64 getRecordingSize();
    bool isRecording();
    void record();
    void stop();
    
private:
    bool fullDuplex;
    double sampleRate;
    OEUInt32 channelNum;
    OEUInt32 framesPerBuffer;
    OEUInt32 bufferNum;
    
    volatile OEUInt32 bufferAudioIndex;
    volatile OEUInt32 bufferEmulationIndex;
    vector<float> bufferInput;
    vector<float> bufferOutput;
    
    bool emulationsThreadShouldRun;
    pthread_t emulationsThread;
    pthread_mutex_t emulationsMutex;
    pthread_cond_t emulationsCond;
    
    bool audioOpen;
    PaStream *audioStream;
    bool timerThreadShouldRun;
    pthread_t timerThread;
    
    float playVolume;
    bool playThrough;
    bool playing;
    SNDFILE *playSNDFILE;
    OEUInt32 playChannelNum;
    OEUInt64 playFrameIndex;
    OEUInt64 playFrameNum;
    double playSRCRatio;
    SRC_STATE *playSRC;
    bool playSRCEndOfInput;
    vector<float> playInput;
    OEUInt32 playInputFrameIndex;
    OEUInt32 playInputFrameNum;
    
    bool recording;
    SNDFILE *recordingSNDFILE;
    OEUInt64 recordingFrameNum;
    
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
    void enableAudio(bool value);
    
    bool openEmulations();
    void closeEmulations();
    
    void playAudio(float *inputBuffer,
                   float *outputBuffer,
                   OEUInt32 frameNum,
                   OEUInt32 channelNum);
    
    void recordAudio(float *outputBuffer,
                     OEUInt32 frameNum,
                     OEUInt32 channelNum);
};

#endif
