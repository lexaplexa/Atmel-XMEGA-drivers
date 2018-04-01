/*************************************************************************
 * fs.h
 *
 * Created: 29.5.2015 14:11:05
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


#ifndef FS_H_
#define FS_H_

#include <extensions/sd/sd.h>

#define FS_FILE_RECORD_LENGTH           32              /* bytes */
#define FS_BLOCK_SIZE                   SD_BLOCK_SIZE   /* bytes */
#define FS_RECORD_TABLE_SIZE            100             /* blocks */
#define FS_RECORDS_PER_BLOCK            (FS_BLOCK_SIZE/FS_FILE_RECORD_LENGTH)


/*===== FILE TYPE ==========================================================
 *==========================================================================*/
enum FILE_TYPE_enum {
    FILE_TYPE_NO_RECORD,
    FILE_TYPE_DIRECTORY,
    FILE_TYPE_GENERAL,
    FILE_TYPE_PICTURE,
    FILE_TYPE_TEXT,
    };


/*===== FILE RECORD STRUCTURE ==============================================
 * Describes file record in file record table
 *==========================================================================*/
struct FILE_RECORD_struct {
    FILE_TYPE_enum eFileType;
    uint8_t pString[20];
    uint32_t unStartBlock;
    uint32_t unNumOfBytes;
//  uint8_t reserved[3];
    };

/*===== FILE HANDLER STRUCTURE =============================================
 * Includes file record and additional information about file
 *==========================================================================*/
struct FILE_HANDLER_struct {
    FILE_RECORD_struct sRecord;
    uint16_t unID;
    };

/*===== CLASS ==============================================================
 *==========================================================================*/
class FS
{
    private:
            
    public:
        /*------------------------------------------------------- 
        * Open file according his name
        * Input Parameters
        *		*pStringName	- file name
        * Output Parameters
        *		*sFile			- pointer to file handler (if file found)
        * Return
        *		true			- file found
        *		false			- file not found
        *-------------------------------------------------------*/
        bool Open(const char *pStringName, FILE_HANDLER_struct *sFile);
        
        /*------------------------------------------------------- 
        * Open file according ID
        * Input Parameters
        *		unID			- file ID (according order of records)
        * Output Parameters
        *		*sFile			- pointer to file handler (if file found)
        * Return
        *		true			- file found
        *		false			- file not found
        *-------------------------------------------------------*/
        bool Open(uint16_t unID, FILE_HANDLER_struct *sFile);
        
        /*------------------------------------------------------- 
        * Create file record
        * Input/Output Parameters
        *		*sFile			- pointer to file handler
        * Return
        *		true			- file created (start block added)
        *		false			- file not created
        *-------------------------------------------------------*/
        bool Create(FILE_HANDLER_struct *sFile);
        
        /*------------------------------------------------------- 
        * Delete file record
        * Input Parameters
        *		*sFile			- pointer to file handler
        * Return
        *		true			- file deleted
        *		false			- file not deleted
        *-------------------------------------------------------*/
        bool Delete(FILE_HANDLER_struct *sFile);
        
        /*------------------------------------------------------- 
        * Read file data
        * Input Parameters
        *		*sFile			- pointer to file handler
        *		unBlockNum		- block number to read (0..n)
        * Output Parameters
        *		*punBuffer		- pointer to data buffer
        * Return
        *		true			- file data read successful
        *		false			- file data read unsuccessful
        *-------------------------------------------------------*/
        bool Read(FILE_HANDLER_struct *sFile, uint32_t unBlockNum, uint8_t *punBuffer);
        
        /*------------------------------------------------------- 
        * Write file data
        * Input Parameters
        *		*sFile			- pointer to file handler
        *		unBlockNum		- block number to write (0..n)
        *		*punBuffer		- pointer to data buffer
        * Return
        *		true			- file data write successful
        *		false			- file data write unsuccessful
        *-------------------------------------------------------*/
        bool Write(FILE_HANDLER_struct *sFile, uint32_t unBlockNum, uint8_t *punBuffer);
};

#endif /* FS_H_ */