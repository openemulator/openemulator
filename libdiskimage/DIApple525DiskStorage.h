
/**
 * libdiskimage
 * Apple 5.25" Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple 5.25" disk image
 */

#include "DICommon.h"

#include "DIFileBackingStore.h"
#include "DIRAMBackingStore.h"
#include "DI2IMGBackingStore.h"
#include "DIDC42BackingStore.h"

#include "DIDiskStorage.h"
#include "DILogicalDiskStorage.h"
#include "DIFDIDiskStorage.h"
#include "DIV2DDiskStorage.h"

typedef struct 
{
    DITrack *track;
    DIInt index;
} DIApple525Track;

class DIApple525DiskStorage
{
public:
    DIApple525DiskStorage();
    
    bool open(string path);
    bool open(DIData& data);
    bool close();
    
    bool isWriteEnabled();
    string getFormatLabel();
    
    bool readTrack(DIInt trackIndex, DIData& data);
    bool writeTrack(DIInt trackIndex, DIData& data);
    
private:
    DIChar gcr53DecodeMap[0x100];
    DIChar gcr62DecodeMap[0x100];
    
    DIFileBackingStore fileBackingStore;
    DIRAMBackingStore ramBackingStore;
    DI2IMGBackingStore twoIMGBackingStore;
    DIDC42BackingStore dc42BackingStore;
    
    DIDiskStorage dummyDiskStorage;
    DILogicalDiskStorage logicalDiskStorage;
    DIFDIDiskStorage fdiDiskStorage;
    DIV2DDiskStorage v2dDiskStorage;
    
    DIDiskStorage *diskStorage;
    
    vector<DIData> trackData;
    bool trackDataModified;
    
    DIChar *streamData;
    DIInt streamSize;
    DIInt streamOffset;
    DIChar gcrVolume;
    DIChar gcrChecksum;  
    bool gcrError;
    
    bool open(DIBackingStore *backingStore, string pathExtension);
    
    bool checkLogicalDisk(DIBackingStore *backingStore,
                          DITrackFormat& trackFormat, DIInt& trackSize);
    
    DIInt *getSectorOrder(DITrackFormat trackFormat);
    bool getLogicalDisk(vector<DIData>& disk);
    
    bool encodeGCR53Track(DIInt trackIndex, DITrack& track);
    bool encodeGCR62Track(DIInt trackIndex, DITrack& track);
    bool encodeNIBTrack(DIInt trackIndex, DITrack& track);
    bool decodeGCR53Track(DITrack& track, DIInt trackIndex);
    bool decodeGCR62Track(DITrack& track, DIInt trackIndex, DITrackFormat trackFormat);
    
    void writeGCR53AddressField(DIInt trackIndex, DIInt sectorIndex);
    void writeGCR62AddressField(DIInt trackIndex, DIInt sectorIndex);
    void writeGCR53DataField(DIChar *data);
    void writeGCR62DataField(DIChar *data);
    bool readGCR53AddressField(DIInt trackIndex, DIInt sectorIndex);
    bool readGCR62AddressField(DIInt trackIndex, DIInt sectorIndex);
    bool readGCR53DataField(DIChar *data);
    bool readGCR62DataField(DIChar *data);
    
    void writeSync(DIInt num, DIInt q3Clocks, DIInt lastQ3Clocks);
    
    void writeFMValue(DIChar value);
    DIChar readFMValue();
    
    void resetGCR();
    bool isGCRError();
    
    void writeGCR53Value(DIChar value);
    void writeGCR62Value(DIChar value);
    void writeGCR53Checksum();
    void writeGCR62Checksum();
    
    DIChar readGCR53Value();
    DIChar readGCR62Value();
    bool validateGCR53Checksum();
    bool validateGCR62Checksum();
    
    void setStreamData(DIData& data);
    DIInt getStreamOffset();
    
    void writeNibble(DIChar value);
    void writeNibble(DIChar value, DISInt q3Clocks);
    DIChar readNibble();
};
