/*************************************************************************
 * display.cpp
 *
 * Created: 27.5.2015 23:08:19
 * Revised: 24.2.2017
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 *
 *************************************************************************/

#include <avr/io.h>
#include "display.h"
#include <util/delay.h>
#include <avr/pgmspace.h>


void DispInit()
{
	DISP_DATA_PORT.OUT = 0x00;
	DISP_DATA_PORT.DIRSET = 0xFF;					/* Data ports set as output */	
	DISP_CMD_PORT.OUT = 0x00;
	DISP_CMD_PORT.DIRSET = 1<<DISP_XCS_PIN|1<<DISP_A0_PIN|1<<DISP_XWR_PIN|1<<DISP_XRES_PIN;	/* Command pins set as output */	

	_delay_us(50);									/* Waiting after switch VCC */
	DISP_CMD_PORT.OUTSET = 1<<DISP_XRES_PIN;		/* HW reset off */
	_delay_ms(10);
	DispSendCommand(DISP_CMD_SLPOUT);				/* Sleep out */
	_delay_ms(10);
	DispSendCommand(DISP_CMD_TEOFF);				/* Tearing output off */
	DispSendCommand(DISP_CMD_COLMOD);				/* I/F format settings */
	DispSendData(COLMOD_2GS);						/* I/F format parameter */
	DispSendCommand(DISP_CMD_NORON);				/* Normal display mode, no partial or scroll mode */
	DispSendCommand(DISP_CMD_DISINOFF);				/* Display inversion off */
	DispSendCommand(DISP_CMD_MADCTL);				/* Writing to RAM mode */
	DispSendData(MADCTL_P);							/* parameter for MADCTL */
	DispSendCommand(DISP_CMD_DISPON);				/* Display on */
	DispGreyArea(0,ROW_END-ROW_ZERO,0,COL_END-COL_ZERO,COLMOD_2GS,0);	/* Makes the screen blank */
}

void DispDot(uint16_t unRow, uint16_t unCol, COLMOD_enum eColmod, uint8_t unGrayLevel)
{
	/* Set COLMOD */
	DispSendCommand(DISP_CMD_COLMOD);
	DispSendData(eColmod);
	/* Set RAM area */
	CaSet(unCol, unCol);
	PaSet(unRow, unRow);
	DispSendCommand(DISP_CMD_RAMWR);
	
	/* Prepare to sending only data (pixels) */
	DISP_CMD_PORT.OUT = 0<<DISP_XCS_PIN|1<<DISP_A0_PIN|1<<DISP_XWR_PIN|1<<DISP_XRES_PIN;
	
	/* Set gray level for one byte */
	if (eColmod == COLMOD_2GS && unGrayLevel) {DISP_DATA_PORT.OUT = 0xFF;}
	else if (eColmod == COLMOD_4GS) {DISP_DATA_PORT.OUT = unGrayLevel<<6;}
	else if (eColmod == COLMOD_16GS) {DISP_DATA_PORT.OUT = unGrayLevel<<4;}
	else {DISP_DATA_PORT.OUT = 0x00;}
	
	DISP_CMD_PORT.OUTCLR = 1<<DISP_XWR_PIN;		/* XWR set LOW */
	DISP_CMD_PORT.OUTSET = 1<<DISP_XWR_PIN;		/* XWR set HIGH */
	
	DISP_CMD_PORT.OUTSET = 1<<DISP_XCS_PIN;		/* Chip is not selected */
}

void DispGreyArea(uint16_t unRowStart, uint16_t unRowEnd, uint16_t unColStart, uint16_t unColEnd, COLMOD_enum eColmod, uint8_t unGrayLevel)
{
	uint32_t i;
	uint32_t count;

	/* Set COLMOD 2GS */
	DispSendCommand(DISP_CMD_COLMOD);
	DispSendData(eColmod);
	/* Set RAM area */
	CaSet(unColStart, unColEnd);
	PaSet(unRowStart, unRowEnd);
	DispSendCommand(DISP_CMD_RAMWR);
	
	/* Calculate number of write cycles */
	count = (uint32_t)(unRowEnd-unRowStart+1);
	if ((unColEnd-unColStart+1) % 8) {count *= (unColEnd-unColStart+1)/8 + 1;}
	else {count *= (unColEnd-unColStart+1)/8;}
	
	/* Prepare to sending only data (pixels) */
	DISP_CMD_PORT.OUT = 0<<DISP_XCS_PIN|1<<DISP_A0_PIN|1<<DISP_XWR_PIN|1<<DISP_XRES_PIN;
	
	/* Set gray level for one byte */
	if (eColmod == COLMOD_2GS && unGrayLevel) {DISP_DATA_PORT.OUT = 0xFF;}
	else if (eColmod == COLMOD_4GS) {DISP_DATA_PORT.OUT = unGrayLevel|unGrayLevel<<2|unGrayLevel<<4|unGrayLevel<<6; count *= 2;}
	else if (eColmod == COLMOD_16GS) {DISP_DATA_PORT.OUT = unGrayLevel|unGrayLevel<<4; count *= 4;}
	else {DISP_DATA_PORT.OUT = 0x00;}
	
	for (i=1;i<=count;i++)
	{
		DISP_CMD_PORT.OUTCLR = 1<<DISP_XWR_PIN;	/* XWR set LOW */
		DISP_CMD_PORT.OUTSET = 1<<DISP_XWR_PIN;	/* XWR set HIGH */
	}
	DISP_CMD_PORT.OUTSET = 1<<DISP_XCS_PIN;		/* Chip is not selected */
}

void DispPrintln(uint8_t *pchText, uint8_t **paFont, uint16_t unRowPos, uint16_t unColPos, uint16_t unColClear)
{
	/*===== FONT STRUCTURE =================================================
	 * Font is stored in Program memory
	 * pchText[0]		= number of rows
	 * pchText[1]		= number of columns
	 * pchText[2]		= number of pixel data bytes
	 * pchText[3..n]	= pixel data
	 *======================================================================*/
	uint8_t *pchSign;
	uint16_t unByteCount;
	uint16_t unStartColumn;
	uint16_t unNumOfChars;
	
	unStartColumn = unColPos;
	unNumOfChars = 0;
	
	/* Set COLMOD */
	DispSendCommand(DISP_CMD_COLMOD);
	DispSendData(COLMOD_2GS);
	
	/* Print signs on screen */
	while (*pchText)
	{
		/* Line Feed or Carriage return clear the rest of the line and continues in next line */
		if (*pchText == '\n' || *pchText == '\r')
		{
			/* Clear rest of line */
			if (unColPos <= unColClear) {DispGreyArea(unRowPos-1,unRowPos+(uint16_t)paFont[0],unColPos,unColClear,COLMOD_2GS,0);}
			unColPos = unStartColumn;
			unRowPos += (uint16_t)paFont[0]+1;
			pchText++;
			unNumOfChars++;
			continue;
		}
		
		/* Pointer to sign. *aFont[0] = FONT_SIZE, others pixel data */
		pchSign = (uint8_t*)paFont[*pchText - ' ' + 1];
		
		/* Count of pixel data bytes */
		unByteCount = pgm_read_byte(pchSign+2);
		
		/* Send data to display */
		CaSet(unColPos, unColPos+pgm_read_byte(pchSign+1)-1);
		PaSet(unRowPos, unRowPos+pgm_read_byte(pchSign)-1);
		SendPixelData((uint8_t *)(pchSign+3),unByteCount,MEMORY_PROGRAM);
		
		/* Position of next sign, clear space between signs */
		unColPos += pgm_read_byte(pchSign+1)+2;
		DispGreyArea(unRowPos,unRowPos+pgm_read_byte(pchSign)-1,unColPos-2,unColPos-1,COLMOD_2GS,0);
		pchText++;
		unNumOfChars++;
	}	
	/* Clear rest of line */
	if (unColPos <= unColClear) {DispGreyArea(unRowPos,unRowPos+(uint16_t)paFont[0],unColPos,unColClear,COLMOD_2GS,0);}
}

void DispPrintPicture(const uint8_t *punPicture, uint16_t unRowPos, uint16_t unColPos)
{
	/*===== PICTURE STRUCTURE ==============================================
	 * Picture is stored in Program memory
	 * punPicture[0,1]	= number of rows (data type uint16_t - low byte first)
	 * punPicture[2,3]	= number of columns (data type uint16_t - low byte first)
	 * punPicture[4]	= COLMOD (2GS,4GS or 16GS)
	 * punPicture[5,6]	= number of pixel data bytes (data type uint16_t - low byte first)
	 * punPicture[7..n]	= pixel data
	 *======================================================================*/
	
	/* Set COLMOD */
	DispSendCommand(DISP_CMD_COLMOD);
	DispSendData(pgm_read_byte(punPicture+4));
	
	/* Send data to display */
	PaSet(unRowPos,unRowPos+pgm_read_word(punPicture)-1);
	CaSet(unColPos,unColPos+pgm_read_word(punPicture+2)-1);
	SendPixelData((uint8_t*)punPicture+7,pgm_read_word(punPicture+5),MEMORY_PROGRAM);
}

#ifdef COMPILE_DISP_PRINT_FILE
void DispPrintPicture(FILE_HANDLER_struct *sFile, uint16_t unRowPos, uint16_t unColPos)
{
	uint8_t aBuffer[FS_BLOCK_SIZE];
	uint16_t unLength;
	uint16_t unRest;
	FS cFS;
	
	/*===== PICTURE STRUCTURE ==============================================
	 * Picture is stored in SD card
	 * punPicture[0,1]	= number of rows (data type uint16_t - low byte first)
	 * punPicture[2,3]	= number of columns (data type uint16_t - low byte first)
	 * punPicture[4]	= COLMOD (2GS,4GS or 16GS)
	 * punPicture[5,6]	= number of pixel data bytes (data type uint16_t - low byte first)
	 * punPicture[7..n]	= pixel data
	 *======================================================================*/
	
	/* Print only picture files */
	if (sFile->sRecord.eFileType != FILE_TYPE_PICTURE) {return;}
	/* Read first block from file system */
	if (!cFS.Read(sFile,0,aBuffer)) {return;}
	
	/* Set COLMOD */
	DispSendCommand(DISP_CMD_COLMOD);
	DispSendData(aBuffer[4]);
	/* Set RAM area (Screen area) */
	PaSet(unRowPos,unRowPos+*(uint16_t*)&aBuffer[0]-1);
	CaSet(unColPos,unColPos+*(uint16_t*)&aBuffer[2]-1);
	
	/* Picture data length */
	unLength = *(uint16_t*)&aBuffer[5];
	
	/* If picture size is not bigger then one block */
	if (unLength <= (FS_BLOCK_SIZE-7)) {SendPixelData((uint8_t*)(aBuffer+7),unLength,MEMORY_RAM); return;}
	
	/* Write to RAM */
	DispSendCommand(DISP_CMD_RAMWR);
	DISP_CMD_PORT.OUT = 0<<DISP_XCS_PIN|1<<DISP_A0_PIN|1<<DISP_XWR_PIN|1<<DISP_XRES_PIN;		/* Before sending data */
	
	/* Fill screen with first block */
	for(uint16_t i=7;i<FS_BLOCK_SIZE;i++)
	{
		DISP_DATA_PORT.OUT = aBuffer[i];				/* Read data from RAM */
		DISP_CMD_PORT.OUTCLR = 1<<DISP_XWR_PIN;			/* XWR set LOW */
		DISP_CMD_PORT.OUTSET = 1<<DISP_XWR_PIN;			/* XWR set HIGH */
	}
	
	unLength -= FS_BLOCK_SIZE-7;							/* decrement length */
	unRest = FS_BLOCK_SIZE;
	
	/* Fill screen with next blocks */
	for (uint8_t i=1; unLength>0; i++)
	{
		cFS.Read(sFile,i,aBuffer);
		if (unLength <= FS_BLOCK_SIZE) {unRest = unLength;}		/* Fill the rest pixels on screen */
			
		for(uint16_t j=0;j<unRest;j++)
		{
			DISP_DATA_PORT.OUT = aBuffer[j];				/* Read data from RAM */
			DISP_CMD_PORT.OUTCLR = 1<<DISP_XWR_PIN;			/* XWR set LOW */
			DISP_CMD_PORT.OUTSET = 1<<DISP_XWR_PIN;			/* XWR set HIGH */
		}		
		unLength -= unRest;									/* decrement length */
	}
	DISP_CMD_PORT.OUTSET = 1<<DISP_XCS_PIN;				/* Chip is not selected */
	return;
}
#endif

