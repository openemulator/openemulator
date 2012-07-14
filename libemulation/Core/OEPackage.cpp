
/**
 * libemulation
 * OEPackage
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a file package (zip and directory type)
 */

#include <fstream>

#include "OEPackage.h"

OEPackage::OEPackage()
{
    is_open = false;
    
    zip = NULL;
}

OEPackage::~OEPackage()
{
    close();
}

bool OEPackage::open(const string& path)
{
    close();
    this->path = path;
    
    bool isPackage;
    if (isPathValid(path))
        isPackage = isPathADirectory(path);
    else
        isPackage = (path.substr(path.size() - 1, 1) == PATH_SEPARATOR);
    
    if (isPackage)
    {
        createDirectory(path);
        
        is_open = isPathValid(path);
    }
    else
    {
        zip = zip_open(path.c_str(), ZIP_CREATE, NULL);
        
        is_open = (zip != NULL);
    }
    
    return is_open;
}

bool OEPackage::isOpen()
{
    return is_open;
}

void OEPackage::close()
{
    is_open = false;
    
    if (zip)
        zip_close(zip);
    
    zip = NULL;
}

bool OEPackage::read(const string& packagePath, OEData *data)
{
    bool error = true;
    
    if (!is_open)
        return false;
    
    if (zip)
    {
        struct zip_stat zipStat;
        struct zip_file *zipFile;
        
        if (zip_stat(zip, (const char *) packagePath.c_str(), 0, &zipStat) == 0)
        {
            if ((zipFile = zip_fopen(zip, packagePath.c_str(), 0)) != NULL)
            {
                data->resize((size_t) zipStat.size);
                error = (zip_fread(zipFile,
                                   &data->front(),
                                   data->size()) != zipStat.size);
                zip_fclose(zipFile);
            }
        }
    }
    else
    {
        string filePath = path + PATH_SEPARATOR + packagePath;
        
        error = !readFile(filePath, data);
    }
    
    return !error;
}

bool OEPackage::write(const string& packagePath, OEData *data)
{
    bool error = true;
    
    if (!is_open)
        return false;
    
    if (zip)
    {
        struct zip_source *zipSource = NULL;
        
        if ((zipSource = zip_source_buffer(zip,
                                           &data->front(), data->size(),
                                           0)) != NULL)
        {
            int index;
            
            if ((index = zip_name_locate(zip, packagePath.c_str(), 0)) == -1)
                error = (zip_add(zip, packagePath.c_str(), zipSource) == -1);
            else
                error = (zip_replace(zip, index, zipSource) == -1);
            
            zip_source_free(zipSource);
        }
    }
    else
    {
        string filePath = path + PATH_SEPARATOR + packagePath;
        
        error = !writeFile(filePath, data);
    }
    
    return !error;
}

bool OEPackage::remove()
{
    close();
    
    return removePath(path);
}
