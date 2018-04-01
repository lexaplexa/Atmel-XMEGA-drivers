/*
 * ST7735.cpp
 *
 * Created: 25.5.2016 9:34:24
 * Revised: 1.4.2018
 * Author: Matej
 */ 

#include <avr/io.h>
#include "ST7735.h"
#include <util/delay.h>
#include "Drivers/spi.h"
#include <avr/pgmspace.h>
#include "Extensions/font_dos.h"


ST7735::ST7735()
{
    ST7735_PORT.DIRSET = 0xFF;      // Pins as output
    ST7735_PORT.OUT = 0x00;
    ST7735_PORT.OUTSET = 1<<CS;     // Chip select to high
    
    
    
    /* Setup SPI driver */
    spi_set_master_port_dir(ST7735_PORT);
    spi_cs_disable(ST7735_PORT);
    spi_set_mode(ST7735_SPI,SPI_MODE_0_gc);
    spi_set_dord(ST7735_SPI,SPI_DORD_MSB_FIRST);
    spi_enable_master(ST7735_SPI);
    
    /* Set SPI Baudrate */
    #if (ST7735_BAUD <= F_CPU/128)
    ST7735_SPI.CTRL |= SPI_PRESCALER_DIV128_gc;
    #elif (ST7735_BAUD <= F_CPU/64)
    ST7735_SPI.CTRL |= SPI_PRESCALER_DIV64_gc;
    #elif (ST7735_BAUD <= F_CPU/32)
    ST7735_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV64_gc;
    #elif (ST7735_BAUD <= F_CPU/16)
    ST7735_SPI.CTRL |= SPI_PRESCALER_DIV16_gc;
    #elif (ST7735_BAUD <= F_CPU/8)
    ST7735_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV16_gc;
    #elif (ST7735_BAUD <= F_CPU/4)
    ST7735_SPI.CTRL |= SPI_PRESCALER_DIV4_gc;
    #elif (ST7735_BAUD <= F_CPU/2)
    ST7735_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
    #else
    ST7735_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
    #endif
    
    _delay_us(50);                      // Wait after switch VCC
    ST7735_PORT.OUTSET = 1<<RES;        // HW reset off
    _delay_ms(200);
    SendCommand(ST7735_CMD_SLPOUT);
    _delay_ms(50);
    SendCommand(ST7735_CMD_TEOFF);          /* Tearing output off */
    SendCommand(ST7735_CMD_COLMOD);         /* I/F format settings */
    SendData(COLMOD_64K);                   /* I/F format parameter */
    SendCommand(ST7735_CMD_NORON);          /* Normal display mode, no partial or scroll mode */
    SendCommand(ST7735_CMD_INOFF);          /* Display inversion off */
    SendCommand(ST7735_CMD_MADCTL);         /* Writing to RAM mode */
    SendData(MADCTL_P|0x08);                /* parameter for MADCTL */
    SendCommand(ST7735_CMD_DISPON);         /* Display on */
    
}

void ST7735::SendData(uint8_t unData)
{
    ST7735_PORT.OUTSET = 1<<A0;
    ST7735_PORT.OUTCLR = 1<<CS;         // Before sending data
    spi_send(ST7735_SPI,unData);
    ST7735_PORT.OUTSET = 1<<CS;         // Chip is not selected
}

void ST7735::SendCommand(ST7735_CMD_enum eCommand)
{
    ST7735_PORT.OUTCLR = 1<<CS|1<<A0;   // Before sending data
    spi_send(ST7735_SPI,eCommand);
    ST7735_PORT.OUTSET = 1<<CS;	        // Chip is not selected
}

void ST7735::CaSet(uint8_t unStart, uint8_t unEnd)
{
    /* Send command */
    ST7735_PORT.OUTCLR = 1<<CS|1<<A0;   // Before sending data
    spi_send(ST7735_SPI,ST7735_CMD_CASET);
    /* Send data */
    ST7735_PORT.OUTSET = 1<<A0;
    spi_send(ST7735_SPI,0x00);
    spi_send(ST7735_SPI,COL_ZERO+unStart);
    spi_send(ST7735_SPI,0x00);
    spi_send(ST7735_SPI,COL_ZERO+unEnd);
    
    ST7735_PORT.OUTSET = 1<<CS;
}

void ST7735::RaSet(uint8_t unStart, uint8_t unEnd)
{
    /* Send command */
    ST7735_PORT.OUTCLR = 1<<CS|1<<A0;
    spi_send(ST7735_SPI,ST7735_CMD_RASET);
    /* Send data */
    ST7735_PORT.OUTSET = 1<<A0;
    spi_send(ST7735_SPI,0x00);
    spi_send(ST7735_SPI,ROW_ZERO+unStart);
    spi_send(ST7735_SPI,0x00);
    spi_send(ST7735_SPI,ROW_ZERO+unEnd);
    
    ST7735_PORT.OUTSET = 1<<CS;
}

void ST7735::Dot(uint8_t unRow, uint8_t unCol, COLOR4K_union unionColor)
{
    /* Set COLMOD */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(COLMOD_4K);
    /* Set RAM area */
    CaSet(unCol,unCol);
    RaSet(unRow,unRow);
    /* Send color data */
    SendCommand(ST7735_CMD_RAMRD);
    ST7735_PORT.OUTSET = 1<<A0;
    ST7735_PORT.OUTCLR = 1<<CS;
    spi_send(ST7735_SPI,unionColor.aunColor[1]);
    spi_send(ST7735_SPI,unionColor.aunColor[0]);
    ST7735_PORT.OUTSET = 1<<CS;
}

void ST7735::Dot(uint8_t unRow, uint8_t unCol, COLOR64K_union unionColor)
{
    /* Set COLMOD */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(COLMOD_64K);
    /* Set RAM area */
    CaSet(unCol,unCol);
    RaSet(unRow,unRow);
    /* Send color data */
    SendCommand(ST7735_CMD_RAMRD);
    ST7735_PORT.OUTSET = 1<<A0;
    ST7735_PORT.OUTCLR = 1<<CS;
    spi_send(ST7735_SPI,unionColor.aunColor[1]);
    spi_send(ST7735_SPI,unionColor.aunColor[0]);
    ST7735_PORT.OUTSET = 1<<CS;
}

void ST7735::Dot(uint8_t unRow, uint8_t unCol, COLOR256K_union unionColor)
{
    /* Set COLMOD */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(COLMOD_256K);
    /* Set RAM area */
    CaSet(unCol,unCol);
    RaSet(unRow,unRow);
    /* Send color data */
    SendCommand(ST7735_CMD_RAMRD);
    ST7735_PORT.OUTSET = 1<<A0;
    ST7735_PORT.OUTCLR = 1<<CS;
    spi_send(ST7735_SPI,unionColor.aunColor[2]);
    spi_send(ST7735_SPI,unionColor.aunColor[1]);
    spi_send(ST7735_SPI,unionColor.aunColor[0]);
    ST7735_PORT.OUTSET = 1<<CS;
}

void ST7735::ColorDisplay(COLOR4K_union unionColor)
{
    ColorArea(0,127,0,127,unionColor);
    
}

void ST7735::ColorDisplay(COLOR64K_union unionColor)
{
    ColorArea(0,127,0,127,unionColor);
    
}

void ST7735::ColorDisplay(COLOR256K_union unionColor)
{
    ColorArea(0,127,0,127,unionColor);
}

void ST7735::ColorArea(uint8_t unRowStart, uint8_t unRowEnd, uint8_t unColStart, uint8_t unColEnd, COLOR4K_union unionColor)
{
    /* Set colmod */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(COLMOD_4K);
    
    /* Set RAM area */
    CaSet(unColStart, unColEnd);
    RaSet(unRowStart, unRowEnd);
    
    /* Get number of pixels */
    uint16_t u16PixCount = ((unColEnd - unColStart + 1) * (unRowEnd - unRowStart + 1)>>1);
    
    /* Send color data */
    SendCommand(ST7735_CMD_RAMWR);
    ST7735_PORT.OUTSET = 1<<A0;
    ST7735_PORT.OUTCLR = 1<<CS;
    for(uint16_t i=0; i<u16PixCount; i++)
    {
        spi_send(ST7735_SPI,unionColor.aunColor[1]);
        spi_send(ST7735_SPI,(unionColor.aunColor[0] & 0xF0) | (unionColor.aunColor[1]>>4));
        spi_send(ST7735_SPI,(unionColor.aunColor[1]<<4) | (unionColor.aunColor[0]>>4));
    }
    ST7735_PORT.OUTCLR = 0<<CS;
}

void ST7735::ColorArea(uint8_t unRowStart, uint8_t unRowEnd, uint8_t unColStart, uint8_t unColEnd, COLOR64K_union unionColor)
{
    /* Set colmod */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(COLMOD_64K);
    
    /* Set RAM area */
    CaSet(unColStart, unColEnd);
    RaSet(unRowStart, unRowEnd);
    
    /* Get number of pixels */
    uint16_t u16PixCount = (unColEnd - unColStart + 1) * (unRowEnd - unRowStart + 1);
    
    /* Send color data */
    SendCommand(ST7735_CMD_RAMWR);
    ST7735_PORT.OUTSET = 1<<A0;	
    ST7735_PORT.OUTCLR = 1<<CS;
    for(uint16_t i=0; i<u16PixCount; i++)
    {
        spi_send(ST7735_SPI,unionColor.aunColor[1]);
        spi_send(ST7735_SPI,unionColor.aunColor[0]);
    }
    ST7735_PORT.OUTCLR = 0<<CS;
}

void ST7735::ColorArea(uint8_t unRowStart, uint8_t unRowEnd, uint8_t unColStart, uint8_t unColEnd, COLOR256K_union unionColor)
{
    /* Set colmod */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(COLMOD_256K);
    
    /* Set RAM area */
    CaSet(unColStart, unColEnd);
    RaSet(unRowStart, unRowEnd);
    
    /* Get number of pixels */
    uint16_t u16PixCount = (unColEnd - unColStart + 1) * (unRowEnd - unRowStart + 1);
    
    /* Send color data */
    SendCommand(ST7735_CMD_RAMWR);
    ST7735_PORT.OUTSET = 1<<A0;
    ST7735_PORT.OUTCLR = 1<<CS;
    for(uint16_t i=0; i<u16PixCount; i++)
    {
        spi_send(ST7735_SPI,unionColor.aunColor[2]);
        spi_send(ST7735_SPI,unionColor.aunColor[1]);
        spi_send(ST7735_SPI,unionColor.aunColor[0]);
    }
    ST7735_PORT.OUTCLR = 0<<CS;
}

void ST7735::SendPixelData(uint8_t *punData, uint16_t unLength, MEMORY_enum eMemory)
{
    uint16_t i;
    
    SendCommand(ST7735_CMD_RAMWR);
    ST7735_PORT.OUT = 0<<CS|1<<A0|1<<RES;
    if(eMemory == MEMORY_PROGRAM)
    {
        for(i=0;i<unLength;i++)
        {
            spi_send(ST7735_SPI,pgm_read_byte(punData+i));
        }
    }
    else if(eMemory = MEMORY_RAM)
    {
        for(i=0;i<unLength;i++)
        {
            spi_send(ST7735_SPI,punData[i]);
        }
    }
    ST7735_PORT.OUTSET = 1<<CS;
}

void ST7735::GRAFICcircle(uint8_t unRowPos, uint8_t unColPos, uint8_t unRadius, COLOR4K_union unionColor)
{
    int16_t f = 1-(int16_t)unRadius;
    int16_t ddF_col = 1;
    int16_t ddF_row = (-2)*(int16_t)unRadius;
    uint16_t col = 0;
    uint16_t row = (uint16_t)unRadius;
    
    Dot(unRowPos + unRadius, unColPos, unionColor);
    Dot(unRowPos - unRadius, unColPos, unionColor);
    Dot(unRowPos, unColPos + unRadius, unionColor);
    Dot(unRowPos, unColPos - unRadius, unionColor);
    
    while(col < row) {
        
        if(f >= 0) {
            row--;
            ddF_row += 2;
            f += ddF_row;
        }
        col++;
        ddF_col += 2;
        f += ddF_col;
        Dot(unRowPos + row, unColPos + col, unionColor);
        Dot(unRowPos + row, unColPos - col, unionColor);
        Dot(unRowPos - row, unColPos + col, unionColor);
        Dot(unRowPos - row, unColPos - col, unionColor);
        Dot(unRowPos + col, unColPos + row, unionColor);
        Dot(unRowPos + col, unColPos - row, unionColor);
        Dot(unRowPos - col, unColPos + row, unionColor);
        Dot(unRowPos - col, unColPos - row, unionColor);
    }
}

void ST7735::GRAFICcircle(uint8_t unRowPos, uint8_t unColPos, uint8_t unRadius, COLOR64K_union unionColor)
{
    int16_t f = 1-(int16_t)unRadius;
    int16_t ddF_col = 1;
    int16_t ddF_row = (-2)*(int16_t)unRadius;
    uint16_t col = 0;
    uint16_t row = (uint16_t)unRadius;
    
    Dot(unRowPos + unRadius, unColPos, unionColor);
    Dot(unRowPos - unRadius, unColPos, unionColor);
    Dot(unRowPos, unColPos + unRadius, unionColor);
    Dot(unRowPos, unColPos - unRadius, unionColor);
    
    while(col < row) {
        
        if(f >= 0) {
            row--;
            ddF_row += 2;
            f += ddF_row;
        }
        col++;
        ddF_col += 2;
        f += ddF_col;
        Dot(unRowPos + row, unColPos + col, unionColor);
        Dot(unRowPos + row, unColPos - col, unionColor);
        Dot(unRowPos - row, unColPos + col, unionColor);
        Dot(unRowPos - row, unColPos - col, unionColor);
        Dot(unRowPos + col, unColPos + row, unionColor);
        Dot(unRowPos + col, unColPos - row, unionColor);
        Dot(unRowPos - col, unColPos + row, unionColor);
        Dot(unRowPos - col, unColPos - row, unionColor);
    }
}

void ST7735::GRAFICcircle(uint8_t unRowPos, uint8_t unColPos, uint8_t unRadius, COLOR256K_union unionColor)
{
    int16_t f = 1-(int16_t)unRadius;
    int16_t ddF_col = 1;
    int16_t ddF_row = (-2)*(int16_t)unRadius;
    uint16_t col = 0;
    uint16_t row = (uint16_t)unRadius;
    
    Dot(unRowPos + unRadius, unColPos, unionColor);
    Dot(unRowPos - unRadius, unColPos, unionColor);
    Dot(unRowPos, unColPos + unRadius, unionColor);
    Dot(unRowPos, unColPos - unRadius, unionColor);
    
    while(col < row) {
        
        if(f >= 0) {
            row--;
            ddF_row += 2;
            f += ddF_row;
        }
        col++;
        ddF_col += 2;
        f += ddF_col;
        Dot(unRowPos + row, unColPos + col, unionColor);
        Dot(unRowPos + row, unColPos - col, unionColor);
        Dot(unRowPos - row, unColPos + col, unionColor);
        Dot(unRowPos - row, unColPos - col, unionColor);
        Dot(unRowPos + col, unColPos + row, unionColor);
        Dot(unRowPos + col, unColPos - row, unionColor);
        Dot(unRowPos - col, unColPos + row, unionColor);
        Dot(unRowPos - col, unColPos - row, unionColor);
    }
}

void ST7735::PrintPicture(const uint8_t *punPicture, uint8_t unRowPos, uint8_t unColPos)
{
    /* Set COLMOD */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(pgm_read_byte(punPicture+4));
    
    /* Send data to display */
    RaSet(unRowPos,unRowPos+pgm_read_byte(punPicture)-1);
    CaSet(unColPos,unColPos+pgm_read_byte(punPicture+2)-1);
    SendPixelData((uint8_t*)punPicture+7,pgm_read_word(punPicture+5),MEMORY_PROGRAM);
}

void ST7735::Println(uint8_t *pchText, uint8_t unRowPos, uint8_t unColPos, uint8_t unColClear, COLOR4K_union unionColorActiv, COLOR4K_union unionColorBack)
{
    /*===== FONT STRUCTURE =================================================
    * Font is stored in Program memory
    * pchText[0]        = number of rows
    * pchText[1]        = number of columns
    * pchText[2]        = number of pixel data bytes
    * pchText[3..n]     = pixel data
    *======================================================================*/
    
    uint8_t *pchSign;
    uint16_t unByteCout;
    
    COLOR4K_union unionColorActiv2 = unionColorActiv;
    COLOR4K_union unionColorBack2 = unionColorBack;
    unionColorActiv2.u16Color >>= 4;
    unionColorBack2.u16Color >>= 4;
    
    /* Set COLMOD */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(COLMOD_4K);
    
    while(*pchText && (*pchText != '\r') && (*pchText != '\n'))
    {
        /* Pointer to sign */
        pchSign = (uint8_t *)characters[*pchText - ' '];
        
        /* Count of pixel data byte */
        unByteCout = pgm_read_byte(pchSign+2);
        
        /* Read data to display */
        CaSet(unColPos, unColPos+pgm_read_byte(pchSign+1)-1);
        RaSet(unRowPos, unRowPos+pgm_read_byte(pchSign)-1);
        
        SendCommand(ST7735_CMD_RAMWR);
        ST7735_PORT.OUTSET = 1<<A0;
        ST7735_PORT.OUTCLR = 1<<CS;
        
        for(uint8_t i = 0; i<unByteCout; i++)
                {
                    uint8_t unData = pgm_read_byte(pchSign+3+i);
                    uint8_t unMask = 0x80;
                    for(uint8_t j=0; j<4; j++)
                    {
                        /* Background color */
                        if(!(unData & unMask) && !(unData & (unMask>>1)))
                        {
                            spi_send(ST7735_SPI,unionColorBack.aunColor[1]);
                            spi_send(ST7735_SPI,(unionColorBack.aunColor[0] & 0xF0) | unionColorBack2.aunColor[1]);
                            spi_send(ST7735_SPI,(unionColorBack2.aunColor[0]));
                        }
                        /* Color of character */
                        else if(!(unData & unMask) && (unData & (unMask>>1)))
                        {
                            spi_send(ST7735_SPI,unionColorBack.aunColor[1]);
                            spi_send(ST7735_SPI,(unionColorBack.aunColor[0] & 0xF0) | unionColorActiv2.aunColor[1]);
                            spi_send(ST7735_SPI,(unionColorActiv2.aunColor[0]));
                        }
                        else if((unData & unMask) && !(unData & (unMask>>1)))
                        {
                            
                            spi_send(ST7735_SPI,unionColorActiv.aunColor[1]);
                            spi_send(ST7735_SPI,(unionColorActiv.aunColor[0] & 0xF0) | unionColorBack2.aunColor[1]);
                            spi_send(ST7735_SPI,(unionColorBack2.aunColor[0]));
                            
                        }
                        else
                        {
                            spi_send(ST7735_SPI,unionColorActiv.aunColor[1]);
                            spi_send(ST7735_SPI,(unionColorActiv.aunColor[0] & 0xF0) | unionColorActiv2.aunColor[1]);
                            spi_send(ST7735_SPI,(unionColorActiv.aunColor[0]));
                        }
                        unMask >>= 2;
                    }
                }
    
        
        ST7735_PORT.OUTSET = 1<<CS;			// Chip is not selected
        
        /* Position of next sign, clear space between signs */
        unColPos += pgm_read_byte(pchSign+1)+2;
        //m_cTFT.GreyArea(unRowPos,unRowPos+pgm_read_byte(pchSign)-1,unColPos-2,unColPos-1,COLMOD_2GS,0);
        ColorArea(unRowPos,unRowPos+pgm_read_byte(pchSign)-1,unColPos-2, unColPos-1,unionColorBack);
        pchText++;
    }
    /* Clear space behind text until inColClear */
    if(unColPos <= unColClear){ColorArea(unRowPos,unRowPos+FONT_SIZE-1,unColPos,unColClear,unionColorBack);}
}

void ST7735::Println(uint8_t *pchText, uint8_t unRowPos, uint8_t unColPos, uint8_t unColClear, COLOR64K_union unionColorActiv, COLOR64K_union unionColorBack)
{
    /*===== FONT STRUCTURE =================================================
    * Font is stored in Program memory
    * pchText[0]       = number of rows
    * pchText[1]       = number of columns
    * pchText[2]       = number of pixel data bytes
    * pchText[3..n]    = pixel data
    *======================================================================*/
    
    uint8_t *pchSign;
    uint16_t unByteCout;
    
    /* Set COLMOD */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(COLMOD_64K);
    
    while(*pchText && (*pchText != '\r') && (*pchText != '\n'))
    {
        /* Pointer to sign */
        pchSign = (uint8_t *)characters[*pchText - ' '];
        
        /* Count of pixel data byte */
        unByteCout = pgm_read_byte(pchSign+2);
        
        /* Read data to display */
        CaSet(unColPos, unColPos+pgm_read_byte(pchSign+1)-1);
        RaSet(unRowPos, unRowPos+pgm_read_byte(pchSign)-1);
        
        SendCommand(ST7735_CMD_RAMWR);
        ST7735_PORT.OUTSET = 1<<A0;
        ST7735_PORT.OUTCLR = 1<<CS;
        
        for(uint8_t i = 0; i<unByteCout; i++)
        {
            uint8_t unData = pgm_read_byte(pchSign+3+i);
            uint8_t unMask = 0x80;
            for(uint8_t j=0; j<8; j++)
            {
                /* Background color */
                if((unData & unMask) == 0)
                {
                    spi_send(ST7735_SPI,unionColorBack.aunColor[1]);
                    spi_send(ST7735_SPI,unionColorBack.aunColor[0]);
                }
                /* Color of character */
                else
                {
                    spi_send(ST7735_SPI,unionColorActiv.aunColor[1]);
                    spi_send(ST7735_SPI,unionColorActiv.aunColor[0]);
                }
                unMask >>= 1;
            }
        }
        ST7735_PORT.OUTSET = 1<<CS;     // Chip is not selected
        
        /* Position of next sign, clear space between signs */
        unColPos += pgm_read_byte(pchSign+1)+2;
        //m_cTFT.GreyArea(unRowPos,unRowPos+pgm_read_byte(pchSign)-1,unColPos-2,unColPos-1,COLMOD_2GS,0);
        ColorArea(unRowPos,unRowPos+pgm_read_byte(pchSign)-1,unColPos-2, unColPos-1,unionColorBack);
        pchText++;
    }
    /* Clear space behind text until inColClear */
    if(unColPos <= unColClear){ColorArea(unRowPos,unRowPos+FONT_SIZE-1,unColPos,unColClear,unionColorBack);}
}

void ST7735::Println(uint8_t *pchText, uint8_t unRowPos, uint8_t unColPos, uint8_t unColClear, COLOR256K_union unionColorActiv, COLOR256K_union unionColorBack)
{
    /*===== FONT STRUCTURE =================================================
    * Font is stored in Program memory
    * pchText[0]       = number of rows
    * pchText[1]       = number of columns
    * pchText[2]       = number of pixel data bytes
    * pchText[3..n]    = pixel data
    *======================================================================*/
    
    uint8_t *pchSign;
    uint16_t unByteCout;
    
    /* Set COLMOD */
    SendCommand(ST7735_CMD_COLMOD);
    SendData(COLMOD_256K);
    
    while(*pchText && (*pchText != '\r') && (*pchText != '\n'))
    {
        /* Pointer to sign */
        pchSign = (uint8_t *)characters[*pchText - ' '];
        
        /* Count of pixel data byte */
        unByteCout = pgm_read_byte(pchSign+2);
        
        /* Read data to display */
        CaSet(unColPos, unColPos+pgm_read_byte(pchSign+1)-1);
        RaSet(unRowPos, unRowPos+pgm_read_byte(pchSign)-1);
        
        SendCommand(ST7735_CMD_RAMWR);
        ST7735_PORT.OUTSET = 1<<A0;
        ST7735_PORT.OUTCLR = 1<<CS;
        
        for(uint8_t i = 0; i<unByteCout; i++)
        {
            uint8_t unData = pgm_read_byte(pchSign+3+i);
            uint8_t unMask = 0x80;
            for(uint8_t j=0; j<8; j++)
            {
                /* Background color */
                if((unData & unMask) == 0)
                {
                    spi_send(ST7735_SPI,unionColorBack.aunColor[2]);
                    spi_send(ST7735_SPI,unionColorBack.aunColor[1]);
                    spi_send(ST7735_SPI,unionColorBack.aunColor[0]);
                }
                /* Color of character */
                else
                {
                    spi_send(ST7735_SPI,unionColorActiv.aunColor[2]);
                    spi_send(ST7735_SPI,unionColorActiv.aunColor[1]);
                    spi_send(ST7735_SPI,unionColorActiv.aunColor[0]);
                }
                unMask >>= 1;
            }
        }
        ST7735_PORT.OUTSET = 1<<CS;     // Chip is not selected
        
        /* Position of next sign, clear space between signs */
        unColPos += pgm_read_byte(pchSign+1)+2;
        //m_cTFT.GreyArea(unRowPos,unRowPos+pgm_read_byte(pchSign)-1,unColPos-2,unColPos-1,COLMOD_2GS,0);
        ColorArea(unRowPos,unRowPos+pgm_read_byte(pchSign)-1,unColPos-2, unColPos-1,unionColorBack);
        pchText++;
    }
    /* Clear space behind text until inColClear */
    if(unColPos <= unColClear){ColorArea(unRowPos,unRowPos+FONT_SIZE-1,unColPos,unColClear,unionColorBack);}
}