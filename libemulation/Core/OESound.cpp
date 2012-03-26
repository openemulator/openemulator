
/**
 * libemulation
 * OESound
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a sound type
 */

#include "OESound.h"

#include "sndfile.h"

typedef struct
{
    OEData *data;
    sf_count_t dataOffset;
} OESoundMemoryFile;

sf_count_t oeSoundGetFileLen(void *user_data)
{
    OESoundMemoryFile *memoryFile = (OESoundMemoryFile *)user_data;
    
    return memoryFile->data->size();
}

sf_count_t oeSoundSeek(sf_count_t offset, int whence, void *user_data)
{
    OESoundMemoryFile *memoryFile = (OESoundMemoryFile *)user_data;
    
    switch (whence)
    {
        case SEEK_SET:
            memoryFile->dataOffset = offset;
            break;
            
        case SEEK_CUR:
            memoryFile->dataOffset += offset;
            break;
            
        case SEEK_END:
            memoryFile->dataOffset = memoryFile->data->size() - offset;
            break;
    }
    
    if (memoryFile->dataOffset < 0)
        memoryFile->dataOffset = 0;
    else if (memoryFile->dataOffset >= memoryFile->data->size())
        memoryFile->dataOffset = memoryFile->data->size();
    
    return memoryFile->dataOffset;
}

sf_count_t oeSoundRead(void *ptr, sf_count_t count, void *user_data)
{
    OESoundMemoryFile *memoryFile = (OESoundMemoryFile *)user_data;
    
    if ((memoryFile->dataOffset + count) >= memoryFile->data->size())
        count = memoryFile->data->size() - memoryFile->dataOffset;
    
    memcpy(ptr, (const void *) (memoryFile->data->front() + count), (size_t) count * sizeof(float));
    
    return count;
}

sf_count_t oeSoundWrite(const void *ptr, sf_count_t count, void *user_data)
{
    return 0;
}

sf_count_t oeSoundTell(void *user_data)
{
    OESoundMemoryFile *memoryFile = (OESoundMemoryFile *)user_data;
    
    return memoryFile->dataOffset;
}

OESound::OESound()
{
    init();
}

OESound::OESound(string path)
{
    init();
    
    load(path);
}

OESound::OESound(OEData& data)
{
    init();
    
    load(data);
}

void OESound::init()
{
    sampleRate = 48000;
    channelNum = 1;
}

float OESound::getSampleRate()
{
    return sampleRate;
}

int OESound::getChannelNum()
{
    return channelNum;
}

int OESound::getFrameNum()
{
    return samples.size() / channelNum;
}

float *OESound::getSamples()
{
    return &samples.front();
}

bool OESound::load(string path)
{
    bool success = false;
    SNDFILE *sndFile;
    SF_INFO sfInfo;
    
    sndFile = sf_open(path.c_str(), SFM_READ, &sfInfo);
    if (sndFile)
    {
        sampleRate = sfInfo.samplerate;
        channelNum = sfInfo.channels;
        size_t frameNum = (size_t) sfInfo.frames;
        
        samples.resize(channelNum * frameNum);
        
        if (sf_read_float(sndFile, &samples.front(), frameNum) == frameNum)
            success = true;
        
        sf_close(sndFile);
    }
    
    return success;
}

bool OESound::load(OEData& data)
{
    bool success = false;
    
    OESoundMemoryFile memoryFile;
    memoryFile.data = &data;
    memoryFile.dataOffset = 0;
    
    SF_VIRTUAL_IO sfVirtual;
    sfVirtual.get_filelen = oeSoundGetFileLen;
    sfVirtual.seek = oeSoundSeek;
    sfVirtual.read = oeSoundRead;
    sfVirtual.write = oeSoundWrite;
    sfVirtual.tell = oeSoundTell;
    
    SNDFILE *sndFile;
    
    SF_INFO sfInfo;
    
    sndFile = sf_open_virtual(&sfVirtual, SFM_READ, &sfInfo, &memoryFile);
    if (sndFile)
    {
        sampleRate = sfInfo.samplerate;
        channelNum = sfInfo.channels;
        size_t frameNum = (size_t) sfInfo.frames;
        
        samples.resize(channelNum * frameNum);
        
        if (sf_read_float(sndFile, &samples.front(), frameNum) == frameNum)
            success = true;
        
        sf_close(sndFile);
    }
    
    return success;
}
