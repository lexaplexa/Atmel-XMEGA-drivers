/*************************************************************************
 * mcp2515.cpp
 *
 * Created: 9.10.2014 13:55:41
 * Revised: 1.4.2018
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * CAN transceiver MCP2515
 *************************************************************************/

#include "mcp2515.h"

MCP2515::MCP2515()
{
    Init();
}

void MCP2515::Init()
{
    /* Reset on */
    MCP_PORT.OUTCLR = 1<<MCP_RESET_PIN;
    
    /* Set PORT direction for functional pins */
    MCP_PORT.DIRSET = 1<<MCP_RESET_PIN;     /* Output pins */
    MCP_PORT.DIRCLR = 1<<MCP_INT_PIN;       /* Input pins */
    
    /* Reset off after start */
    MCP_PORT.OUTSET = 1<<MCP_RESET_PIN;
    
    /* Setup SPI driver */
    spi_set_master_port_dir(MCP_PORT);
    spi_cs_disable(MCP_PORT);
    spi_set_mode(MCP_SPI,SPI_MODE_0_gc);
    spi_set_dord(MCP_SPI,SPI_DORD_MSB_FIRST);
    spi_enable_master(MCP_SPI);
    
    /* Set SPI Baudrate */
#if (MCP_BAUD <= F_CPU/128)
    MCP_SPI.CTRL |= SPI_PRESCALER_DIV128_gc;
#elif (MCP_BAUD <= F_CPU/64)
    MCP_SPI.CTRL |= SPI_PRESCALER_DIV64_gc;
#elif (MCP_BAUD <= F_CPU/32)
    MCP_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV64_gc;
#elif (MCP_BAUD <= F_CPU/16)
    MCP_SPI.CTRL |= SPI_PRESCALER_DIV16_gc;
#elif (MCP_BAUD <= F_CPU/8)
    MCP_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV16_gc;
#elif (MCP_BAUD <= F_CPU/4)
    MCP_SPI.CTRL |= SPI_PRESCALER_DIV4_gc;
#elif (MCP_BAUD <= F_CPU/2)
    MCP_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
#else
    MCP_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
#endif
    
    /* Set interrupt on pin MCP_INT */
    MCP_PORT.INT0MASK = 1<<MCP_INT_PIN;
    MCP_PORT.INTCTRL = PORT_INT0LVL_HI_gc;
#if MCP_INT_PIN == 0
    MCP_PORT.PIN0CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif MCP_INT_PIN == 1
    MCP_PORT.PIN1CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif MCP_INT_PIN == 2
    MCP_PORT.PIN2CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif MCP_INT_PIN == 3
    MCP_PORT.PIN3CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif MCP_INT_PIN == 4
    MCP_PORT.PIN4CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif MCP_INT_PIN == 5
    MCP_PORT.PIN5CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif MCP_INT_PIN == 6
    MCP_PORT.PIN6CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif MCP_INT_PIN == 7
    MCP_PORT.PIN7CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#endif
    
    /* Set Configuration mode */
    WriteReg(CANCTRL,CANCTRL_REQOP_CONFIGURATION_MODE);
    
    /* Set Interrupt register */
    WriteReg(CANINTE,(0<<CANINTE_MERRIE_bp|
                      1<<CANINTE_WAKIE_bp|
                      0<<CANINTE_ERRIE_bp|
                      0<<CANINTE_TX2IE_bp|
                      0<<CANINTE_TX1IE_bp|
                      0<<CANINTE_TX0IE_bp|
                      1<<CANINTE_RX1IE_bp|
                      1<<CANINTE_RX0IE_bp));
    
    /* Set CAN bit timing */
    spi_cs_enable(MCP_PORT);
    spi_send(MCP_SPI,INST_WRITE);       /* Instruction */
    spi_send(MCP_SPI,CNF3);             /* Address */
    spi_send(MCP_SPI,0<<CNF3_SOF_bp|0<<CNF3_WAKFIL_bp|CNF3_PHSEG2_16TQ);                        /* CNF3 */
    spi_send(MCP_SPI,0<<CNF2_BTLMODE_bp|0<<CNF2_SAM_bp|CNF2_PHSEG1_16TQ|CNF2_PRGSEG_16TQ);      /* CNF2 */
    spi_send(MCP_SPI,CNF1_SWJ_1TQ|CNF1_BRP_500k_16TQ);                                          /* CNF1 */
    spi_cs_disable(MCP_PORT);
    
    /* Set Normal mode and CLKOUT enable */
    WriteReg(CANCTRL,CANCTRL_REQOP_NORMAL_MODE|1<<CANCTRL_CLKEN_bp|CANCTRL_CLKPRE_FCLKOUT_DIV1);
}

uint8_t MCP2515::ReadReg(uint8_t unRegAddress)
{
    uint8_t unVal;
    
    spi_cs_enable(MCP_PORT);
    spi_send(MCP_SPI,INST_READ);            /* Instruction */
    spi_send(MCP_SPI,unRegAddress);         /* Address */
    spi_send(MCP_SPI,NO_OPERATION);
    unVal = spi_read(MCP_SPI);              /* Read value in register */
    spi_cs_disable(MCP_PORT);
    
    return unVal;
}

void MCP2515::WriteReg(uint8_t unRegAddress, uint8_t unVal)
{
    spi_cs_enable(MCP_PORT);
    spi_send(MCP_SPI,INST_WRITE);           /* Instruction */
    spi_send(MCP_SPI,unRegAddress);         /* Address */
    spi_send(MCP_SPI,unVal);                /* Write value to register */
    spi_cs_disable(MCP_PORT);
}

bool MCP2515::SendCANmsg(CAN_MSG_t *psCanmsg)
{
    uint8_t unCounter=0, TxBuf;
    
    /* Wait until one of TX buffers is ready to load can message */
    do
    {
        if (!(ReadReg(TXBCTRL(0)) & (1<<TXBCTRL_TXREQ_bp))) {TxBuf = 0; break;}
        else if (!(ReadReg(TXBCTRL(1)) & (1<<TXBCTRL_TXREQ_bp))) {TxBuf = 2; break;}
        else if (!(ReadReg(TXBCTRL(2)) & (1<<TXBCTRL_TXREQ_bp))) {TxBuf = 4; break;}
        if (unCounter++ > 200)
        {
            /* Abort TX transmissions, because it's taking long time */
            WriteReg(TXBCTRL(0),~(1<<TXBCTRL_TXREQ_bp));
            WriteReg(TXBCTRL(1),~(1<<TXBCTRL_TXREQ_bp));
            WriteReg(TXBCTRL(2),~(1<<TXBCTRL_TXREQ_bp));
            return false;
        }
    } while (1);
    
    /* LOAD DATA TO TX BUFFER STANDARD ID */
    if (psCanmsg->id < 0x800)
    {
        spi_cs_enable(MCP_PORT);
        spi_send(MCP_SPI,INST_LOAD_TX0_BUF_ID+TxBuf);
        spi_send(MCP_SPI,(uint8_t)(psCanmsg->id>>3));                                   /* SIDH */
        spi_send(MCP_SPI,(uint8_t)(psCanmsg->id<<5));                                   /* SIDL */
        spi_send(MCP_SPI,0);                                                            /* EIDH */
        spi_send(MCP_SPI,0);                                                            /* EIDL */
        spi_send(MCP_SPI,psCanmsg->dlc);                                                /* DLC */
        for (uint8_t i=0; i<psCanmsg->dlc; i++)	{spi_send(MCP_SPI,psCanmsg->data[i]);}  /* DATA */
        spi_cs_disable(MCP_PORT);
    }
    /* LOAD DATA TO TX BUFFER EXTENDED ID */
    else
    {
        spi_cs_enable(MCP_PORT);
        spi_send(MCP_SPI,INST_LOAD_TX0_BUF_ID+TxBuf);
        spi_send(MCP_SPI,(uint8_t)(psCanmsg->id>>21));                                      /* SIDH */
        spi_send(MCP_SPI,(uint8_t)(psCanmsg->id<<13)|0x08|(uint8_t)(psCanmsg->id>>16));     /* SIDL */
        spi_send(MCP_SPI,(uint8_t)(psCanmsg->id>>8));                                       /* EIDH */
        spi_send(MCP_SPI,(uint8_t)(psCanmsg->id>>0));                                       /* EIDL */
        spi_send(MCP_SPI,psCanmsg->dlc);                                                    /* DLC */
        for (uint8_t i=0; i<psCanmsg->dlc; i++)	{spi_send(MCP_SPI,psCanmsg->data[i]);}      /* DATA */
        spi_cs_disable(MCP_PORT);
    }
    
    /* SEND MESSAGE */
    spi_cs_enable(MCP_PORT);
    spi_send(MCP_SPI,INST_RTS((TxBuf>>1)));
    spi_cs_disable(MCP_PORT);
    return true;
}

void MCP2515::ReadCANmsg(RXTXBUFFER_enum eRxBuf, CAN_MSG_t *psCanmsg)
{
    uint8_t id[4];
    
    /* READ RX OR TX BUFFER REGISTERS */
    spi_cs_enable(MCP_PORT);
    if (eRxBuf == TX0_BUFFER) {spi_send(MCP_SPI,INST_READ); spi_send(MCP_SPI,TXBSIDH(0));}
    else if (eRxBuf == TX1_BUFFER) {spi_send(MCP_SPI,INST_READ); spi_send(MCP_SPI,TXBSIDH(1));}
    else if (eRxBuf == TX1_BUFFER) {spi_send(MCP_SPI,INST_READ); spi_send(MCP_SPI,TXBSIDH(2));}
    else if (eRxBuf == RX0_BUFFER) {spi_send(MCP_SPI,INST_READ_RX0_BUF_ID);}
    else if (eRxBuf == RX1_BUFFER) {spi_send(MCP_SPI,INST_READ_RX1_BUF_ID);}
    else {spi_cs_disable(MCP_PORT); return;}
    
    /* READ CAN MESSAGE */	
    spi_send(MCP_SPI,NO_OPERATION);
    id[0] = spi_read(MCP_SPI);                              /* SIDH */
    spi_send(MCP_SPI,NO_OPERATION);
    id[1] = spi_read(MCP_SPI);                              /* SIDL */
    spi_send(MCP_SPI,NO_OPERATION);
    id[2] = spi_read(MCP_SPI);                              /* EIDH */
    spi_send(MCP_SPI,NO_OPERATION);
    id[3] = spi_read(MCP_SPI);                              /* EIDL */
    spi_send(MCP_SPI,NO_OPERATION);
    psCanmsg->dlc = spi_read(MCP_SPI) & 0x0F;               /* DLC */
    for (uint8_t i=0; i<psCanmsg->dlc; i++)
    {
        spi_send(MCP_SPI,NO_OPERATION);
        psCanmsg->data[i] = spi_read(MCP_SPI);              /* DATA */
    }
    spi_cs_disable(MCP_PORT);
    
    /* EXTENDED ID */
    if (id[1] & 0x08)
    {
        psCanmsg->id =  (uint32_t)(id[0])<<21|              /* SIDH */
                        (uint32_t)(id[1] & 0xE0)<<13|       /* SIDL */
                        (uint32_t)(id[1] & 0x03)<<16|       /* SIDL */
                        (uint32_t)(id[2])<<8|               /* EIDH */
                        (uint32_t)(id[3])<<0;               /* EIDL */
    }
    /* STANDARD ID */
    else
    {
        psCanmsg->id =  (uint32_t)(id[0]<<3)|
                        (uint32_t)(id[1]>>5);
    }
}

void MCP2515::BitModify(uint8_t unReg, uint8_t unMask, uint8_t unBits)
{
    spi_cs_enable(MCP_PORT);
    spi_send(MCP_SPI,INST_BIT_MODIFY);
    spi_send(MCP_SPI,unMask);
    spi_send(MCP_SPI,unBits);
    spi_cs_disable(MCP_PORT);
}

void MCP2515::SetFilter(uint8_t unRxBuf, uint32_t unMask, uint8_t unRegRxfSidH, uint32_t unID)
{
    /* Set Configuration mode */
    WriteReg(CANCTRL,CANCTRL_REQOP_CONFIGURATION_MODE);
    
    /* STANDARD ID */
    if (unID < 0x800)
    {
        /* Set RX filter */
        WriteReg(RXBCTRL(unRxBuf),RXBCTRL_RXM_STDID_ONLY_gc);
        /* Set message mask */
        spi_cs_enable(MCP_PORT);
        spi_send(MCP_SPI,INST_WRITE);
        spi_send(MCP_SPI,RXMSIDH(unRxBuf));
        spi_send(MCP_SPI,(uint8_t)(unMask>>3)); /* SIDH */
        spi_send(MCP_SPI,(uint8_t)(unMask<<5)); /* SIDL */
        spi_send(MCP_SPI,0);                    /* EIDH */
        spi_send(MCP_SPI,0);                    /* EIDL */
        spi_cs_disable(MCP_PORT);
        
        /* Set message filter */
        spi_cs_enable(MCP_PORT);
        spi_send(MCP_SPI,INST_WRITE);
        spi_send(MCP_SPI,unRegRxfSidH);
        spi_send(MCP_SPI,(uint8_t)(unID>>3));   /* SIDH */
        spi_send(MCP_SPI,(uint8_t)(unID<<5));   /* SIDL */
        spi_send(MCP_SPI,0);                    /* EIDH */
        spi_send(MCP_SPI,0);                    /* EIDL */
        spi_cs_disable(MCP_PORT);
    }
    /* EXTENDED ID */
    else
    {
        /* Set RX filter */
        WriteReg(RXBCTRL(unRxBuf),RXBCTRL_RXM_EXTID_ONLY_gc);
        /* Set message mask */
        spi_cs_enable(MCP_PORT);
        spi_send(MCP_SPI,INST_WRITE);
        spi_send(MCP_SPI,RXMSIDH(unRxBuf));
        spi_send(MCP_SPI,(uint8_t)(unMask>>21));                            /* SIDH */
        spi_send(MCP_SPI,(uint8_t)(unMask<<13)|0x08|(uint8_t)(unMask>>16)); /* SIDL */
        spi_send(MCP_SPI,(uint8_t)(unMask>>8));                             /* EIDH */
        spi_send(MCP_SPI,(uint8_t)(unMask>>0));                             /* EIDL */
        spi_cs_disable(MCP_PORT);
        
        /* Set message filter */
        spi_cs_enable(MCP_PORT);
        spi_send(MCP_SPI,INST_WRITE);
        spi_send(MCP_SPI,unRegRxfSidH);
        spi_send(MCP_SPI,(uint8_t)(unID>>21));                              /* SIDH */
        spi_send(MCP_SPI,(uint8_t)(unID<<13)|0x08|(uint8_t)(unID>>16));     /* SIDL */
        spi_send(MCP_SPI,(uint8_t)(unID>>8));                               /* EIDH */
        spi_send(MCP_SPI,(uint8_t)(unID>>0));                               /* EIDL */
        spi_cs_disable(MCP_PORT);
    }
    
    /* Set Normal mode */
    WriteReg(CANCTRL,CANCTRL_REQOP_NORMAL_MODE);
}