
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
                                     (int) frameCount);
    
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

void PAAudio::setChannelNum(int value)
{
    closePlayer();
    closeRecorder();
    
    bool state = disableAudio();
    
    channelNum = value;
    
    enableAudio(state);
}

void PAAudio::setFramesPerBuffer(int value)
{
    bool state = disableAudio();
    
    framesPerBuffer = value;
    
    enableAudio(state);
}

void PAAudio::setBufferNum(int value)
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
    int bufferSize = bufferNum * framesPerBuffer * channelNum;
    bufferInput.resize(bufferSize);
    bufferOutput.resize(bufferSize);
    
    bufferAudioIndex = 0;
    bufferEmulationIndex = bufferNum;
}

bool PAAudio::isAudioBufferEmpty()
{
    int stateNum = 2 * bufferNum;
    int delta = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
    return delta <= 0;
}

float *PAAudio::getAudioInputBuffer()
{
    int index = bufferAudioIndex % bufferNum;
    int samplesPerBuffer = framesPerBuffer * channelNum;
    return &bufferInput[index * samplesPerBuffer];
}

float *PAAudio::getAudioOutputBuffer()
{
    int index = bufferAudioIndex % bufferNum;
    int samplesPerBuffer = framesPerBuffer * channelNum;
    return &bufferOutput[index * samplesPerBuffer];
}

void PAAudio::advanceAudioBuffer()
{
    int stateNum = 2 * bufferNum;
    bufferAudioIndex = (bufferAudioIndex + 1) % stateNum;
}

bool PAAudio::isEmulationsBufferEmpty()
{
    int stateNum = 2 * bufferNum;
    int delta = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
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
    while (emulationsThreadShouldRun)
    {
        lock();
        
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
        AudioBuffer buffer =
        {
            sampleRate,
            channelNum,
            framesPerBuffer,
            inputBuffer,
            outputBuffer,
        };
        
        notify(this, AUDIO_FRAME_WILL_RENDER, &buffer);
        notify(this, AUDIO_FRAME_IS_RENDERING, &buffer);
        notify(this, AUDIO_FRAME_DID_RENDER, &buffer);
        
        // Audio recording
        recordAudio(outputBuffer, framesPerBuffer, channelNum);
        
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
                               &inputParameters,
                               &outputParameters,
                               sampleRate,
                               framesPerBuffer,
                               paClipOff,
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
                                   paClipOff,
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

void PAAudio::enableAudio(bool state)
{
    initBuffer();
    
    unlock();
    
    if (state)
        openAudio();
}

void PAAudio::runAudio(const float *input,
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

void PAAudio::runTimer()
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
    
    SF_INFO sfInfo = 
    {
        0,
        0,
        0,
        0,
        0,
        0,
    };
    
    lock();
    
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
            logMessage("could not init sample rate converter, error " + getString(error));
            sf_close(playSNDFILE);
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

void PAAudio::setPlayPosition(float time)
{
    if (!playSNDFILE)
        return;
    
    lock();
    
    playFrameIndex = time * sampleRate;
    sf_seek(playSNDFILE, playFrameIndex / playSRCRatio, SEEK_SET);
    
    if (!playing)
    {
        playSRCBufferFrameBegin = 0;
        playSRCBufferFrameEnd = 0;
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

void PAAudio::playAudio(float *outputBuffer,
                        float *inputBuffer, int frameNum, int channelNum)
{
    if (!playing)
        return;
    
    vector<float> srcBuffer;
    srcBuffer.resize(frameNum * playChannelNum);
    
    int playSRCBufferFrameNum = (int) playSRCBuffer.size() / playChannelNum;
    
    do
    {
        if (playSRCBufferFrameBegin >= playSRCBufferFrameEnd)
        {
            int n = (int) sf_readf_float(playSNDFILE,
                                         &playSRCBuffer.front(),
                                         playSRCBufferFrameNum);
            
            playSRCBufferFrameBegin = 0;
            playSRCBufferFrameEnd = n;
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

long long PAAudio::getRecordingSize()
{
    return (long long) recordingFrameNum * channelNum * sizeof(short);
}

void PAAudio::recordAudio(float *outputBuffer, int frameNum, int channelNum)
{
    if (!recording)
        return;
    
    int n = (int) sf_writef_float(recordingSNDFILE, outputBuffer, frameNum);
    recordingFrameNum += n;
    
    if (frameNum != n)
    {
        sf_close(recordingSNDFILE);
        recording = false;
        recordingSNDFILE = NULL;
        recordingFrameNum = 0;
    }
}
