
/**
 * libemulation-hal
 * PortAudio audio
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a PortAudio audio component
 */

#include <iostream>

#include "PAAudio.h"

#include "AudioInterface.h"

#define DEFAULT_SAMPLERATE          48000
#define DEFAULT_CHANNELNUM          2
#define DEFAULT_FRAMESPERBUFFER     512
#define DEFAULT_BUFFERNUM           3

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
                                     (OEInt) frameCount);
    
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

// Configuration

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
    
    playerVolume = 1;
    playerPlayThrough = false;
    playerSNDFILE = NULL;
    playerPlaying = false;
    recorderSNDFILE = NULL;
    recorderRecording = false;
}

PAAudio::~PAAudio()
{
    pthread_cond_destroy(&emulationsCond);
    
    closePlayer();
    closeRecorder();
}

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

void PAAudio::setChannelNum(OEInt value)
{
    closePlayer();
    closeRecorder();
    
    bool state = disableAudio();
    
    channelNum = value;
    
    enableAudio(state);
}

void PAAudio::setFramesPerBuffer(OEInt value)
{
    bool state = disableAudio();
    
    framesPerBuffer = value;
    
    enableAudio(state);
}

void PAAudio::setBufferNum(OEInt value)
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

// Audio buffering

void PAAudio::initBuffer()
{
    OEInt bufferSize = bufferNum * framesPerBuffer * channelNum;
    bufferInput.resize(bufferSize);
    bufferOutput.resize(bufferSize);
    
    bufferAudioIndex = 0;
    bufferEmulationIndex = bufferNum;
}

bool PAAudio::isAudioBufferEmpty()
{
    OEInt stateNum = 2 * bufferNum;
    
    OEInt delta = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
    
    return delta <= 0;
}

float *PAAudio::getAudioInputBuffer()
{
    OEInt index = bufferAudioIndex % bufferNum;
    
    OEInt samplesPerBuffer = framesPerBuffer * channelNum;
    
    return &bufferInput[index * samplesPerBuffer];
}

float *PAAudio::getAudioOutputBuffer()
{
    OEInt index = bufferAudioIndex % bufferNum;
    
    OEInt samplesPerBuffer = framesPerBuffer * channelNum;
    
    return &bufferOutput[index * samplesPerBuffer];
}

void PAAudio::advanceAudioBuffer()
{
    OEInt stateNum = 2 * bufferNum;
    
    bufferAudioIndex = (bufferAudioIndex + 1) % stateNum;
}

bool PAAudio::isEmulationsBufferEmpty()
{
    OEInt stateNum = 2 * bufferNum;
    
    OEInt delta = (stateNum + bufferEmulationIndex - bufferAudioIndex) % stateNum;
    
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

// Emulations

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
    OEInt localBufferSize = 0;
    float *localInputBuffer = NULL;
    float *localOutputBuffer = NULL;
    
    while (emulationsThreadShouldRun)
    {
        lock();
        
        if (isEmulationsBufferEmpty())
            pthread_cond_wait(&emulationsCond, &emulationsMutex);
        
        OEInt samplesPerBuffer = framesPerBuffer * channelNum;
        OEInt bytesPerBuffer = samplesPerBuffer * sizeof(float);
        
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
        
        postNotification(this, AUDIO_BUFFER_WILL_RENDER, &audioBuffer);
        postNotification(this, AUDIO_BUFFER_IS_RENDERING, &audioBuffer);
        postNotification(this, AUDIO_BUFFER_DID_RENDER, &audioBuffer);
        
        // Audio recording
        recordAudio(localOutputBuffer, framesPerBuffer, channelNum);
        
        // Copy local output buffer to circular output buffer
        memcpy(getEmulationsOutputBuffer(), localOutputBuffer, bytesPerBuffer);
        
        unlock();
        
        advanceEmulationsBuffer();
    }
}

// Audio

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
                       OEInt frameCount)
{
    OEInt samplesPerBuffer = frameCount * channelNum;
    OEInt bytesPerBuffer = samplesPerBuffer * sizeof(float);
    
    // Render noise when no data is available
    // Note: this should be removed when the framework is stable
    if (isAudioBufferEmpty() || (frameCount != framesPerBuffer))
    {
        float k = 0.1 / RAND_MAX;
        
        for (OEInt i = 0; i < samplesPerBuffer; i++)
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
        OEInt samplesPerBuffer = framesPerBuffer * channelNum;
        OEInt bytesPerBuffer = samplesPerBuffer * sizeof(float);
        
        usleep(1E6 * framesPerBuffer / sampleRate);
        
        if (isAudioBufferEmpty())
            continue;
        
        memset(getAudioInputBuffer(), 0, bytesPerBuffer);
        
        advanceAudioBuffer();
        
        pthread_cond_signal(&emulationsCond);
    }
}

// Player

void PAAudio::openPlayer(string path)
{
    closePlayer();
    
    lock();
    
    SF_INFO sfInfo;
    
    playerSNDFILE = sf_open(path.c_str(), SFM_READ, &sfInfo);
    if (playerSNDFILE)
    {
        playerChannelNum = sfInfo.channels;
        playerSRCRatio = (double) sampleRate / sfInfo.samplerate;
        playerFrameIndex = 0;
        playerFrameNum = sfInfo.frames * playerSRCRatio;
        
        int error;
        
        playerSRC = src_new(SRC_SINC_FASTEST, playerChannelNum, &error);
        if (playerSRC)
        {
            playerInput.resize(framesPerBuffer * playerChannelNum);
            playerInputFrameNum = 0;
        }
        else
        {
            logMessage("could not init sample rate converter, error " + getString(error));
            
            sf_close(playerSNDFILE);
            
            playerSNDFILE = NULL;
        }
    }
    else
        logMessage("could not open " + path);
    
    unlock();
}

void PAAudio::closePlayer()
{
    if (!playerSNDFILE)
        return;
    
    lock();
    
    sf_close(playerSNDFILE);
    
    playerPlaying = false;
    playerSNDFILE = NULL;
    playerFrameIndex = 0;
    playerFrameNum = 0;
    
    unlock();
}

void PAAudio::setPlayerVolume(float value)
{
    playerVolume = value;
}

void PAAudio::setPlayerPlayThrough(bool value)
{
    playerPlayThrough = value;
}

void PAAudio::setPlayerPosition(float value)
{
    if (!playerSNDFILE)
        return;
    
    lock();
    
    playerFrameIndex = value * sampleRate;
    sf_seek(playerSNDFILE, playerFrameIndex / playerSRCRatio, SEEK_SET);
    
    src_reset(playerSRC);
    
    playerInputFrameNum = 0;
    playerSRCEndOfInput = false;
    
    unlock();
}

void PAAudio::startPlayer()
{
    if (getPlayerPosition() >= (getPlayerTime() - 0.1))
        setPlayerPosition(0);
    
    if (!playerSNDFILE)
        return;
    
    playerPlaying = true;
}

void PAAudio::pausePlayer()
{
    if (!playerSNDFILE)
        return;
    
    playerPlaying = false;
}

float PAAudio::getPlayerPosition()
{
    return (float) playerFrameIndex / sampleRate;
}

float PAAudio::getPlayerTime()
{
    return (float) playerFrameNum / sampleRate;
}

bool PAAudio::isPlayerPlaying()
{
    return playerPlaying;
}

void PAAudio::playAudio(float *inputBuffer,
                        float *outputBuffer,
                        OEInt frameNum,
                        OEInt channelNum)
{
    if (!playerPlaying)
        return;
    
    OEInt srcOutputFrameIndex = 0;
    OEInt srcOutputFrameNum = frameNum;
    
    vector<float> srcOutput;
    srcOutput.resize(srcOutputFrameNum * playerChannelNum);
    
    do
    {
        if (!playerInputFrameNum)
        {
            OEInt inputFrameNum = (OEInt) playerInput.size() / playerChannelNum;
            
            playerInputFrameIndex = 0;
            playerInputFrameNum = (OEInt) sf_readf_float(playerSNDFILE,
                                                            &playerInput.front(),
                                                            inputFrameNum);
            
            playerSRCEndOfInput = (playerInputFrameNum != inputFrameNum);
        }
        
        SRC_DATA srcData =
        {
            &playerInput[playerInputFrameIndex * playerChannelNum],
            &srcOutput[srcOutputFrameIndex * playerChannelNum],
            playerInputFrameNum,
            srcOutputFrameNum,
            0, 0,
            playerSRCEndOfInput,
            playerSRCRatio,
        };
        
        src_process(playerSRC, &srcData);
        
        if (playerSRCEndOfInput && !srcData.output_frames_gen)
        {
            playerPlaying = false;
            
            break;
        }
        
        playerInputFrameIndex += srcData.input_frames_used;
        playerInputFrameNum -= srcData.input_frames_used;
        
        srcOutputFrameIndex += srcData.output_frames_gen;
        srcOutputFrameNum -= srcData.output_frames_gen;
        
        playerFrameIndex += srcData.output_frames_gen;
    } while (srcOutputFrameNum > 0);
    
    float linearVolume = getLevelFromVolume(playerVolume);
    OEInt sampleNum = (frameNum - srcOutputFrameNum) * channelNum;
    
    for (OEInt ch = 0; ch < channelNum; ch++)
    {
        float *x = &srcOutput.front() + (ch % playerChannelNum);
        float *yi = inputBuffer + ch;
        float *yo = outputBuffer + ch;
        
        for (OEInt i = 0; i < sampleNum; i += channelNum)
        {
            float value = *x * linearVolume;
            
            yi[i] += value;
            if (playerPlayThrough)
                yo[i] += value;
            
            x += playerChannelNum;
        }
    }
}

// Recorder

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
    
    if (!(recorderSNDFILE = sf_open(path.c_str(), SFM_WRITE, &sfInfo)))
        logMessage("could not open temporary recorder file " + path);
    
    recorderFrameNum = 0;
    
    unlock();
}

void PAAudio::closeRecorder()
{
    if (!recorderSNDFILE)
        return;
    
    lock();
    
    sf_close(recorderSNDFILE);
    
    recorderSNDFILE = NULL;
    recorderRecording = false;
    
    unlock();
}

float PAAudio::getRecorderTime()
{
    return (float) recorderFrameNum / sampleRate;
}

OELong PAAudio::getRecorderSize()
{
    return (OELong) recorderFrameNum * channelNum * sizeof(short);
}

bool PAAudio::isRecorderRecording()
{
    return recorderRecording;
}

void PAAudio::startRecorder()
{
    if (recorderSNDFILE)
        recorderRecording = true;
}

void PAAudio::stopRecorder()
{
    if (recorderSNDFILE)
        recorderRecording = false;
}

void PAAudio::recordAudio(float *outputBuffer,
                          OEInt frameNum,
                          OEInt channelNum)
{
    if (!recorderRecording)
        return;
    
    OEInt n = (OEInt) sf_writef_float(recorderSNDFILE, outputBuffer, frameNum);
    recorderFrameNum += n;
    
    if (frameNum != n)
    {
        sf_close(recorderSNDFILE);
        
        recorderRecording = false;
        recorderSNDFILE = NULL;
        recorderFrameNum = 0;
    }
}
