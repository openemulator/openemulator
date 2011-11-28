
/**
 * libemulation-hal
 * PortAudio audio
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a PortAudio audio component
 */

#include <math.h>
#include <iostream>

#include "PAAudio.h"

#include "AudioInterface.h"

#define DEFAULT_SAMPLERATE			48000
#define DEFAULT_CHANNELNUM			2
#define DEFAULT_FRAMESPERBUFFER		512
#define DEFAULT_BUFFERNUM			3

#define PLAY_FRAMESPERBUFFER        1024

using namespace std;

// Callbacks

static int PAAudioRunAudio(const void *input,
                           void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData)
{
    ((PAAudio *) userData)->runAudio((const float *)input,
                                     (float *)output,
                                     (OEUInt32) frameCount);
    
    return paContinue;
}

void *PAAudioRunTimer(void *arg)
{
    ((PAAudio *) arg)->runTimer();
    
    return NULL;
}

void *PAAudioRunEmulations(void *arg)
{
    ((PAAudio *) arg)->runEmulations();
    
    return NULL;
}

// Constructor/destructor

PAAudio::PAAudio()
{
    fullDuplex = false;
    sampleRate = DEFAULT_SAMPLERATE;
    channelNum = DEFAULT_CHANNELNUM;
    framesPerBuffer = DEFAULT_FRAMESPERBUFFER;
    bufferNum = DEFAULT_BUFFERNUM;
    
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

PAAudio::~PAAudio()
{
    pthread_cond_destroy(&emulationsCond);
}

// Configuration

void PAAudio::setFullDuplex(bool value)
{
    bool state = disableAudio();
    
    fullDuplex = value;
    
    enableAudio(state);
}

void PAAudio::setSampleRate(double value)
{
    closePlayer();
    closeRecorder();
    
    bool state = disableAudio();
    
    sampleRate = value;
    
    enableAudio(state);
}

void PAAudio::setChannelNum(OEUInt32 value)
{
    closePlayer();
    closeRecorder();
    
    bool state = disableAudio();
    
    channelNum = value;
    
    enableAudio(state);
}

void PAAudio::setFramesPerBuffer(OEUInt32 value)
{
    bool state = disableAudio();
    
    framesPerBuffer = value;
    
    enableAudio(state);
}

void PAAudio::setBufferNum(OEUInt32 value)
{
    bool state = disableAudio();
    
    bufferNum = value;
    
    enableAudio(state);
}

bool PAAudio::open()
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

void PAAudio::close()
{
    closeAudio();
    closeEmulations();
}

//
// Audio buffering
//
void PAAudio::initBuffer()
{
    OEUInt32 bufferSize = bufferNum * framesPerBuffer * channelNum;
    bufferInput.resize(bufferSize);
    bufferOutput.resize(bufferSize);
    
    bufferAudioIndex = 0;
    bufferEmulationIndex = bufferNum;
}

bool PAAudio::isAudioBufferEmpty()
{
    OEUInt32 stateNum = 2 * bufferNum;
    
    OEUInt32 delta = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
    
    return delta <= 0;
}

float *PAAudio::getAudioInputBuffer()
{
    OEUInt32 index = bufferAudioIndex % bufferNum;
    
    OEUInt32 samplesPerBuffer = framesPerBuffer * channelNum;
    
    return &bufferInput[index * samplesPerBuffer];
}

float *PAAudio::getAudioOutputBuffer()
{
    OEUInt32 index = bufferAudioIndex % bufferNum;
    
    OEUInt32 samplesPerBuffer = framesPerBuffer * channelNum;
    
    return &bufferOutput[index * samplesPerBuffer];
}

void PAAudio::advanceAudioBuffer()
{
    OEUInt32 stateNum = 2 * bufferNum;
    
    bufferAudioIndex = (bufferAudioIndex + 1) % stateNum;
}

bool PAAudio::isEmulationsBufferEmpty()
{
    OEUInt32 stateNum = 2 * bufferNum;
    
    OEUInt32 delta = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
    
    return (bufferNum - delta) <= 0;
}

float *PAAudio::getEmulationsInputBuffer()
{
    int index = bufferEmulationIndex % bufferNum;
    
    int samplesPerBuffer = framesPerBuffer * channelNum;
    
    return &bufferInput[index * samplesPerBuffer];
}

float *PAAudio::getEmulationsOutputBuffer()
{
    int index = bufferEmulationIndex % bufferNum;
    
    int samplesPerBuffer = framesPerBuffer * channelNum;
    
    return &bufferOutput[index * samplesPerBuffer];
}

void PAAudio::advanceEmulationsBuffer()
{
    int stateNum = 2 * bufferNum;
    
    bufferEmulationIndex = (bufferEmulationIndex + 1) % stateNum;
}

//
// Emulations
//
bool PAAudio::openEmulations()
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
                                       PAAudioRunEmulations,
                                       this);
                if (!error)
                    return true;
                else
                    logMessage("could not create eulations thread, error " + getString(error));
            }
            else
                logMessage("could not attr emulations thread, error " + getString(error));
        }
        else
            logMessage("could not init emulations attr, error " + getString(error));
    }
    else
        logMessage("could not init emulations mutex, error " + getString(error));
    
    return false;
}

void PAAudio::closeEmulations()
{
    if (!emulationsThreadShouldRun)
        return;
    
    emulationsThreadShouldRun = false;
    
    pthread_cond_signal(&emulationsCond);
    
    void *status;
    pthread_join(emulationsThread, &status);
    
    pthread_mutex_destroy(&emulationsMutex);
}

void PAAudio::lock()
{
    pthread_mutex_lock(&emulationsMutex);
}

void PAAudio::unlock()
{
    pthread_mutex_unlock(&emulationsMutex);
}

void PAAudio::runEmulations()
{
    OEUInt32 localBufferSize = 0;
    float *localInputBuffer = NULL;
    float *localOutputBuffer = NULL;
    
    while (emulationsThreadShouldRun)
    {
        lock();
        
        if (isEmulationsBufferEmpty())
            pthread_cond_wait(&emulationsCond, &emulationsMutex);
        
        OEUInt32 samplesPerBuffer = framesPerBuffer * channelNum;
        OEUInt32 bytesPerBuffer = samplesPerBuffer * sizeof(float);
        
        // Resize local buffers
        if (localBufferSize != bytesPerBuffer)
        {
            localBufferSize = bytesPerBuffer;
            
            free(localInputBuffer);
            free(localOutputBuffer);
            
            localInputBuffer = (float *) malloc(2 * bytesPerBuffer);
            localOutputBuffer = (float *) malloc(2 * bytesPerBuffer);
            
            memset(localInputBuffer + samplesPerBuffer, 0, bytesPerBuffer);
            memset(localOutputBuffer + samplesPerBuffer, 0, bytesPerBuffer);
        }
        
        // Copy circular input buffer to local input buffer
        memcpy(localInputBuffer, localInputBuffer + samplesPerBuffer, bytesPerBuffer);
        memcpy(localInputBuffer + samplesPerBuffer, getEmulationsInputBuffer(), bytesPerBuffer);
        
        // Shift local output buffer
        memcpy(localOutputBuffer, localOutputBuffer + samplesPerBuffer, bytesPerBuffer);
        memset(localOutputBuffer + samplesPerBuffer, 0, bytesPerBuffer);
        
        // Audio play
        playAudio(localInputBuffer + samplesPerBuffer, localOutputBuffer, framesPerBuffer, channelNum);
        
        // Output
        AudioBuffer audioBuffer =
        {
            sampleRate,
            channelNum,
            framesPerBuffer,
            localInputBuffer,
            localOutputBuffer,
        };
        
        postNotification(this, AUDIO_FRAME_WILL_RENDER, &audioBuffer);
        postNotification(this, AUDIO_FRAME_IS_RENDERING, &audioBuffer);
        postNotification(this, AUDIO_FRAME_DID_RENDER, &audioBuffer);
        
        // Audio recording
        recordAudio(localOutputBuffer, framesPerBuffer, channelNum);
        
        // Copy local output buffer to circular output buffer
        memcpy(getEmulationsOutputBuffer(), localOutputBuffer, bytesPerBuffer);
        
        unlock();
        
        advanceEmulationsBuffer();
    }
}

//
// Audio
//
bool PAAudio::openAudio()
{
    if (audioOpen)
        closeAudio();
    
    int status = Pa_Initialize();
    if (status == paNoError)
    {
        PaStreamParameters inputParameters;
        PaStreamParameters outputParameters;
        const PaDeviceInfo *deviceInfo;
        
        inputParameters.device = Pa_GetDefaultInputDevice();
        inputParameters.channelCount = channelNum;
        inputParameters.sampleFormat = paFloat32;
        deviceInfo = Pa_GetDeviceInfo(inputParameters.device);
        inputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
        inputParameters.hostApiSpecificStreamInfo = NULL;
        
        outputParameters.device = Pa_GetDefaultOutputDevice();
        outputParameters.channelCount = channelNum;
        outputParameters.sampleFormat = paFloat32;
        deviceInfo = Pa_GetDeviceInfo(outputParameters.device);
        outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        
        status = Pa_OpenStream(&audioStream,
                               fullDuplex ? &inputParameters : NULL,
                               &outputParameters,
                               sampleRate,
                               framesPerBuffer,
                               paNoFlag,
                               PAAudioRunAudio,
                               this);
        if ((status != paNoError) && fullDuplex)
        {
            logMessage("could not open audio stream, error " + getString(status));
            logMessage("attempting half-duplex");
            
            status = Pa_OpenStream(&audioStream,
                                   NULL,
                                   &outputParameters,
                                   sampleRate,
                                   framesPerBuffer,
                                   paNoFlag,
                                   PAAudioRunAudio,
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
                logMessage("could not start audio stream, error " + getString(status));
            
            Pa_CloseStream(audioStream);
        }
        else
            logMessage("could not open audio stream, error " + getString(status));
    }
    else
        logMessage("could not init portaudio, error " + getString(status));
    
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
                                   PAAudioRunTimer,
                                   this);
            if (!error)
            {
                logMessage("started timer thread");
                
                audioOpen = true;
                
                return true;
            }
            else
                logMessage("could not create timer thread, error " + getString(error));
        }
        else
            logMessage("could not attr timer thread, error " + getString(error));
    }
    else
        logMessage("could not init timer thread, error " + getString(error));
    
    return false;
}

void PAAudio::closeAudio()
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

bool PAAudio::disableAudio()
{
    bool state = audioOpen;
    
    if (state)
        closeAudio();
    
    lock();
    
    return state;
}

void PAAudio::enableAudio(bool value)
{
    initBuffer();
    
    unlock();
    
    if (value)
        openAudio();
}

void PAAudio::runAudio(const float *input,
                       float *output,
                       OEUInt32 frameCount)
{
    OEUInt32 samplesPerBuffer = frameCount * channelNum;
    OEUInt32 bytesPerBuffer = samplesPerBuffer * sizeof(float);
    
    // Render noise when no data is available
    // Note: this should go once the framework is stable
    if (isAudioBufferEmpty() || (frameCount != framesPerBuffer))
    {
        float k = 0.1 / RAND_MAX;
        
        for (OEUInt32 i = 0; i < samplesPerBuffer; i++)
            *output++ = k * rand();
        
        return;
    }
    
    // Copy input buffer
    if (input)
        memcpy(getAudioInputBuffer(), input, bytesPerBuffer);
    else
        memset(getAudioInputBuffer(), 0, bytesPerBuffer);
    
    // Copy output buffer
    memcpy(output, getAudioOutputBuffer(), bytesPerBuffer);
    
    advanceAudioBuffer();
    
    pthread_cond_signal(&emulationsCond);
    
    return;
}

void PAAudio::runTimer()
{
    while (timerThreadShouldRun)
    {
        OEUInt32 samplesPerBuffer = framesPerBuffer * channelNum;
        OEUInt32 bytesPerBuffer = samplesPerBuffer * sizeof(float);
        
        usleep(1E6 * framesPerBuffer / sampleRate);
        
        if (isAudioBufferEmpty())
            continue;
        
        memset(getAudioInputBuffer(), 0, bytesPerBuffer);
        
        advanceAudioBuffer();
        
        pthread_cond_signal(&emulationsCond);
    }
}

//
// Play
//
void PAAudio::setPlayVolume(float value)
{
    playVolume = value;
}

void PAAudio::setPlayThrough(bool value)
{
    playThrough = value;
}

void PAAudio::openPlayer(string path)
{
    closePlayer();
    
    lock();
    
    SF_INFO sfInfo = 
    {
        0,
        0,
        0,
        0,
        0,
        0,
    };
    
    playSNDFILE = sf_open(path.c_str(), SFM_READ, &sfInfo);
    if (playSNDFILE)
    {
        playChannelNum = sfInfo.channels;
        playSRCRatio = (double) sampleRate / sfInfo.samplerate;
        playFrameIndex = 0;
        playFrameNum = sfInfo.frames * playSRCRatio;
        
        int error;
        
        playSRC = src_new(SRC_SINC_FASTEST, playChannelNum, &error);
        if (playSRC)
        {
            playSRCInputFrameNum = 0;
            
            playSRCInput.resize(framesPerBuffer * playChannelNum);
        }
        else
        {
            logMessage("could not init sample rate converter, error " + getString(error));
            
            sf_close(playSNDFILE);
              
            playSNDFILE = NULL;
        }
    }
    else
        logMessage("could not open file " + path);
    
    unlock();
}

void PAAudio::closePlayer()
{
    if (!playSNDFILE)
        return;
    
    lock();
    
    sf_close(playSNDFILE);
    
    playSNDFILE = NULL;
    playing = false;
    playFrameIndex = 0;
    playFrameNum = 0;
    
    unlock();
}

void PAAudio::setPlayPosition(float value)
{
    if (!playSNDFILE)
        return;
    
    lock();
    
    playFrameIndex = value * sampleRate;
    sf_seek(playSNDFILE, playFrameIndex / playSRCRatio, SEEK_SET);
    
    if (!playing)
    {
        playSRCInputFrameNum = 0;
        playSRCEndOfInput = false;
    }
    
    unlock();
}

void PAAudio::play()
{
    if ((getPlayTime() + 0.1) >= getPlayDuration())
        setPlayPosition(0.0);
    
    if (playSNDFILE)
        playing = true;
}

void PAAudio::pause()
{
    if (playSNDFILE)
        playing = false;
}

bool PAAudio::isPlaying()
{
    return playing;
}

float PAAudio::getPlayTime()
{
    return (float) playFrameIndex / sampleRate;
}

float PAAudio::getPlayDuration()
{
    return (float) playFrameNum / sampleRate;
}

void PAAudio::playAudio(float *inputBuffer,
                        float *outputBuffer,
                        OEUInt32 frameNum,
                        OEUInt32 channelNum)
{
    if (!playing)
        return;
    
    OEUInt32 srcOutputFrameIndex = 0;
    OEUInt32 srcOutputFrameNum = frameNum;
    
    vector<float> srcOutput;
    srcOutput.resize(srcOutputFrameNum * playChannelNum);
    
    do
    {
        if (!playSRCInputFrameNum)
        {
            OEUInt32 inputFrameNum = (OEUInt32) playSRCInput.size() / playChannelNum;
            
            playSRCInputFrameIndex = 0;
            
            playSRCInputFrameNum = (OEUInt32) sf_readf_float(playSNDFILE,
                                                             &playSRCInput.front(),
                                                             inputFrameNum);
            
            playSRCEndOfInput = (playSRCInputFrameNum != inputFrameNum);
        }
        
        SRC_DATA srcData =
        {
            &playSRCInput[playSRCInputFrameIndex * playChannelNum],
            &srcOutput[srcOutputFrameIndex * playChannelNum],
            playSRCInputFrameNum,
            srcOutputFrameNum,
            0, 0,
            playSRCEndOfInput,
            playSRCRatio,
        };
        
        src_process(playSRC, &srcData);
        
        if (playSRCEndOfInput && !srcData.output_frames_gen)
        {
            playing = false;
            
            break;
        }
        
        playSRCInputFrameIndex += srcData.input_frames_used;
        playSRCInputFrameNum -= srcData.input_frames_used;
        
        srcOutputFrameIndex += srcData.output_frames_gen;
        srcOutputFrameNum -= srcData.output_frames_gen;
        
        playFrameIndex += srcData.output_frames_gen;
    } while (srcOutputFrameNum > 0);
    
    float linearVolume = pow(10.0, (playVolume - 1.0) * 100.0 / 20.0);
    OEUInt32 sampleNum = (frameNum - srcOutputFrameNum) * channelNum;
    
    for (OEUInt32 ch = 0; ch < channelNum; ch++)
    {
        float *x = &srcOutput.front() + (ch % playChannelNum);
        float *yi = inputBuffer + ch;
        float *yo = outputBuffer + ch;
        
        for (OEUInt32 i = 0; i < sampleNum; i += channelNum)
        {
            float value = *x * linearVolume;
            
            yi[i] += value;
            if (playThrough)
                yo[i] += value;
            
            x += playChannelNum;
        }
    }
}

//
// Recording
//
void PAAudio::openRecorder(string path)
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
    
    lock();
    
    if (!(recordingSNDFILE = sf_open(path.c_str(), SFM_WRITE, &sfInfo)))
        logMessage("could not open temporary file " + path);
    
    recordingFrameNum = 0;
    
    unlock();
}

void PAAudio::closeRecorder()
{
    if (!recordingSNDFILE)
        return;
    
    lock();
    
    sf_close(recordingSNDFILE);
    
    recordingSNDFILE = NULL;
    recording = false;
    
    unlock();
}

void PAAudio::record()
{
    if (recordingSNDFILE)
        recording = true;
}

void PAAudio::stop()
{
    if (recordingSNDFILE)
        recording = false;
}

bool PAAudio::isRecording()
{
    return recording;
}

float PAAudio::getRecordingTime()
{
    return (float) recordingFrameNum / sampleRate;
}

OEUInt64 PAAudio::getRecordingSize()
{
    return (OEUInt64) recordingFrameNum * channelNum * sizeof(short);
}

void PAAudio::recordAudio(float *outputBuffer,
                          OEUInt32 frameNum,
                          OEUInt32 channelNum)
{
    if (!recording)
        return;
    
    OEUInt32 n = (OEUInt32) sf_writef_float(recordingSNDFILE, outputBuffer, frameNum);
    recordingFrameNum += n;
    
    if (frameNum != n)
    {
        sf_close(recordingSNDFILE);
        
        recording = false;
        recordingSNDFILE = NULL;
        recordingFrameNum = 0;
    }
}
