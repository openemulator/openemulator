
/**
 * libdiskimage
 * Disk Image DDL
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses Disk Description Language storage
 */

#include <string.h>

#include <libxml/parser.h>

#include "DIDDLDiskStorage.h"

DIDDLDiskStorage::DIDDLDiskStorage()
{
    writing = false;
    
    close();
}

DIDDLDiskStorage::~DIDDLDiskStorage()
{
    close();
}

bool DIDDLDiskStorage::open(DIBackingStore *backingStore)
{
    close();
    
    writing = false;
    
/*    int xmlSAXUserParseFile(xmlSAXHandlerPtr sax,
                            void * user_data,
                            const char *filename);
    
    int xmlSAXUserParseMemory(xmlSAXHandlerPtr sax,
                              this,
                              char *buffer,
                              int size);*/
    
    this->backingStore = backingStore;
    
    return true;
}

bool DIDDLDiskStorage::create(DIBackingStore *backingStore,
                              DIDiskType diskType, DIInt tracksPerInch)
{
    close();
    
    writing = true;
    
    this->diskType = diskType;
    this->tracksPerInch = tracksPerInch;
    
    this->backingStore = backingStore;
    
    return true;
}

void DIDDLDiskStorage::close()
{
    if (writing)
    {
        string medium;
        
        switch (diskType)
        {
            case DI_8_INCH:
                medium = "8 inch disk";
                
                break;
                
            case DI_525_INCH:
                medium = "5.25 inch disk";
                
                break;
                
            case DI_35_INCH:
                medium = "3.5 inch disk";
                
                break;
                
            case DI_3_INCH:
                medium = "3 inch disk";
                
                break;
        }
        
        string s;
        
        s = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        s += "<!DOCTYPE DDL PUBLIC \"-//OPENEMULATOR//DTD DDL 1.0//EN\" \"http://www.openemulator.org/DTD/ddl-1.0.dtd\">\n";
        s += "<ddl version=\"1.0\" generator=\"libdiskimage " DI_VERSION "\" description=\"\" medium=\"" + medium + "\" tracksPerInch=\"" + getString(tracksPerInch) + "\">\n";
        s += "\t<cylinder index=\"0\">\n";
        
        writeString(s);
        
        for (DIInt h = 0; h < trackData.size(); h++)
        {
            s = "\t\t<head index=\"" + getString(h) + "\">\n";
            
            writeString(s);
            
            for (DIInt t = 0; t < trackData[h].size(); t++)
            {
                s = "\t\t\t<track index=\"" + getString(t) + "\">\n";
                
                writeString(s);
                
                
                
                s = "\t\t\t</track>\n";
                
                writeString(s);
            }
            
            s = "\t\t</head>\n";
            
            writeString(s);
        }
        
        s = "\t</cylinder>\n";
        s += "</ddl>\n";
        
        writeString(s);
    }
    
    writing = false;
    
    diskType = DI_525_INCH;
    tracksPerInch = 0;
    
    trackData.clear();
    
    backingStore = NULL;
}

bool DIDDLDiskStorage::isWriteEnabled()
{
    return backingStore->isWriteEnabled();
}

DIDiskType DIDDLDiskStorage::getDiskType()
{
    return diskType;
}

DIInt DIDDLDiskStorage::getTracksPerInch()
{
    return tracksPerInch;
}

string DIDDLDiskStorage::getFormatLabel()
{
    return "DDL Disk Image";
}

bool DIDDLDiskStorage::readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    return false;
}

bool DIDDLDiskStorage::writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    if (track.format == DI_BLANK)
        return true;
    
    if (headIndex >= trackData.size())
        trackData.resize(headIndex + 1);
    
    if (trackIndex >= trackData[headIndex].size())
        trackData[headIndex].resize(trackIndex + 1);
    
    trackData[headIndex][trackIndex] = track.data;
    
    return true;
}

bool DIDDLDiskStorage::writeString(string& s)
{
    return backingStore->write(backingStore->getSize(),
                               (const DIChar *) s.c_str(), (DIInt) s.size());
}
