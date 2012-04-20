
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

#include "DIFDIDiskStorage.h"
#include "DIV2DDiskStorage.h"

typedef enum
{
    DI_APPLE525_DOS,
    DI_APPLE525_PRODOS,
    DI_APPLE525_CPM,
} DIApple525SectorOrder;

class DIApple525DiskStorage
{
public:
    DIApple525DiskStorage();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool isWriteEnabled();
    string getFormatLabel();
    
    DITrack *getTrack(DIInt head, DIInt track);
    
private:
    DIFileBackingStore fileBackingStore;
    DIRAMBackingStore ramBackingStore;
    DI2IMGBackingStore twoIMGBackingStore;
    DIDC42BackingStore dc42BackingStore;
    
    DIFDIDiskStorage fdiDiskStorage;
    DIV2DDiskStorage v2dDiskStorage;
    
    vector<DITrack> trackData;
    
    bool writeEnabled;
    
    bool open(DIBackingStore *backingStore, string pathExtension);
    bool encodeGCRDisk(DIBackingStore *backingStore,
                       DIApple525SectorOrder sectorOrder, DIInt volume);
    bool encodeNIBDisk(DIBackingStore *backingStore);
    bool encodeFDIDisk();
    bool encodeV2DDisk();
    
    void encodeGCR53Track(DIInt qtrack, DIData& data);
    void encodeGCR62Track(DIInt qtrack, DIData& data, DIApple525SectorOrder sectorOrder);
    void encodeNIBTrack(DIInt qtrack, DIData& data);
    void encodeFDIBitstream(DIInt qtrack, DIData& data);
    void encodeFDIPulses(DIInt qtrack, DIData& data);
};
