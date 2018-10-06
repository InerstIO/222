#include "pfm.h"

PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance()
{
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}


PagedFileManager::PagedFileManager()
{
}


PagedFileManager::~PagedFileManager()
{
}

// The file exists, and is open for input
RC PagedFileManager::createFile(const string &fileName)
{
    ifstream ifile(fileName);
    if (ifile) {
	   return FILE_EXISTED;
    } else{
	   ofstream outfile (fileName);
	   outfile.close();
	   return SUCCESS;
    }
    return -1;
}


RC PagedFileManager::destroyFile(const string &fileName)
{
    RC rc = -1;
    if( std::remove(fileName.c_str()) == 0){
        rc = SUCCESS;
    } else{
	   rc= FILE_DELETE_FAIL;
    }
    return rc;
}


RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
    RC rc = -1;
    if(fileHandle.isOpen){
	   rc = HANDLE_USED;
    } else{
	   fileHandle.filefs.open(fileName);
	   if(fileHandle.filefs.is_open()){
            fileHandle.isOpen = true;
	       rc = SUCCESS;
	   } else{
	       rc = OPEN_HANDLE_FAIL;
	   }
    }
    return rc;
}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
    fileHandle.filefs.flush();
    fileHandle.filefs.close();
    return SUCCESS;
}


FileHandle::FileHandle()
{
    readPageCounter = 0;
    writePageCounter = 0;
    appendPageCounter = 0;
    isOpen = false;
}


FileHandle::~FileHandle()
{
    fileHandle.filefs.flush();
    fileHandle.filefs.close();
}


RC FileHandle::readPage(PageNum pageNum, void *data)
{
    RC rc = -1;
    //if(sizeof(*(char*)data) == PAGE_SIZE){
        if(pageNum <= getNumberOfPages()){
            filefs.seekg(pageNum*PAGE_SIZE, filefs.beg);
            filefs.read((char*)data, PAGE_SIZE);
            if(filefs){
                rc = SUCCESS;
            } else{
                rc = READ_PAGE_FAIL;
            }
        } else{
            rc = PAGENUM_EXCEED;
        }
    //} else{
    //    rc = WRONG_DATA_SIZE;
    //}
    readPageCounter++;
    return rc;
}


RC FileHandle::writePage(PageNum pageNum, const void *data)
{
    RC rc = -1;
    //if(sizeof(*(char*)data) == PAGE_SIZE){
        if(pageNum <= getNumberOfPages()){
            filefs.seekg(pageNum*PAGE_SIZE, filefs.beg);
            filefs.write((char*)data, PAGE_SIZE);
            if(filefs){
                rc = SUCCESS;
            } else{
                rc = WRITE_PAGE_FAIL;
            }
        } else{
            rc = PAGENUM_EXCEED;
        }
    //} else{
      //  rc = WRONG_DATA_SIZE;
    //}
    writePageCounter++;
    return rc;
}


RC FileHandle::appendPage(const void *data)
{
    RC rc = -1;
    //cout<<sizeof((char*)data)<<endl;
    //if(sizeof(*(char*)data) == PAGE_SIZE){
        //cout<<"hyx1"<<endl;
        filefs<<(char*)data;
        //cout<<"hyx2"<<endl;
        rc = SUCCESS;
    //} else{
      //  rc = WRONG_DATA_SIZE;
    //}
    appendPageCounter++;
    return rc;
}


unsigned FileHandle::getNumberOfPages()
{
    return appendPageCounter;
}


RC FileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
    readPageCount = readPageCounter;
    writePageCount = writePageCounter;
    appendPageCount = appendPageCounter;
    return 0;
}
