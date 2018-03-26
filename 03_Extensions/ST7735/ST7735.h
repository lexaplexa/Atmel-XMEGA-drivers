/*
 * ST7735.h
 *
 * Created: 25.5.2016 9:03:10
 * Revised: 31.5.2016
 *  Author: Matej
 */ 



#ifndef ST7735_H_
#define ST7735_H_

#ifndef F_CPU
	#warning F_CPU Defined by ST7735 to 32MHz
	#define F_CPU 32000000U
#endif

#include <avr/io.h>
#include "Drivers/spi.h"

/*===== HARDWARE CONNECTION ================================================*/

#ifndef ST7735_SPI
	//#warning Default configuration ST7735_SPI
	#define ST7735_SPI					SPIC
#endif
#ifndef ST7735_PORT
	//#warning Default configuration ST7735_SPI
	#define ST7735_PORT					PORTC
#endif

#ifndef MCP_BAUD
	//#warning Default configuration MCP_BAUD
	#define MCP_BAUD				1600000
#endif

#define SCK				PIN7_bp
#define SDA				PIN5_bp
#define A0				PIN2_bp
#define RES				PIN3_bp
#define CS				PIN4_bp

/*===== COMMANDS ===========================================================*/

enum ST7735_CMD_enum {
	ST7735_CMD_NOP 			= 0x00,		/* No operation */
	ST7735_CMD_SWRESET		= 0x01,		/* Software reset */
	ST7735_CMD_RDDID		= 0x04,		/* Read Display ID */
	ST7735_CMD_RDDST		= 0x09,		/* Read Display Status */
	ST7735_CMD_RDDPM		= 0x0A,		/* Read Display Power */
	ST7735_CMD_RDD_MADCTL	= 0x0B,		/* Read display */
	ST7735_CMD_RDD_COLMOD	= 0x0C,		/* Read Display Pixel */
	ST7735_CMD_RDDIM		= 0x0D,		/* Read Display Image */
	ST7735_CMD_RDDSM		= 0x0E,		/* Read Display Signal */
	ST7735_CMD_SLPIN		= 0x10,		/* Sleep in */
	ST7735_CMD_SLPOUT		= 0x11,		/* Sleep out */
	ST7735_CMD_PTLON		= 0x12,		/* Partial mode on */
	ST7735_CMD_NORON		= 0x13,		/* Normal display */
	ST7735_CMD_INOFF		= 0x20,		/* Display inversion off */
	ST7735_CMD_INVON		= 0x21,		/* Display inversion on */
	ST7735_CMD_GAMSET		= 0x26,		/* Gamma curve select */
	ST7735_CMD_DISPOFF		= 0x28,		/* Display off */
	ST7735_CMD_DISPON		= 0x29,		/* Display on */
	ST7735_CMD_CASET		= 0x2A,		/* Column address set */
	ST7735_CMD_RASET		= 0x2B,		/* Row address set */
	ST7735_CMD_RAMWR		= 0x2C,		/* RAM write */
	ST7735_CMD_RAMRD		= 0x2E,		/* RAM read */
	ST7735_CMD_PTLAR		= 0x30, 	/* Partial start/end address set */
	ST7735_CMD_TEOFF		= 0x34,		/* Tearing output off */
	ST7735_CMD_TEON			= 0x35,		/* Tearing output on */
	ST7735_CMD_MADCTL		= 0x36,		/* Ram address control */
	ST7735_CMD_IDMOFF		= 0x38,		/* Idle mode off */
	ST7735_CMD_IDMON		= 0x39,		/* Idle mode on */
	ST7735_CMD_COLMOD		= 0x3A,		/* I/F format */
	ST7735_CMD_RDID1		= 0xDA,		/* Read ID1 */
	ST7735_CMD_RDID2		= 0xDB,		/* Read ID2 */
	ST7735_CMD_RDID3		= 0xDD,		/* Read ID3 */
};

/*===== COLMOD ===============================================================*/

enum COLMOD_enum {
	COLMOD_4K		= 3,
	COLMOD_64K		= 5,
	COLMOD_256K		= 6,
};

/*===== MEMORY =============================================================
 * There are different memory types from which can be read data to display.
 *==========================================================================*/

enum MEMORY_enum {
	MEMORY_RAM,
	MEMORY_PROGRAM
};

/*===== COLOURS =============================================================
 * Unions of colors. Area "color" is for sending data, structure for defining
 * of elementary colors.
 *==========================================================================*/

union COLOR4K_union{
	uint8_t		aunColor[2];
	uint16_t	u16Color;
	struct{
		uint16_t unEmpty:4;
		uint16_t unBlue:4;
		uint16_t unGreen:4;
		uint16_t unRed:4;
	};
	
};

union COLOR64K_union{
	uint8_t aunColor[2];
	struct{
		uint16_t unBlue:5;
		uint16_t unGreen:6;
		uint16_t unRed:5;
	};
};

union COLOR256K_union{
	uint8_t aunColor[3];
	struct{
		uint8_t unBlueEmpty:2;
		uint8_t unBlue:6;
		uint8_t unGreenEmpty:2;
		uint8_t unGreen:6;
		uint8_t unRedEmpty:2;
		uint8_t unRed:6;
	};
};

	
/*===== MADCTL ===============================================================
 * Parameter bits:
 * Bit5 - 0 vertical screen; 1 horizontal screen;
 * Bit6 - 0 column address (0h-140h, from left to right); 1 column address (140h-0h, from right to left)
 * Bit7 - 0 page(row) address (0h-140h, from top to bottom); 1 page(row) address (140h-0h, from bottom to top)
 *==========================================================================*/

#define MADCTL_P 0xA0

/* VERTICAL SCREEN */
#if (MADCTL_P == 0x00)		/* writing to screen begins in TOP LEFT corner */
	#define ROW_ZERO		0
	#define ROW_END			127
	#define COL_ZERO		0
	#define COL_END			127
#elif (MADCTL_P == 0x40)	/* writing to screen begins in TOP RIGHT corner */
	#define ROW_ZERO		0
	#define ROW_END			127
	#define COL_ZERO		0
	#define COL_END			127
#elif (MADCTL_P == 0x80)	/* writing to screen begins in BOTTOM LEFT corner */
	#define ROW_ZERO		32
	#define ROW_END			159
	#define COL_ZERO		0
	#define COL_END			127
#elif (MADCTL_P == 0xC0)	/* writing to screen begins in BOTTOM RIGHT corner */
	#define ROW_ZERO		32
	#define ROW_END			159
	#define COL_ZERO		0
	#define COL_END			127

/* HORIZONTAL SCREEN */
#elif (MADCTL_P == 0x20)	/* writing to screen begins in TOP LEFT corner */
	#define ROW_ZERO		0
	#define ROW_END			127
	#define COL_ZERO		0
	#define COL_END			127
#elif (MADCTL_P == 0x60)	/* writing to screen begins in TOP RIGHT corner */
	#define ROW_ZERO		0
	#define ROW_END			127
	#define COL_ZERO		0
	#define COL_END			127
#elif (MADCTL_P == 0xA0)	/* writing to screen begins in BOTTOM LEFT corner */
	#define ROW_ZERO		0
	#define ROW_END			127
	#define COL_ZERO		32
	#define COL_END			159
#elif (MADCTL_P == 0xE0)	/* writing to screen begins in BOTTOM RIGHT corner */
	#define ROW_ZERO		0
	#define ROW_END			127
	#define COL_ZERO		32
	#define COL_END			159
#endif



/*===== CLASS ==============================================================*/

class ST7735
{
	private:

	public:
		ST7735();
		void SendData(uint8_t unData);
		void SendCommand(ST7735_CMD_enum eCommand);
		void CaSet(uint8_t unStart, uint8_t unEnd);
		void RaSet(uint8_t unStart, uint8_t unEnd);
		void Dot(uint8_t unRow, uint8_t unCol, COLOR4K_union unionColor);
		void Dot(uint8_t unRow, uint8_t unCol, COLOR64K_union unionColor);
		void Dot(uint8_t unRow, uint8_t unCol, COLOR256K_union unionColor);
		void ColorDisplay(COLOR4K_union unionColor);
		void ColorDisplay(COLOR64K_union unionColor);
		void ColorDisplay(COLOR256K_union unionColor);
		void ColorArea(uint8_t unRowStart, uint8_t unRowEnd, uint8_t unColStart, uint8_t unColEnd, COLOR4K_union unionColor);
		void ColorArea(uint8_t unRowStart, uint8_t unRowEnd, uint8_t unColStart, uint8_t unColEnd, COLOR64K_union unionColor);
		void ColorArea(uint8_t unRowStart, uint8_t unRowEnd, uint8_t unColStart, uint8_t unColEnd, COLOR256K_union unionColor);
		void SendPixelData(uint8_t *punData, uint16_t unLength, MEMORY_enum eMemory);
		void GRAFICcircle(uint8_t unRowPos, uint8_t unColPos, uint8_t unRadius, COLOR4K_union);
		void GRAFICcircle(uint8_t unRowPos, uint8_t unColPos, uint8_t unRadius, COLOR64K_union);
		void GRAFICcircle(uint8_t unRowPos, uint8_t unColPos, uint8_t unRadius, COLOR256K_union);
		void PrintPicture(const uint8_t *punPicture, uint8_t unRowPos, uint8_t unColPos);
		void Println(uint8_t *pchText, uint8_t unRowPos, uint8_t unColPos, uint8_t unColClear, COLOR4K_union unionColorActiv, COLOR4K_union unionColorBack);
		void Println(uint8_t *pchText, uint8_t unRowPos, uint8_t unColPos, uint8_t unColClear, COLOR64K_union unionColorActiv, COLOR64K_union unionColorBack);
		void Println(uint8_t *pchText, uint8_t unRowPos, uint8_t unColPos, uint8_t unColClear, COLOR256K_union unionColorActiv, COLOR256K_union unionColorBack);
};

#endif /* ST7735_H_ */