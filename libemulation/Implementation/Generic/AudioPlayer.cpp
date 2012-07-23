
/**
 * libemulation
 * Audio player
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an audio player
 */

#include <string.h>

#include "AudioPlayer.h"

#include "AudioPlayerInterface.h"
#include "ControlBusInterface.h"

AudioPlayer::AudioPlayer()
{
    audio = NULL;
    controlBus = NULL;
    
    playing = false;
    loop = false;
    volume = 1;
    frameIndex = 0;
    
    sound = NULL;
    
    srcChannelNum = 0;
    srcState = NULL;
    srcEndOfInput = false;
    
    audioBuffer = NULL;
    audioBufferFrame = 0;
}

bool AudioPlayer::setValue(string name, string value)
{
    if (name == "playing")
        playing = getOEInt(value);
    else if (name == "loop")
        loop = getOEInt(value);
    else if (name == "volume")
        volume = getFloat(value);
    else
        return false;
    
    return true;
}

bool AudioPlayer::getValue(string name, string value)
{
    if (name == "playing")
        value = getString(playing);
    else if (name == "loop")
        value = getString(loop);
    else if (name == "volume")
        value = getString(volume);
    else
        return false;
    
    return true;
}

bool AudioPlayer::setRef(string name, OEComponent *ref)
{
    if (name == "audio")
    {
        if (audio)
        {
            audio->removeObserver(this, AUDIO_BUFFER_WILL_RENDER);
            audio->removeObserver(this, AUDIO_BUFFER_DID_RENDER);
        }
        audio = ref;
        if (audio)
        {
            audio->addObserver(this, AUDIO_BUFFER_WILL_RENDER);
            audio->addObserver(this, AUDIO_BUFFER_DID_RENDER);
        }
    }
    else if (name == "controlBus")
        controlBus = ref;
    else
        return false;
    
    return true;
}

bool AudioPlayer::setData(string name, OEData *data)
{
    if (name == "sound")
    {
        loadedSound = OESound(*data);
        sound = &loadedSound;
    }
    else
        return false;
    
    return true;
}

bool AudioPlayer::init()
{
    if (!audio)
    {
        logMessage("audio not connected");
        
        return false;
    }
    
    return true;
}

bool AudioPlayer::postMessage(OEComponent *sender, int message, void *data)
{
    updateAudio(false);
    
    switch (message)
    {
        case AUDIOPLAYER_PLAY:
            playing = true;
            
            break;
            
        case AUDIOPLAYER_PAUSE:
            playing = false;
            
            break;
            
        case AUDIOPLAYER_STOP:
            frameIndex = 0;
            playing = false;
            
            src_reset(srcState);
            
            break;
            
        case AUDIOPLAYER_SET_SOUND:
            sound = (OESound *)data;
            
            return true;
            
        case AUDIOPLAYER_SET_LOOP:
            loop = *((bool *)data);
            
            return true;
            
        case AUDIOPLAYER_SET_VOLUME:
            volume = *((float *)data);
            
            return true;
            
        case AUDIOPLAYER_IS_PLAYING:
            *((bool *)data) = playing;
            
            return true;
    }
    
    return false;
}

void AudioPlayer::notify(OEComponent *sender, int notification, void *data)
{
    if (notification == AUDIO_BUFFER_WILL_RENDER)
    {
        audioBuffer = (AudioBuffer *) data;
        audioBufferFrame = 0;
    }
    else
        updateAudio(true);
}

void AudioPlayer::updateAudio(bool bufferDidRender)
{
    OEInt nextAudioBufferFrame;
    
    if (bufferDidRender)
        nextAudioBufferFrame = audioBuffer->frameNum;
    else
    {
        if (!controlBus)
            return;
        
        float value;
        
        controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERFRAME, &value);
        
        nextAudioBufferFrame = value;
    }
    
    if (playing && sound)
    {
        if (srcChannelNum != sound->getChannelNum())
        {
            int error;
            
            if (srcState)
                src_delete(srcState);
            
            srcState = src_new(SRC_SINC_FASTEST, sound->getChannelNum(), &error);
            srcChannelNum = sound->getChannelNum();
        }
        
        if (srcState)
        {
            if (frameIndex > sound->getFrameNum())
                frameIndex = sound->getFrameNum();
            
            vector<float> output;
            output.resize(audioBuffer->frameNum * srcChannelNum);
            
            do
            {
                SRC_DATA srcData =
                {
                    sound->getSamples() + frameIndex * srcChannelNum,
                    &output.front() + audioBufferFrame * srcChannelNum,
                    (long) (sound->getFrameNum() - frameIndex),
                    nextAudioBufferFrame - audioBufferFrame,
                    0, 0,
                    0,
                    audioBuffer->sampleRate / sound->getSampleRate(),
                };
                
                if (srcData.src_ratio != 1.0)
                    src_process(srcState, &srcData);
                else
                {
                    srcData.input_frames_used = ((srcData.input_frames < srcData.output_frames) ?
                                                 srcData.input_frames : srcData.output_frames);
                    srcData.output_frames_gen = srcData.input_frames_used;
                    
                    memcpy(srcData.data_out, srcData.data_in,
                           srcData.input_frames_used * srcChannelNum * sizeof(float));
                }
                
                frameIndex += (OEInt) srcData.input_frames_used;
                audioBufferFrame += (OEInt) srcData.output_frames_gen;
                
                if (loop && (frameIndex == sound->getFrameNum()))
                    frameIndex = 0;
                else if (!srcData.input_frames_used &&
                         !srcData.output_frames_gen)
                {
                    playing = false;
                    
                    break;
                }
            } while (audioBufferFrame != nextAudioBufferFrame);
            
            OEInt sampleNum = audioBuffer->frameNum * audioBuffer->channelNum;
            
            for (OEInt ch = 0; ch < audioBuffer->channelNum; ch++)
            {
                float *x = &output.front() + (ch % srcChannelNum);
                float *y = audioBuffer->output + ch;
                
                for (OEInt i = 0; i < sampleNum; i += audioBuffer->channelNum)
                {
                    y[i] += *x * volume;
                    x += srcChannelNum;
                }
            }
        }
    }
    
    audioBufferFrame = nextAudioBufferFrame;
}
