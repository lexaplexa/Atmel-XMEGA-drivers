/*************************************************************************
 * rf.cpp
 *
 * Created: 11.10.2014 16:19:42
 * Revised: 31.3.2018
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 *
 *************************************************************************/

#include "rf.h"

RF::RF()
{
    /* Setup pins direction */
    RF_PORT.DIRSET = 1<<RF_CE_PIN;      /* Output pins */
    RF_PORT.DIRCLR = 1<<RF_IRQ_PIN;     /* Input pins */
    spi_set_master_port_dir(RF_PORT);
    
    /* Setup IRQ pin */
    RF_PORT.INT0MASK = 1<<RF_IRQ_PIN;
    RF_PORT.INTCTRL = PORT_INT0LVL_HI_gc;
#if RF_IRQ_PIN == PIN0_bp
    RF_PORT.PIN0CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif RF_IRQ_PIN == PIN1_bp
    RF_PORT.PIN1CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif RF_IRQ_PIN == PIN2_bp
    RF_PORT.PIN2CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif RF_IRQ_PIN == PIN3_bp
    RF_PORT.PIN3CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif RF_IRQ_PIN == PIN4_bp
    RF_PORT.PIN4CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif RF_IRQ_PIN == PIN5_bp
    RF_PORT.PIN5CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif RF_IRQ_PIN == PIN6_bp
    RF_PORT.PIN6CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#elif RF_IRQ_PIN == PIN7_bp
    RF_PORT.PIN7CTRL = PORT_ISC_FALLING_gc|PORT_OPC_PULLUP_gc;
#endif
    
    RF_CE_DISABLE;
    spi_cs_disable(RF_PORT);
    
    spi_set_mode(RF_SPI,SPI_MODE_0_gc);
    spi_set_dord(RF_SPI,SPI_DORD_MSB_FIRST);
    
    /* Clear prescaler */
    RF_SPI.CTRL &= ~(SPI_PRESCALER_gm);
    RF_SPI.CTRL &= ~(1<<SPI_CLK2X_bp);
    
    /* Baudrate */
#if RF_BAUD_SPI <= (F_CPU/128)
    RF_SPI.CTRL |= SPI_PRESCALER_DIV128_gc;
#elif RF_BAUD_SPI <= F_CPU/64 
    RF_SPI.CTRL |= SPI_PRESCALER_DIV64_gc;
#elif RF_BAUD_SPI <= F_CPU/32
    RF_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV64_gc;
#elif RF_BAUD_SPI <= F_CPU/16 
    RF_SPI.CTRL |= SPI_PRESCALER_DIV16_gc;
#elif RF_BAUD_SPI <= F_CPU/8 
    RF_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV16_gc;
#elif RF_BAUD_SPI <= F_CPU/4
    RF_SPI.CTRL |= SPI_PRESCALER_DIV4_gc;
#elif RF_BAUD_SPI <= F_CPU/2
    RF_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
#else 
    RF_SPI.CTRL |= 1<<SPI_CLK2X_bp|SPI_PRESCALER_DIV4_gc;
#endif

    spi_enable_master(RF_SPI);
}

void RF::WriteReg(uint8_t unReg, uint8_t unVal)
{
    spi_cs_enable(RF_PORT);
    spi_send(RF_SPI,W_REGISTER|(unReg & 0x1F));
    spi_send(RF_SPI,unVal);
    spi_cs_disable(RF_PORT);
}

uint8_t RF::ReadReg(uint8_t unReg)
{
    uint8_t unRet;
    spi_cs_enable(RF_PORT);
    spi_send(RF_SPI,R_REGISTER|(unReg & 0x1F));
    spi_send(RF_SPI,NO_OPERATION);
    unRet = spi_read(RF_SPI);
    spi_cs_disable(RF_PORT);
    return unRet;
}

uint8_t RF::SendCommand(uint8_t unCommand)
{
    uint8_t unRet;
    spi_cs_enable(RF_PORT);
    spi_send(RF_SPI,unCommand);
    unRet = spi_read(RF_SPI);
    spi_cs_disable(RF_PORT);
    return unRet;
}

void RF::Init()
{
    RF_CE_DISABLE;
    /* Clear Rx and TX FIFO buffer */
    SendCommand(FLUSH_TX);
    SendCommand(FLUSH_RX);
    
    /* Setup Registers */
    
    WriteReg(RF_REG_SETUP_RETR,0x78);                           /* Set delay between retransmissions (bit7-4) and number of max. retransmission (bit3-0) */
    
#if ENABLE_AUTOACK == true  
    WriteReg(RF_REG_EN_AA,DATA_PIPE0_EN<<0|                     /* Enable auto acknowledgment for data pipes */
                          DATA_PIPE1_EN<<1|
                          DATA_PIPE2_EN<<2|
                          DATA_PIPE3_EN<<3|
                          DATA_PIPE4_EN<<4|
                          DATA_PIPE5_EN<<5);
#else
    WriteReg(RF_REG_EN_AA,0x00);                                /* Disable auto acknowledgment for all data pipes */
#endif
    
    WriteReg(RF_REG_EN_RXADDR,DATA_PIPE0_EN<<0|                 /* Enable data pipes */
                              DATA_PIPE1_EN<<1|
                              DATA_PIPE2_EN<<2|
                              DATA_PIPE3_EN<<3|
                              DATA_PIPE4_EN<<4|
                              DATA_PIPE5_EN<<5);
    WriteReg(RF_REG_STATUS,1<<MAX_RT_bp|1<<TX_DS_bp|1<<RX_DR_bp);   /* Clear flags */
    
#if DYNAMIC_PAYLOAD == true
    WriteReg(RF_REG_FEATURE, 1<<EN_DPL_bp|1<<EN_DYN_ACK_bp);    /* Enable features Dynamic payload */
    WriteReg(RF_REG_DYNPD,DATA_PIPE0_EN<<0|                     /* Enable dynamic payload length data pipes */
                          DATA_PIPE1_EN<<1|
                          DATA_PIPE2_EN<<2|
                          DATA_PIPE3_EN<<3|
                          DATA_PIPE4_EN<<4|
                          DATA_PIPE5_EN<<5);

#else
    WriteReg(RF_REG_RX_PW_P0, STATIC_PAYLOAD_BUFFER_LENGTH);    /* Set number of data bytes for pipe. Can by changed. 0 = disabled */
    WriteReg(RF_REG_RX_PW_P1, STATIC_PAYLOAD_BUFFER_LENGTH);    /* Set number of data bytes for pipe. Can by changed. 0 = disabled */
    WriteReg(RF_REG_RX_PW_P2, STATIC_PAYLOAD_BUFFER_LENGTH);    /* Set number of data bytes for pipe. Can by changed. 0 = disabled */
    WriteReg(RF_REG_RX_PW_P3, STATIC_PAYLOAD_BUFFER_LENGTH);    /* Set number of data bytes for pipe. Can by changed. 0 = disabled */
    WriteReg(RF_REG_RX_PW_P4, STATIC_PAYLOAD_BUFFER_LENGTH);    /* Set number of data bytes for pipe. Can by changed. 0 = disabled */
    WriteReg(RF_REG_RX_PW_P5, STATIC_PAYLOAD_BUFFER_LENGTH);    /* Set number of data bytes for pipe. Can by changed. 0 = disabled */
#endif
}

void RF::WakeUp()
{
    WriteReg(RF_REG_CONFIG,0<<MASK_RX_DR_bp|0<<MASK_TX_DS_bp|0<<MASK_MAX_RT_bp|1<<EN_CRC_bp|0<<CRCO_bp|1<<PWR_UP_bp|1<<PRIM_RX_bp);     /* Activate Stand by mode I */
    RF_CE_ENABLE;                                                                                                                       /* Activate Stand by mode II */
    m_eStatus = RF_STATUS_RECEIVING;
}

void RF::Sleep()
{
    RF_CE_DISABLE;                                                                                                                      /* Activate Stand by mode I */
    WriteReg(RF_REG_CONFIG,0<<MASK_RX_DR_bp|0<<MASK_TX_DS_bp|0<<MASK_MAX_RT_bp|1<<EN_CRC_bp|0<<CRCO_bp|0<<PWR_UP_bp|1<<PRIM_RX_bp);     /* Power down */
    m_eStatus = RF_STATUS_SLEEP;
}

void RF::SendMsg(uint8_t *punData, uint8_t unLength)
{
    uint8_t unResult;
    
    RF_CE_DISABLE;
    unResult = ReadReg(RF_REG_CONFIG);
    WriteReg(RF_REG_CONFIG, unResult & ~(1<<PRIM_RX_bp));   /* Set to TX mode */
    
    SendCommand(FLUSH_TX);
    
    /* Write data to TX FIFO */
    spi_cs_enable(RF_PORT);
    spi_send(RF_SPI,W_TX_PAYLOAD);
    for (uint8_t i=0; i<unLength; i++)
    {
        spi_send(RF_SPI,punData[i]);
    }
    m_eStatus = RF_STATUS_SENDING;
    spi_cs_disable(RF_PORT);
    RF_CE_ENABLE;
}

void RF::SetRXaddress(uint8_t *punAddress, uint8_t unLength, uint8_t unPipe)
{
    if (unLength <= 5 && unLength >= 3) {WriteReg(RF_REG_SETUP_AW, unLength-2);}    /* Set address length. Only from 3 to 5 bytes allowed */
    else {return;}
    
    if (unPipe == 0 || unPipe == 1)
    {
        /* Set all RX address bytes */
        spi_cs_enable(RF_PORT);
        spi_send(RF_SPI,W_REGISTER|((RF_REG_RX_ADDR_P0+unPipe) & 0x1F));
        for(uint8_t i=0; i<unLength; i++)
        {
            /* From LSB to MSB */
            spi_send(RF_SPI,punAddress[i]);
        }
        spi_cs_disable(RF_PORT);
    }
    else if (unPipe >= 2 && unPipe <= 5)
    {
        WriteReg(RF_REG_RX_ADDR_P0+unPipe, *punAddress);    /* only LSB byte */
    }
}

void RF::SetTXaddress(uint8_t *punAddress, uint8_t unLength)
{
    if (unLength <= 5 && unLength >= 3) {WriteReg(RF_REG_SETUP_AW, unLength-2);}    /* Set address length. Only from 3 to 5 bytes allowed */
    else {return;}
    
    /* Set TX address */
    spi_cs_enable(RF_PORT);
    spi_send(RF_SPI,W_REGISTER|(RF_REG_TX_ADDR & 0x1F));
    for(uint8_t i=0; i<unLength; i++) 
    {
        /* From LSB to MSB */
        spi_send(RF_SPI,punAddress[i]);
    }
    spi_cs_disable(RF_PORT);
}

void RF::SetChannel(uint8_t unChNumber)
{
    if (unChNumber >= RF_CHANNEL_TOTAL) {return;}
    WriteReg(RF_REG_RF_CH, unChNumber);
    m_unChannel = unChNumber;
}

void RF::SetPowerTx(RF_PWR_enum ePwr)
{
    uint8_t unRet;
    if (ePwr == RF_PWR_0dBm_gc || ePwr == RF_PWR_minus6dBm_gc || ePwr == RF_PWR_minus12dBm_gc || ePwr == RF_PWR_minus18dBm_gc)
    {
        unRet = ReadReg(RF_REG_RF_SETUP);
        WriteReg(RF_REG_RF_SETUP,(unRet & ~RF_PWR_bm)|ePwr);
        m_ePwrTx = ePwr;
    }
    else {return;}
}

void RF::SetBaud(RF_BAUD_enum eBaud)
{
    uint8_t unRet;
    
    if (eBaud == RF_BAUD_250kbps_gc || eBaud == RF_BAUD_1Mbps_gc || eBaud == RF_BAUD_2Mbps_gc)
    {
        unRet = ReadReg(RF_REG_RF_SETUP);
        WriteReg(RF_REG_RF_SETUP,(unRet & ~RF_DR_bm)|eBaud);
        m_eBaud = eBaud;
    }
    else {return;}
}

