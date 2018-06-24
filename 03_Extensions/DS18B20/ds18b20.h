/****************************************************************************
 * ds18b20.h
 *
 * Created: 18.4.2017 16:44:17
 * Revised: 24.6.2018
 * Author: LeXa
 * BOARD:
 * ABOUT:
 *
 *****************************************************************************/


#ifndef DS18B20_H_
#define DS18B20_H_

#include <core/drivers/usart.h>
#include <conf/conf_board.h>


/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/
#ifndef DS18B20_PORT
    #define DS18B20_PORT            PORTF
#endif

#ifndef DS18B20_USART
    #define DS18B20_USART           USARTF0
#endif


/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void ds18b20_init()
{
    usart_port_config(DS18B20_PORT,PIN3_bp,PIN2_bp);
    usart_config(DS18B20_USART,USART_CHSIZE_8BIT_gc,USART_PMODE_DISABLED_gc);
    usart_set_interrupt(DS18B20_USART,USART_TXCINTLVL_OFF_gc,USART_RXCINTLVL_OFF_gc,USART_DREINTLVL_OFF_gc);
    usart_tx_enable(DS18B20_USART);
    usart_rx_enable(DS18B20_USART);
}

bool ds18b20_write(uint8_t *pData, uint8_t unDataLen)
{
    uint8_t unRet;
    
    DS18B20_PORT.PIN3CTRL = PORT_OPC_WIREDAND_gc;
    
    /* Reset device and presence test */
    usart_rx_enable(DS18B20_USART);
    usart_baud(DS18B20_USART,F_CPU,9600);
    usart_send(DS18B20_USART,0xF0);
    usart_read(DS18B20_USART,unRet);
    if (unRet == 0xF0) {return false;}
    
    /* Set faster baud rate */
    usart_baud(DS18B20_USART,F_CPU,115200);
    usart_rx_disable(DS18B20_USART);
    DS18B20_PORT.PIN3CTRL = PORT_OPC_TOTEM_gc;
    
    /* Data write */
    for (uint8_t byte=0; byte<unDataLen; byte++)
    {
        for (uint8_t i=0; i<8; i++)
        {
            if (pData[byte] & (1<<i))   {usart_send(DS18B20_USART,0xFF);}
            else                        {usart_send(DS18B20_USART,0x00);}
        }
    }
    
    return true;
}

bool ds18b20_read(uint8_t *pDataToWrite, uint8_t unDataWriteLen, uint8_t *pDataToRead, uint8_t unDataReadLen)
{
    uint8_t unRet;
    
    DS18B20_PORT.PIN3CTRL = PORT_OPC_WIREDAND_gc;
    
    /* Reset device and presence test */
    usart_rx_enable(DS18B20_USART);
    usart_baud(DS18B20_USART,F_CPU,9600);
    usart_send(DS18B20_USART,0xF0);
    usart_read(DS18B20_USART,unRet);
    if (unRet == 0xF0) {return false;}
    
    /* Set faster baud rate */
    usart_baud(DS18B20_USART,F_CPU,115200);
    usart_rx_disable(DS18B20_USART);
    DS18B20_PORT.PIN3CTRL = PORT_OPC_TOTEM_gc;
    
    /* Data write */
    for (uint8_t byte=0; byte<unDataWriteLen; byte++)
    {
        for (uint8_t i=0; i<8; i++)
        {
            if (pDataToWrite[byte] & (1<<i))    {usart_send(DS18B20_USART,0xFF);}
            else                                {usart_send(DS18B20_USART,0x00);}
        }
    }
    
    /* Data read */
    DS18B20_PORT.PIN3CTRL = PORT_OPC_WIREDAND_gc;
    usart_rx_enable(DS18B20_USART);
    for (uint8_t byte=0; byte<unDataReadLen; byte++)
    {
        for (uint8_t i=0; i<8; i++)
        {
            usart_send(DS18B20_USART,0xFF);
            usart_read(DS18B20_USART,unRet);
            if (unRet == 0xFF) {pDataToRead[byte] |= 1<<i;}
            else {pDataToRead[byte] &= ~(1<<i);}
        }
    }
    return true;
}

#endif /* 1WIRE_UART_H_ */