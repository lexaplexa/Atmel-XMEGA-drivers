/*************************************************************************
 * mfrc522.cpp
 *
 * Created: 13.1.2016 15:50:10
 * Revised: 1.4.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#include "mfrc522.h"

MFRC522::MFRC522()
{
    /* Set Reset pin */
    MFRC522_PORT.OUTCLR = 1<<MFRC522_RESET_PIN;     /* Reset on */
    MFRC522_PORT.DIRSET = 1<<MFRC522_RESET_PIN;     /* Set as output */
    MFRC522_PORT.OUTSET = 1<<MFRC522_RESET_PIN;     /* Reset off */
    
    /* Setup SPI driver */
    spi_set_master_port_dir(MFRC522_PORT);
    spi_cs_disable(MFRC522_PORT);
    spi_set_mode(MFRC522_SPI,SPI_MODE_0_gc);
    spi_set_dord(MFRC522_SPI,SPI_DORD_MSB_FIRST);
    spi_enable_master(MFRC522_SPI); 
        
    /* Set SPI Baudrate */
    #if (MFRC522_BAUD <= F_CPU/128)
        MFRC522_SPI.CTRL |= SPI_PRESCALER_DIV128_gc;
    #elif (MFRC522_BAUD <= F_CPU/64)
        MFRC522_SPI.CTRL |= SPI_PRESCALER_DIV64_gc;
    #elif (MFRC522_BAUD <= F_CPU/32)
        MFRC522_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV64_gc;
    #elif (MFRC522_BAUD <= F_CPU/16)
        MFRC522_SPI.CTRL |= SPI_PRESCALER_DIV16_gc;
    #elif (MFRC522_BAUD <= F_CPU/8)
        MFRC522_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV16_gc;
    #elif (MFRC522_BAUD <= F_CPU/4)
        MFRC522_SPI.CTRL |= SPI_PRESCALER_DIV4_gc;
    #elif (MFRC522_BAUD <= F_CPU/2)
        MFRC522_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
    #else
        MFRC522_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
    #endif  
    
    /* Set interrupt on pin MFRC522_IRQ_PIN */
    MFRC522_PORT.INT0MASK = 1<<MFRC522_IRQ_PIN;
    MFRC522_PORT.INTCTRL = PORT_INT0LVL_HI_gc;
    #if MFRC522_IRQ_PIN == 0
        MFRC522_PORT.PIN0CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
    #elif MFRC522_IRQ_PIN == 1
        MFRC522_PORT.PIN1CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
    #elif MFRC522_IRQ_PIN == 2
        MFRC522_PORT.PIN2CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
    #elif MFRC522_IRQ_PIN == 3
        MFRC522_PORT.PIN3CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
    #elif MFRC522_IRQ_PIN == 4
        MFRC522_PORT.PIN4CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
    #elif MFRC522_IRQ_PIN == 5
        MFRC522_PORT.PIN5CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
    #elif MFRC522_IRQ_PIN == 6
        MFRC522_PORT.PIN6CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
    #elif MFRC522_IRQ_PIN == 7
        MFRC522_PORT.PIN7CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
    #endif
    
    for (uint16_t i=0; i<0x7FFF; i++);              /* Wait routine until oscillator is ready. t >= 37.74us */
    
    WriteReg(REG_CommandReg,COMM_SoftReset);        /* Soft Reset */
    
    WriteReg(REG_TModeReg, 0x80);                   /* TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds   */
    WriteReg(REG_TPrescalerReg, 0xA9);              /* TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25us.        */
    WriteReg(REG_TReloadRegH, 0x03);                /* Reload timer with 0x3E8 = 1000, ie 25ms before timeout.                                                       */
    WriteReg(REG_TReloadRegL, 0xe8);                
    
    WriteReg(REG_TxASKReg, 0x40);                   /* Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting                      */
    WriteReg(REG_ModeReg, 0x3D);                    /* Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4) */
    
    AntennaOn();
}

inline uint8_t MFRC522::ReadReg(REG_enum eReg)
{
    uint8_t unValue;
    
    spi_cs_enable(MFRC522_PORT);
    spi_send(MFRC522_SPI,(eReg<<1)|READ_REG_SIG);       /* Register address with READ signature */
    spi_send(MFRC522_SPI,0xFF);                         /* No operation, next clock cycle */
    unValue = spi_read(MFRC522_SPI);                    /* Read value in register */
    spi_cs_disable(MFRC522_PORT);
    
    return unValue;
}

inline void MFRC522::ReadReg(REG_enum eReg, uint8_t unNum, uint8_t *punBuffer)
{
    uint8_t unAddress;
    unAddress = (uint8_t)eReg;
    spi_cs_enable(MFRC522_PORT);
    spi_send(MFRC522_SPI,((unAddress++)<<1)|READ_REG_SIG);      /* Register address with READ signature */
    for (unNum; unNum>0; unNum--)
    {
        spi_send(MFRC522_SPI,((unAddress++)<<1)|READ_REG_SIG);  /* Next address, next clock cycle */
        *(punBuffer++) = spi_read(MFRC522_SPI);                 /* Read value in register */
    }
    spi_cs_disable(MFRC522_PORT);
    return;
}

inline void MFRC522::WriteReg(REG_enum eReg, uint8_t unData)
{
    spi_cs_enable(MFRC522_PORT);
    spi_send(MFRC522_SPI,(eReg<<1)&~WRITE_REG_SIG);             /* Register address with WRITE signature */
    spi_send(MFRC522_SPI,unData);                               /* Send data */
    spi_cs_disable(MFRC522_PORT);
}

inline void MFRC522::WriteReg(REG_enum eReg, uint8_t *unData, uint8_t unLen)
{
    spi_cs_enable(MFRC522_PORT);
    spi_send(MFRC522_SPI,(eReg<<1)&~WRITE_REG_SIG);             /* Register address with WRITE signature */
    for (uint8_t i=0; i<unLen; i++) {spi_send(MFRC522_SPI,*(unData+i));}    /* Send data */
    spi_cs_disable(MFRC522_PORT);
}

inline void MFRC522::SetRegBitMask(REG_enum eReg, uint8_t unBitMask)
{
    uint8_t unValue;
    
    spi_cs_enable(MFRC522_PORT);
    spi_send(MFRC522_SPI,(eReg<<1)|READ_REG_SIG);               /* Register address with READ signature */
    spi_send(MFRC522_SPI,0xFF);                                 /* No operation, next clock cycle */
    unValue = spi_read(MFRC522_SPI);                            /* Read value in register */
    spi_cs_disable(MFRC522_PORT);
    
    unValue |= unBitMask;
    
    spi_cs_enable(MFRC522_PORT);
    spi_send(MFRC522_SPI,(eReg<<1)&~WRITE_REG_SIG);             /* Register address with WRITE signature */
    spi_send(MFRC522_SPI,unValue);                              /* Send data */
    spi_cs_disable(MFRC522_PORT);
}

inline void MFRC522::ClrRegBitMask(REG_enum eReg, uint8_t unBitMask)
{
    uint8_t unValue;
    
    spi_cs_enable(MFRC522_PORT);
    spi_send(MFRC522_SPI,(eReg<<1)|READ_REG_SIG);               /* Register address with READ signature */
    spi_send(MFRC522_SPI,0xFF);                                 /* No operation, next clock cycle */
    unValue = spi_read(MFRC522_SPI);                            /* Read value in register */
    spi_cs_disable(MFRC522_PORT);
    
    unValue &= ~unBitMask;
    
    spi_cs_enable(MFRC522_PORT);
    spi_send(MFRC522_SPI,(eReg<<1)&~WRITE_REG_SIG);             /* Register address with WRITE signature */
    spi_send(MFRC522_SPI,unValue);                              /* Send data */
    spi_cs_disable(MFRC522_PORT);
}

void MFRC522::AntennaOn()
{
    uint8_t unVal;
    
    unVal = ReadReg(REG_TxControlReg);
    if (!(unVal & 0x03)) {WriteReg(REG_TxControlReg,(unVal | 0x03));}
}

void MFRC522::AntennaOff()
{
    uint8_t unVal;
    
    unVal = ReadReg(REG_TxControlReg);
    if (unVal & 0x03) {WriteReg(REG_TxControlReg,(unVal & ~(0x03)));}
}
uint8_t unVal;
STATUS_enum MFRC522::SendCommand(COMM_enum eComm, uint8_t *punDatatoSend, uint8_t unLen, uint8_t *punReturnData, uint8_t *punReturnLen, uint8_t *punValidBits)
{
    uint16_t unDelay = 2000;
    
    
    *punReturnLen = 0;
    WriteReg(REG_CommandReg, COMM_Idle);                        /* Stop running command                                 */
    WriteReg(REG_ComIrqReg, 0x7F);                              /* Clear all seven interrupt request bits               */
    SetRegBitMask(REG_FIFOLevelReg, 0x80);                      /* FlushBuffer = 1, FIFO initialization                 */
    WriteReg(REG_FIFODataReg, punDatatoSend, unLen);            /* Write sendData to the FIFO                           */
    WriteReg(REG_BitFramingReg, *punValidBits & 0x07);          /* Bit adjustments                                      */
    WriteReg(REG_CommandReg, eComm);                            /* Execute the command                                  */
    if (eComm == COMM_Transceive)
    {
        SetRegBitMask(REG_BitFramingReg,0x80);                  /* Start transmission of data                           */  
    }
    
    /* Wait until transmission is complete */
    while (1)
    {
        unVal = ReadReg(REG_ComIrqReg);
        if (unVal & 0x30) {break;}                              /* RxIRq and IdleIRq                                    */
        if (unVal & 0x01) {return STATUS_TIMEOUT;}              /* Timer timeout                                        */
        if (!--unDelay) {return STATUS_TIMEOUT;}                /* Test Timeout                                         */
    }
    
    unVal = ReadReg(REG_ErrorReg);
    if (unVal & 0x13) {return STATUS_ERROR;}                    /* BufferOvfl ParityErr ProtocolErr                     */
    if (unVal & 0x08) {return STATUS_COLLISION;}                /* CollErr                                              */
    
    *punReturnLen = ReadReg(REG_FIFOLevelReg);                  /* Read RX FIFO length                                  */
    ReadReg(REG_FIFODataReg,*punReturnLen,punReturnData);
    *punValidBits = ReadReg(REG_ControlReg) & 0x07;             /* Number of valid bits in last received byte           */
    return STATUS_OK;
}


STATUS_enum MFRC522::CalculateCRC(uint8_t *punDatatoSend, uint8_t unLen, uint8_t *punReturnDat)
{
    uint16_t unDelay = 2000;
    uint8_t unVal;
    
    WriteReg(REG_CommandReg, COMM_Idle);                /* Stop any active command.                                     */
    WriteReg(REG_DivIrqReg, 0x04);                      /* Clear the CRCIRq interrupt request bit                       */
    SetRegBitMask(REG_FIFOLevelReg, 0x80);              /* FlushBuffer = 1, FIFO initialization                         */
    WriteReg(REG_FIFODataReg, punDatatoSend, unLen);    /* Write data to the FIFO                                       */
    WriteReg(REG_CommandReg, COMM_CalcCRC);             /* Start the calculation                                        */
    
    /* Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73µs. */
    while (1) 
    {
        unVal = ReadReg(REG_DivIrqReg);                 /* DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq   reserved CRCIRq reserved reserved                          */
        if (unVal & 0x04) {break;}                      /* CRCIRq bit set - calculation done                                                                                        */
        if (--unDelay == 0) {return STATUS_TIMEOUT;}    /* The emergency break. We will eventually terminate on this one after 89ms. Communication with the MFRC522 might be down.  */
    }
    
    WriteReg(REG_CommandReg, COMM_Idle);                /* Stop calculating CRC for new content in the FIFO.                                                                        */
    
    /* Transfer the result from the registers to the result buffer  */
    punReturnDat[0] = ReadReg((REG_enum)(REG_CRCResultReg+1));
    punReturnDat[1] = ReadReg(REG_CRCResultReg);
    return STATUS_OK;
}