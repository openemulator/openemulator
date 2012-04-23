
/**
 * libdiskimage
 * Disk Image FDI
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses FDI storage
 */

#include "DIBackingStore.h"
#include "DIDiskStorage.h"

class DIFDIDiskStorage : public DIDiskStorage
{
public:
    DIFDIDiskStorage();
    
    bool openForReading(DIBackingStore *backingStore);
    bool openForWriting(DIBackingStore *backingStore, bool writeEnabled,
                        DIDiskType diskType, DIInt headNum, float rotationSpeed,
                        DIInt tracksPerInch);
    bool close();
    
    bool isWriteEnabled();
    DIDiskType getDiskType();
    DIInt getHeadNum();
    float getRotationSpeed();
    DIInt getTracksPerInch();
    string getFormatLabel();
    
    bool readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    bool writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    
private:
    DIBackingStore *backingStore;
    
    bool writeEnabled;
    DIDiskType diskType;
    DIInt headNum;
    float rotationSpeed;
    DIInt tracksPerInch;
    
    vector<DIInt> trackFormat;
    vector<DIInt> trackBitrate;
    vector<DIInt> trackOffset;
    vector<DIInt> trackSize;
    
    vector<DIData> trackData;
    
    DIInt getCodeFromTPI(DIInt value);
    DIInt getTPIFromCode(DIInt value);
    
    bool encodeBitstreamTrack(DIData& data, DIInt bitrate, DITrack& track);
    bool decodeBitstreamTrack(DITrack& track, DIData& data, DIInt& bitrate);
    bool encodePulsesTrack(DIData& data, DIInt bitrate, DITrack& track);
    bool decodePulsesTrack(DITrack& track, DIData& data, DIInt& bitrate);
};
