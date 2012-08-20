
/*
 * nib2ddl
 * (C) 2012 by Marc S. Ressl
 * Released under the GPL.
 *
 * Simple NIB to DDL format 1.0 converter
 */

#include <stdio.h>

#define TRUE            1
#define FALSE           0

#define TRACK_NUM       35
#define TRACK_SIZE      6656

#define DATA_COLNUM     16
#define NODATA_COLNUM   8

int convertNIB2DDL(char *source, char *target, int insertSync)
{
    FILE *fpi;
    FILE *fpo;
    
    int success = 0;
    
    fpi = fopen(source, "rb");
    if (fpi)
    {
        fpo = fopen(target, "wt");
        if (fpo)
        {
            int trackIndex;
            
            fprintf(fpo, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            fprintf(fpo, "<!DOCTYPE ddl PUBLIC \"-//DDL//DTD DDL 1.0//EN" "http://www.openemulator.org/DTD/ddl-1.0.dtd\">\n");
            fprintf(fpo, "<ddl version=\"1.0\" creator=\"nib2ddl 1.0\" label=\"\" description=\"\" medium=\"5.25 inch disk\" tracksPerInch=\"48\" writeEnabled=\"1\">\n");
            
            for (trackIndex = 0; trackIndex < TRACK_NUM; trackIndex++)
            {
                unsigned char data[TRACK_SIZE];
                
                fprintf(fpo, "<track index=\"%d\">\n", trackIndex);
                fprintf(fpo, "<side index=\"0\">\n");
                fprintf(fpo, "<data>\n");
                
                if (fread(data, TRACK_SIZE, 1, fpi))
                {
                    int i;
                    int horizIndex = 0;
                    int inData = FALSE;
                    
                    int linebreak = TRUE;
                    
                    int insertSectorInfo = FALSE;
                    int sectorIndex;
                    
                    for (i = 0; i < TRACK_SIZE; i++)
                    {
                        /* Detect data markers */
                        if (!inData)
                        {
                            /* Detect begin data marker */
                            if ((data[(i + 0) % TRACK_SIZE] == 0xd5) &&
                                (data[(i + 1) % TRACK_SIZE] == 0xaa) &&
                                ((data[(i + 2) % TRACK_SIZE] == 0x96) ||
                                 (data[(i + 2) % TRACK_SIZE] == 0xb5) ||
                                 (data[(i + 2) % TRACK_SIZE] == 0xad)))
                            {
                                inData = 1;
                                
                                if (horizIndex)
                                    horizIndex = DATA_COLNUM;
                                linebreak = TRUE;
                                
                                // Add sector data
                                if (data[(i + 2) % TRACK_SIZE] != 0xad)
                                {
                                    insertSectorInfo = TRUE;
                                    
                                    sectorIndex = (data[(i + 7) % TRACK_SIZE] << 1) | 0x01;
                                    sectorIndex &= data[(i + 8) % TRACK_SIZE];
                                }
                            }
                        }
                        else
                        {
                            /* Detect end data marker */
                            if ((data[(TRACK_SIZE + i - 3) % TRACK_SIZE] == 0xde) &&
                                (data[(TRACK_SIZE + i - 2) % TRACK_SIZE] == 0xaa))
                            {
                                inData = 0;
                                
                                if (horizIndex)
                                    horizIndex = DATA_COLNUM;
                                linebreak = TRUE;
                            }
                        }
                        
                        /* Check horizontal end */
                        if ((horizIndex >= DATA_COLNUM) ||
                            (insertSync && !inData && (horizIndex >= NODATA_COLNUM)))
                        {
                            horizIndex = 0;
                            
                            fprintf(fpo, "\n");
                        }
                        
                        /* Print line break */
                        if (linebreak)
                        {
                            horizIndex = 0;
                            linebreak = FALSE;
                            
                            fprintf(fpo, "\n");
                        }
                        
                        /* Print sector info */
                        if (insertSectorInfo)
                        {
                            insertSectorInfo = FALSE;
                            
                            fprintf(fpo, "<!-- Track %d Sector %d -->\n\n", trackIndex, sectorIndex);
                        }
                        
                        /* Print data */
                        if (horizIndex)
                            fprintf(fpo, " ");
                        
                        fprintf(fpo, "%02x", data[i]);
                        
                        /* Add sync if -s option is enabled */
                        if (insertSync && !inData)
                            fprintf(fpo, "--");
                        
                        horizIndex++;
                    }
                }
                
                fprintf(fpo, "\n\n");
                
                fprintf(fpo, "</data>\n");
                fprintf(fpo, "</side>\n");
                fprintf(fpo, "</track>\n");
            }
            
            fprintf(fpo, "</ddl>\n");
            
            fclose(fpo);
        }
        
        fclose(fpi);
    }
    
    return success;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("usage: nib2ddl [-s] source_file target_file\n");
        
        return 0;
    }
    
    if (argc == 3)
        return convertNIB2DDL(argv[1], argv[2], 0);
    else
    {
        if (strcmp(argv[1], "-s") != 0)
            return 1;
        
        return convertNIB2DDL(argv[2], argv[3], 1);
    }
}
