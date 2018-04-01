/*************************************************************************
 * display.h
 *
 * Created: 27.5.2015 23:09:35
 * Revised: 24.2.2017
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 * Driver for EPSON TFT display 16 grey (A2C00892300-01)
 *************************************************************************/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <conf/conf_board.h>


#ifndef DISPLAY_H_
#define DISPLAY_H_

/*===== COMPILATOR SWITCHES ==================================================
 * Uncomment if needed.
 *==========================================================================*/
#ifndef CONF_BOARD_H_ 
    //#define COMPILE_DISP_PRINT_FROM_FILE
#endif

#ifdef COMPILE_DISP_PRINT_FILE
    #include <modules/filesystem/fs.h>
#endif

/*===== HARDWARE CONNECTION ================================================*/

#ifndef CONF_BOARD_H_
    #define DISP_DATA_PORT      PORTF       /* Data port */
    #define DISP_CMD_PORT       PORTE       /* Command port */
    #define DISP_XCS_PIN        PIN4_bp     /* Chip select signal (LOW=selected, HIGH=not selected) */
    #define DISP_XWR_PIN        PIN5_bp     /* Writing control signal (write on rising edge) */
    #define DISP_A0_PIN         PIN6_bp     /* Data command select signal (Data=HIGH, Command=LOW) */
    #define DISP_XRES_PIN       PIN7_bp     /* Reset. Initialized when set to LOW */
#endif

/*===== COMMANDS ===========================================================*/

enum DISP_CMD_enum {
    DISP_CMD_NOP        = 0x00,     /* No operation */
    DISP_CMD_SWRESET    = 0x01,     /* Software reset */
    DISP_CMD_RDDSDR     = 0x0F,     /* Read display self diagnostic results */
    DISP_CMD_SLPIN      = 0x10,     /* Sleep in */
    DISP_CMD_SLPOUT     = 0x11,     /* Sleep out */
    DISP_CMD_NORON      = 0x13,     /* Normal display */
    DISP_CMD_DISINOFF   = 0x20,     /* Display inversion off */
    DISP_CMD_DISINV     = 0x21,     /* Display inversion on */
    DISP_CMD_DISPOFF    = 0x28,     /* Display off */
    DISP_CMD_DISPON     = 0x29,     /* Display on */
    DISP_CMD_CASET      = 0x2A,     /* Column address set */
    DISP_CMD_PASET      = 0x2B,     /* Row address set */
    DISP_CMD_RAMWR      = 0x2C,     /* RAM write */
    DISP_CMD_RAMRD      = 0x2E,     /* RAM read */
    DISP_CMD_TEOFF      = 0x34,     /* Tearing output off */
    DISP_CMD_TEON       = 0x35,     /* Tearing output on */
    DISP_CMD_MADCTL     = 0x36,     /* Ram address control */
    DISP_CMD_COLMOD     = 0x3A,     /* I/F format */
};

/*===== MADCTL ===============================================================
 * In normal display mode zero positions on Epson TFT display are not actually zeros. Zero position on screen is
 * not same as zero address in RAM. Visible screen is in middle of RAM. Zero position on screen depends on which
 * parameter of command MADCTL is used.
 * Parameter bits:
 * Bit5 - 0 vertical screen; 1 horizontal screen;
 * Bit6 - 0 column address (0h-140h, from left to right); 1 column address (140h-0h, from right to left)
 * Bit7 - 0 page(row) address (0h-140h, from top to bottom); 1 page(row) address (140h-0h, from bottom to top)
 *==========================================================================*/

#define MADCTL_P 0x60

/* VERTICAL SCREEN */
#if (MADCTL_P == 0x00)      /* writing to screen begins in TOP LEFT corner */
    #define ROW_ZERO        0
    #define ROW_END         319
    #define COL_ZERO        40
    #define COL_END         279
#elif (MADCTL_P == 0x40)    /* writing to screen begins in TOP RIGHT corner */
    #define ROW_ZERO        0
    #define ROW_END         319
    #define COL_ZERO        41
    #define COL_END         280
#elif (MADCTL_P == 0x80)    /* writing to screen begins in BOTTOM LEFT corner */
    #define ROW_ZERO        1
    #define ROW_END         320
    #define COL_ZERO        40
    #define COL_END         279
#elif (MADCTL_P == 0xC0)    /* writing to screen begins in BOTTOM RIGHT corner */
    #define ROW_ZERO        1
    #define ROW_END         320
    #define COL_ZERO        41
    #define COL_END         280

/* HORIZONTAL SCREEN */
#elif (MADCTL_P == 0x20)    /* writing to screen begins in TOP LEFT corner */
    #define ROW_ZERO        40
    #define ROW_END         279
    #define COL_ZERO        0
    #define COL_END         319
#elif (MADCTL_P == 0x60)    /* writing to screen begins in TOP RIGHT corner */
    #define ROW_ZERO        41
    #define ROW_END         280
    #define COL_ZERO        0
    #define COL_END         319
#elif (MADCTL_P == 0xA0)    /* writing to screen begins in BOTTOM LEFT corner */
    #define ROW_ZERO        40
    #define ROW_END         279
    #define COL_ZERO        1
    #define COL_END         320
#elif (MADCTL_P == 0xE0)    /* writing to screen begins in BOTTOM RIGHT corner */
    #define ROW_ZERO        41
    #define ROW_END         280
    #define COL_ZERO        1
    #define COL_END         320
#endif

/*===== COLMOD ===============================================================
 * Display driver has 3 gray scale modes. Every pixel has 4 bits assigned in memory. 
 * In 2GS mode are only 2 states, 1pixel = 1bit (0000,1111 in memory). 
 * In 4GS mode are 4 states, 1pixel = 2bits (0000,0011,1100,1111 in memory).
 * In 16GS mode are 16 states, 1pixel = 4bits (0000,0001,...,1110,1111 in memory).
 * 0000 - non transparent
 * 1111 - total transparent
 * Parameter COLMOD:
 * 0x00 - 2G/S (black and white)
 * 0x01 - 4G/S (4 level gray scale)
 * 0x03 - 16G/S (16 level gray scale)
 *==========================================================================*/

enum COLMOD_enum {
    COLMOD_2GS  = 0,
    COLMOD_4GS  = 1,
    COLMOD_16GS = 3,
};

/*===== MEMORY =============================================================
 * There are different memory types from which can be read data to display.
 *==========================================================================*/

enum MEMORY_enum {
    MEMORY_RAM,
    MEMORY_PROGRAM
};

/************************************************************************/
/* FUNCTION DECLARATION                                                 */
/************************************************************************/

    /**
     * \brief   Display initialization 
     * 
     * \return  void
     */
    void DispInit();

        
    /**
     * \brief   Send data to display controller
     * 
     * \param   unData      - data to be send
     * 
     * \return  void
     */
    inline void DispSendData(uint8_t unData)
    {
        DISP_CMD_PORT.OUT = 0<<DISP_XCS_PIN|1<<DISP_A0_PIN|1<<DISP_XWR_PIN|1<<DISP_XRES_PIN;    /* Before sending data */
        
        DISP_DATA_PORT.OUT = unData;            /* Sending data */
        DISP_CMD_PORT.OUTCLR = 1<<DISP_XWR_PIN; /* XWR set LOW */
        DISP_CMD_PORT.OUTSET = 1<<DISP_XWR_PIN; /* XWR set HIGH */
    
        DISP_CMD_PORT.OUTSET = 1<<DISP_XCS_PIN; /* Chip is not selected */
    };
        
    
    /**
    * \brief    Send Command to display controller
    * 
    * \param    eCommand    - command to be send
    * 
    * \return   void
    */
    inline void DispSendCommand(DISP_CMD_enum eCommand)
    {
        DISP_CMD_PORT.OUT = 0<<DISP_XCS_PIN|0<<DISP_A0_PIN|1<<DISP_XWR_PIN|1<<DISP_XRES_PIN;    /* Before sending data */
        
        DISP_DATA_PORT.OUT = eCommand;          /* Sending command */
        DISP_CMD_PORT.OUTCLR = 1<<DISP_XWR_PIN; /* XWR set LOW */
        DISP_CMD_PORT.OUTSET = 1<<DISP_XWR_PIN; /* XWR set HIGH */
    
        DISP_CMD_PORT.OUTSET = 1<<DISP_XCS_PIN; /* Chip is not selected */
    };
    
    
    /**
    * \brief    Set column area for sending pixel data
    * 
    * \param    unStart     - start column
    * \param    unEnd       - end column (including)
    * 
    * \return   void
    */
    inline void CaSet(uint16_t unStart, uint16_t unEnd)
    {
        /* Move to position on visible screen area */
        unStart += COL_ZERO;
        unEnd += COL_ZERO;
        
        DispSendCommand(DISP_CMD_CASET);
        DispSendData((uint8_t) (unStart>>8));       /* upper data byte */
        DispSendData((uint8_t) (unStart));          /* lower data byte */
        DispSendData((uint8_t) (unEnd>>8));         /* upper data byte */
        DispSendData((uint8_t) (unEnd));            /* lower data byte */
    };	
    
    
    /**
    * \brief    Set row area for sending pixel data
    * 
    * \param    unStart     - start row
    * \param    unEnd       - end row (including)
    * 
    * \return	void
    */
    inline void PaSet(uint16_t unStart, uint16_t unEnd)
    {
        /* Move to position on visible screen area */
        unStart += ROW_ZERO;
        unEnd += ROW_ZERO;
        
        DispSendCommand(DISP_CMD_PASET);
        DispSendData((uint8_t) (unStart>>8));       /* upper data byte */
        DispSendData((uint8_t) (unStart));          /* lower data byte */
        DispSendData((uint8_t) (unEnd>>8));         /* upper data byte */
        DispSendData((uint8_t) (unEnd));            /* lower data byte */
    };
    
    
    /**
    * \brief    Send pixel data to visible screen. You must use PaSet and CaSet before
    * 
    * \param    punData     - pointer to data buffer
    * \param    unLength    - number of data bytes
    * \param    eMemory     - type of memory
    * 
    * \return   void
    */
    inline void SendPixelData(uint8_t *punData, uint16_t unLength, MEMORY_enum eMemory)
    {
        uint16_t i;
        
        DispSendCommand(DISP_CMD_RAMWR);
        DISP_CMD_PORT.OUT = 0<<DISP_XCS_PIN|1<<DISP_A0_PIN|1<<DISP_XWR_PIN|1<<DISP_XRES_PIN;    /* Before sending data */
        
        if (eMemory == MEMORY_PROGRAM)
        {
            for(i=0;i<unLength;i++)
            {
                DISP_DATA_PORT.OUT = pgm_read_byte(punData+i);  /* Read data from program memory */
                DISP_CMD_PORT.OUTCLR = 1<<DISP_XWR_PIN;         /* XWR set LOW */
                DISP_CMD_PORT.OUTSET = 1<<DISP_XWR_PIN;         /* XWR set HIGH */
            }
        }
        else if (eMemory == MEMORY_RAM)
        {
            for(i=0;i<unLength;i++)
            {
                DISP_DATA_PORT.OUT = punData[i];            /* Read data from RAM */
                DISP_CMD_PORT.OUTCLR = 1<<DISP_XWR_PIN;     /* XWR set LOW */
                DISP_CMD_PORT.OUTSET = 1<<DISP_XWR_PIN;     /* XWR set HIGH */
            }
        }
        
        DISP_CMD_PORT.OUTSET = 1<<DISP_XCS_PIN;                 /* Chip is not selected */
    };
    
    
    /**
    * \brief    Print dot on visible display screen
    * 
    * \param    unRow           - row position
    * \param    unCol           - column position
    * \param    eColmod         - Colmod parameter
    * \param    unGrayLevel     - Grey level
    * 
    * \return   void
    */
    void DispDot(uint16_t unRow, uint16_t unCol, COLMOD_enum eColmod, uint8_t unGrayLevel);
    
    
    /**
    * \brief    Set grey scale area on visible screen
    * 
    * \param    unRowStart      - start row position
    * \param    unRowEnd        - end row position (including)
    * \param    unColStart      - start column position
    * \param    unColEnd        - end column position (including)
    * \param    eColmod         - Colmod parameter
    * \param    unGrayLevel     - Grey level
    * 
    * \return   void
    */
    void DispGreyArea(uint16_t unRowStart, uint16_t unRowEnd, uint16_t unColStart, uint16_t unColEnd, COLMOD_enum eColmod, uint8_t unGrayLevel);
        
    
    /**
    * \brief    Print text on screen
    * 
    * \param    pchText     - pointer to text
    * \param    paFont      - pointer to font
    * \param    unRowPos    - row position (left top corner)
    * \param    unColPos    - column position (left top corner)
    * \param    unColClear  - column position for clear rest of the text line
    * 
    * \return   void
    */
    void DispPrintln(uint8_t *pchText, uint8_t **paFont, uint16_t unRowPos, uint16_t unColPos, uint16_t unColClear);
    
    
    /**
    * \brief    picture on screen. Picture loaded from program memory
    * 
    * \param    punPicture  - pointer to picture in program memory
    * \param    unRowPos    - row position (left top corner)
    * \param    unColPos    - column position (left top corner)
    * 
    * \return	void
    */
    void DispPrintPicture(const uint8_t *punPicture, uint16_t unRowPos, uint16_t unColPos);
    
        
#ifdef COMPILE_DISP_PRINT_FROM_FILE
    /**
        * \brief    Print picture on screen. Picture loaded from file system
        * 
        * \param    sFile       - pointer to file handler
        * \param    unRowPos    - row position (left top corner)
        * \param    unColPos    - column position (left top corner)
        * 
        * \return   void
        */
    void DispPrintPicture(FILE_HANDLER_struct *sFile, uint16_t unRowPos, uint16_t unColPos);
#endif

#endif /* DISPLAY_H_ */