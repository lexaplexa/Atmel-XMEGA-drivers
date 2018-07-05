/*************************************************************************
 * fs.cpp
 *
 * Created: 1.6.2015 14:59:57
 * Revised: 31.3.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * File system by LeXa
 *
 * File Record Table
 * -----------------------------------------------------------------------
 * Starts in Block 0
 * Size of table is FS_RECORD_TABLE_SIZE blocks
 *
 *
 * File Record in file system table has 32 byte
 * -----------------------------------------------------------------------
 * File type        - 1 byte (FILE_TYPE_enum)
 * Name             - 20 byte (String - ends with 0x00)
 * Start Block      - 4 byte (uint32_t)
 * Number of bytes  - 4 byte (uint32_t)
 * Reserved         - 3 byte
 *
 *************************************************************************/

#include <avr/io.h>
#include <string.h>
#include "fs.h"
#include <conf.h>

SD cSD;

bool FS::Open(const char *pStringName, FILE_HANDLER_struct *sFile)
{
    /*===== FILE OPEN ======================================================
     * Read blocks in FS table and find file record according file name
     * If not found return false
     *======================================================================*/
    uint8_t pBuffer[FS_BLOCK_SIZE];
    
    for (uint8_t i=0; i<FS_RECORD_TABLE_SIZE; i++)
    {
        /* Read block in FS record table */
        if (!cSD.ReadBlock((uint32_t)i, pBuffer)) {return false;}
        for (uint16_t j=0; j<FS_BLOCK_SIZE; j+=FS_FILE_RECORD_LENGTH)
        {
            /* If no record then exit function */
            if (pBuffer[j] == FILE_TYPE_NO_RECORD) {return false;}  
            /* Compare file name */
            if (!strncmp((const char*)pStringName, (const char*)&pBuffer[j+1], 20))
            {
                /* If found then copy file record */
                memcpy(sFile,pBuffer+j,sizeof(FILE_RECORD_struct));
                sFile->unID = i*FS_RECORDS_PER_BLOCK+j/FS_FILE_RECORD_LENGTH;
                return true;
            }
        }
    }
    return false;
}

bool FS::Open(uint16_t unID, FILE_HANDLER_struct *sFile)
{
    /*===== FILE OPEN ======================================================
     * Read blocks in FS table and find file record according order in record table
     * ID number is order of record
     * If not found return false
     *======================================================================*/
    uint8_t pBuffer[FS_BLOCK_SIZE];
    uint32_t unBlockNum;
    uint16_t unFileRecordPos;
    
    unBlockNum = unID/FS_RECORDS_PER_BLOCK;
    if (cSD.ReadBlock(unBlockNum,pBuffer))
    {
        unFileRecordPos = (unID%FS_RECORDS_PER_BLOCK)*FS_FILE_RECORD_LENGTH;
        /* If no record then exit function */
        if (pBuffer[unFileRecordPos] == FILE_TYPE_NO_RECORD) {return false;}
        memcpy(sFile,pBuffer+unFileRecordPos,sizeof(FILE_RECORD_struct));
        sFile->unID = unID;
        return true;
    }
    return false;
}

bool FS::Create(FILE_HANDLER_struct *sFile)
{
    /*===== CREATE FILE ==================================================== 
     * Create new record in File Record Table.
     * File record will be written in first free space
     *======================================================================*/
    uint8_t pBuffer[FS_BLOCK_SIZE];
    FILE_HANDLER_struct sFilePrev;
    
    for (uint32_t i=0; i<FS_RECORD_TABLE_SIZE; i++)
    {
        cSD.ReadBlock(i,pBuffer);
        for (uint8_t j=0; j<FS_BLOCK_SIZE; j+=FS_FILE_RECORD_LENGTH)
        {
            if (pBuffer[j] == FILE_TYPE_NO_RECORD) 
            {               
                sFile->unID = i*FS_RECORDS_PER_BLOCK+j/FS_FILE_RECORD_LENGTH;
                /* Open previous file to set StartBlock of new record */
                Open(sFile->unID-1,&sFilePrev);
                if (sFilePrev.sRecord.unNumOfBytes%FS_BLOCK_SIZE) {sFile->sRecord.unStartBlock = sFilePrev.sRecord.unStartBlock + (sFilePrev.sRecord.unNumOfBytes/FS_BLOCK_SIZE) + 1;}
                else {sFile->sRecord.unStartBlock = sFilePrev.sRecord.unStartBlock + (sFilePrev.sRecord.unNumOfBytes/FS_BLOCK_SIZE);}
                /* Create new record */         
                memcpy(pBuffer+j,&sFile->sRecord,sizeof(FILE_RECORD_struct));
                cSD.WriteBlock(i,pBuffer);
                return true;
            }
        }
    }
    return false;
}

bool FS::Delete(FILE_HANDLER_struct *sFile)
{
    /*===== DELETE FILE ==================================================== 
     * Find file record according order in record table and delete this record
     * ID number is order of record
     * If not found return false
     *======================================================================*/
    uint8_t pBuffer[FS_BLOCK_SIZE];
    uint32_t unBlockNum;
    uint16_t unFileRecordPos;
    
    unBlockNum = sFile->unID*FS_FILE_RECORD_LENGTH/FS_BLOCK_SIZE;
    if (cSD.ReadBlock(unBlockNum,pBuffer))
    {
        unFileRecordPos = (sFile->unID%FS_RECORDS_PER_BLOCK)*FS_FILE_RECORD_LENGTH;
        /* Clear record */
        for (uint8_t i=0; i<sizeof(FILE_RECORD_struct); i++) {*(pBuffer+unFileRecordPos+i) = 0;}
        if (cSD.WriteBlock(unBlockNum,pBuffer)) {return true;}
        return false;
    }
    return false;
}

bool FS::Read(FILE_HANDLER_struct *sFile, uint32_t unBlockNum, uint8_t *punBuffer)
{
    /*===== FILE BLOCK READ ================================================ 
     * First block of file starts with 0 => sFile->unStartBlock + 0 = unStartBlock 
     * If unBlockNum exceeds sFile->unNumOfBytes then false is returned
     *======================================================================*/
    if (sFile->sRecord.unNumOfBytes > (FS_BLOCK_SIZE*unBlockNum)) 
    {
        return cSD.ReadBlock(sFile->sRecord.unStartBlock + unBlockNum, punBuffer);
    }
    return false;
}

bool FS::Write(FILE_HANDLER_struct *sFile, uint32_t unBlockNum, uint8_t *punBuffer)
{
    /*===== FILE BLOCK WRITE =============================================== 
     * First block of file starts with 0 => sFile->unStartBlock + 0 = unStartBlock 
     * If unBlockNum exceeds sFile->unNumOfBytes then false is returned
     *======================================================================*/
    if (sFile->sRecord.unNumOfBytes > (FS_BLOCK_SIZE*unBlockNum))
    {
        return cSD.WriteBlock(sFile->sRecord.unStartBlock + unBlockNum, punBuffer);
    }
    return false;
}